#include <iostream>
#include <windows.h>
#include <vector>
#include "DLL.h"

#define MOD_MAJOR_VERSION 4
#define MOD_MINOR_VERSION 1


#define no_optimize __attribute__((optimize("O0")))

#define MUST_IMPORT(dllname, name)\
dllname->name = GetProcAddress(dllname->handle, #name);\
            if (!dllname->name) {\
                char ERROR_MESSAGE_POPUP[512] = {0};\
                sprintf(ERROR_MESSAGE_POPUP, "%s does not export " #name ".\n", dllname->fileName.c_str());\
                Popup("Error", ERROR_MESSAGE_POPUP);\
                exit(1);\
            }

#define IMPORT(dllname, name)\
dllname->name = GetProcAddress(dllname->handle, #name);

#define PUSH_ALL "push rax\npush rbx\npush rcx\npush rdx\npush rsi\npush rdi\npush rbp\npush r8\npush r9\npush r10\npush r11\npush r12\npush r13\npush r14\npush r15\n"
#define POP_ALL "pop r15\npop r14\npop r13\npop r12\npop r11\npop r10\npop r9\npop r8\npop rbp\npop rdi\npop rsi\npop rdx\npop rcx\npop rbx\npop rax\n"

#define PREPARE_STACK "mov rax, rsp \n and rsp, 0xFFFFFFFFFFFFFFF0 \n push rax \n sub rsp, 0x28 \n"
#define RESTORE_STACK "add rsp, 0x28 \n pop rsp \n"


using namespace std;

void* base;
vector <DLL*> modDLLs;

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

#include "callbacks/ChatHandler.h"
#include "callbacks/P2PRequestHandler.h"

void SetupHandlers() {
    SetupChatHandler();
    SetupP2PRequestHandler();
}

void Popup(const char* title, char* msg ){
    MessageBoxA(0, msg, title, MB_OK | MB_ICONINFORMATION);
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        base = GetModuleHandle(NULL);

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
            IMPORT(dll, ModInitialize);
            IMPORT(dll, HandleChat);
            IMPORT(dll, HandleP2PRequest);
        }

        // Ensure version compatibility
        char msg[512] = {0};
        for (DLL* dll: modDLLs) {
            int majorVersion = ((int(*)())dll->ModMajorVersion)();
            int minorVersion = ((int(*)())dll->ModMinorVersion)();
            if (majorVersion != MOD_MAJOR_VERSION) {
                sprintf(msg, "%s has major version %d but requires %d.\n", dll->fileName.c_str(), majorVersion, MOD_MAJOR_VERSION);
                Popup("Error", msg);
                exit(1);
            }

            if (minorVersion > MOD_MINOR_VERSION) {
                sprintf(msg, "%s has minor version %d but requires %d or lower.\n", dll->fileName.c_str(), minorVersion, MOD_MINOR_VERSION);
                Popup("Error", msg);
                exit(1);
            }
        }

        // Run Initialization routines on all mods
        for (DLL* dll: modDLLs) {
            ((void(*)())dll->ModPreInitialize)();
        }

        for (DLL* dll: modDLLs) {
            if (dll->ModInitialize) {
                ((void(*)())dll->ModInitialize)();
            }
        }

    }
    return true;
}
