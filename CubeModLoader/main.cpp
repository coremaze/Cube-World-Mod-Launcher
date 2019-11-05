#include "main.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include "DLL.h"
#include "crc.h"
#include "mutex.h"

#define MOD_MAJOR_VERSION 5
#define MOD_MINOR_VERSION 1

#define CUBE_VERSION "1.0.0-1"
#define CUBE_PACKED_CRC 0xC7682619
#define CUBE_UNPACKED_CRC 0xBA092543

#define MODLOADER_NAME "CubeModLoader"


using namespace std;

global void* base; // Module base
global vector <DLL*> modDLLs; // Every mod we've loaded
global HMODULE hSelf; // A handle to ourself, to prevent being unloaded
global void** initterm_eReference; // A pointer-pointer to a function which is run extremely soon after starting, or after being unpacked
GETTER_VAR(void*, initterm_e); // A pointer to that function

#include "callbacks/ChatHandler.h"
#include "callbacks/P2PRequestHandler.h"
#include "callbacks/CheckInventoryFullHandler.h"
#include "callbacks/GameTickHandler.h"

void SetupHandlers() {
    SetupChatHandler();
    SetupP2PRequestHandler();
    SetupCheckInventoryFullHandler();
	SetupGameTickHandler();
}


// Handles injecting callbacks and the mods
bool already_loaded_mods = false;
mutex already_loaded_mods_mtx;
extern "C" void StartMods() {
    char msg[256] = {0};

    already_loaded_mods_mtx.lock();
    // Don't allow this to run more than once
    if (already_loaded_mods) {
        already_loaded_mods_mtx.unlock();
        return;
    }
    already_loaded_mods = true;
    already_loaded_mods_mtx.unlock();

    SetupHandlers();

    //Find mods
    HANDLE hFind;
    WIN32_FIND_DATA data;

    CreateDirectory("Mods", NULL);
    hFind = FindFirstFile("Mods\\*.dll", &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // We should be loaded into the application's address space, so we can just LoadLibraryA
            DLL* dll = new DLL(string("Mods\\") + data.cFileName);
            dll->Load();
            printf("Loaded %s\n", dll->fileName.c_str());
            modDLLs.push_back(dll);
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    }

    // Find all the functions the mods may export
    for (DLL* dll: modDLLs) {
        MUST_IMPORT(dll, ModMajorVersion);
        MUST_IMPORT(dll, ModMinorVersion);
        MUST_IMPORT(dll, ModPreInitialize);
		MUST_IMPORT(dll, MakeMod);
    }

    // Ensure version compatibility
	for (DLL* dll : modDLLs) {
		int majorVersion = ((int(*)())dll->ModMajorVersion)();
		int minorVersion = ((int(*)())dll->ModMinorVersion)();
		if (majorVersion != MOD_MAJOR_VERSION) {
			sprintf(msg, "%s has major version %d but requires %d.\n", dll->fileName.c_str(), majorVersion, MOD_MAJOR_VERSION);
			Popup("Error", msg);
			exit(1);

			if (minorVersion > MOD_MINOR_VERSION) {
				sprintf(msg, "%s has minor version %d but requires %d or lower.\n", dll->fileName.c_str(), minorVersion, MOD_MINOR_VERSION);
				Popup("Error", msg);
				exit(1);
			}
		}
	}

    // Run Initialization routines on all mods
    for (DLL* dll: modDLLs) {
        ((void(*)())dll->ModPreInitialize)();
		dll->mod = ((GenericMod*(*)())dll->MakeMod)();
    }

    for (DLL* dll: modDLLs) {
		dll->mod->Initialize();
    }
    
    if (hSelf) PrintLoadedMods();
    return;
}


void ASMStartMods() {
    asm(".intel_syntax \n"
		PUSH_ALL
        PREPARE_STACK

        // Initialize mods and callbacks
        "call StartMods \n"

        RESTORE_STACK
        POP_ALL

        // Run initterm_e properly this time.
		DEREF_JMP(initterm_e)
        );
}

void PatchFreeImage(){
    // Patch FreeImage, because Windows 8 and higher do not work properly with it.
    DWORD oldProtect;
    void* patchaddr = Offset(GetModuleHandleA("FreeImage.dll"), 0x1E8C4E);
    VirtualProtect((LPVOID)patchaddr, 9, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset(patchaddr, 0x90, 9);
    VirtualProtect((LPVOID)patchaddr, 9, oldProtect, &oldProtect);

    patchaddr = Offset(patchaddr, 0x14);
    VirtualProtect((LPVOID)patchaddr, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset(patchaddr, 0x90, 14);
    VirtualProtect((LPVOID)patchaddr, 14, oldProtect, &oldProtect);
}

void PatchInitterm_ePtr() {
    // Get ** to initterm_e
    initterm_eReference = (void**)(Offset(base, 0x42CBD8));

    initterm_e = *initterm_eReference;

    DWORD oldProtect;
    VirtualProtect((LPVOID)initterm_eReference, 8, PAGE_EXECUTE_READWRITE, &oldProtect);
    *initterm_eReference = (void*)&ASMStartMods;
    VirtualProtect((LPVOID)initterm_eReference, 8, oldProtect, &oldProtect);
}

void Popup(const char* title, const char* msg ) {
    MessageBoxA(0, msg, title, MB_OK | MB_ICONINFORMATION);
}

void PrintLoadedMods() {
    std::string mods("Mods Loaded:\n");
    for (DLL* dll : modDLLs) {
        mods += dll->fileName;
        mods += "\n";
    }
    Popup("Loaded Mods", mods.c_str());
}

void WriteFarJMP(void* source, void* destination) {
    DWORD dwOldProtection;
    VirtualProtect(source, 14, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    char* location = (char*)source;

    // Far jump
    *((UINT16*)&location[0]) = 0x25FF;

    // mode
    *((UINT32*)&location[2]) = 0x00000000;

    *((UINT64*)&location[6]) = (UINT64)destination;

    VirtualProtect(location, 14, dwOldProtection, &dwOldProtection);
}

void* Offset(void* x1, uint64_t x2) {
	return (void*)((char*)x1 + x2);
}

bool already_initialized = false;
mutex already_initialized_mtx;
extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:


        already_initialized_mtx.lock();
        if (already_initialized) {
            already_initialized_mtx.unlock();
            return true;
        }
        already_initialized = true;
        already_initialized_mtx.unlock();

        base = GetModuleHandle(NULL);

        char msg[256] = {0};

        // This serves to prevent ourself from being unloaded, if we are a .fip
        hSelf = LoadLibrary(MODLOADER_NAME ".fip");

        // The user could also inject this as a DLL, so if we can't find our .fip, we were probably launched with a launcher.
        // Therefore, we don't need to prompt the user.
        if (hSelf) {
            if (MessageBoxA(NULL, "Would you like to run with mods?", "Cube World Mod Loader", MB_YESNO) != IDYES) {
                PatchFreeImage();
                return true;
            }
        }

        base = GetModuleHandle(NULL);


        // Figure out where the executable is and verify its checksum
        char cubePath[_MAX_PATH+1];
        GetModuleFileName(NULL, cubePath, _MAX_PATH);

        uint32_t checksum = crc32_file(cubePath);
        if (checksum == CUBE_PACKED_CRC || checksum == CUBE_UNPACKED_CRC) {
            // Patch some code to run StartMods. This method makes it work with AND without SteamStub.
            PatchInitterm_ePtr();
        } else {
            sprintf(msg, "%s does not seem to be version %s. CRC %08X", cubePath, CUBE_VERSION, checksum);
            Popup("Error", msg);
            PatchFreeImage();
            return true;
        }

        Sleep(250);
        PatchFreeImage();
    }
    return true;
}
