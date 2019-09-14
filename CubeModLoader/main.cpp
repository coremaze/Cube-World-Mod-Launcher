#include <iostream>
#include <windows.h>
#include <vector>
#include "DLL.h"

#define MOD_MAJOR_VERSION 1
#define MOD_MINOR_VERSION 1


#define no_optimize __attribute__((optimize("O0")))

#define MUST_IMPORT(dllname, name)\
dllname->name = GetProcAddress(dllname->handle, #name);\
            if (!dllname->name) {\
                printf("%s does not export " #name ".\n", dllname->fileName.c_str());\
                exit(1);\
            }

#define IMPORT(dllname, name)\
dllname->name = GetProcAddress(dllname->handle, #name);


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



// Example handler
void NumberHandler(int number) {
    for (DLL* dll: modDLLs) {
        if (dll->HandleNumber) {
            ((void(*)(int))dll->HandleNumber)(number);
        }
    }
}
void* NumberHandler_ptr = (void*)&NumberHandler;

void* ASMNumberHandler_jmpback;
char ASMNumberHandler_formatstring[] = "%d...\n";
void no_optimize ASMNumberHandler() {
    asm("call rax \n"
        "mov eax, [rbp-4] \n"
        "push rax\n push rcx\n push rdx\n push r8\n push r9\n push r10\n push r11\n"

        "sub rsp, 0x20 \n"
        "mov ecx, eax \n"
        "call [NumberHandler_ptr] \n"
        "add rsp, 0x20 \n"

        "pop r11\n pop r10\n pop r9\n pop r8\n pop rdx\n pop rcx\n pop rax\n"

        "mov edx, eax \n"
        "lea rcx, [ASMNumberHandler_formatstring] \n"
        "jmp [ASMNumberHandler_jmpback]"
       );
}
void SetupNumberHandler() {
    WriteFarJMP(base+0x1597, (void*)&ASMNumberHandler);
    ASMNumberHandler_jmpback = (void*)base+0x15A5;
}


void SetupHandlers() {
    SetupNumberHandler();
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        base = GetModuleHandle(NULL);

        SetupHandlers();

        MessageBoxA(0, "hello!", "DLL Message", MB_OK | MB_ICONINFORMATION);

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
            IMPORT(dll, HandleNumber);
        }

        // Ensure version compatibility
        for (DLL* dll: modDLLs) {
            int majorVersion = ((int(*)())dll->ModMajorVersion)();
            int minorVersion = ((int(*)())dll->ModMinorVersion)();
            if (majorVersion != MOD_MAJOR_VERSION) {
                printf("%s has major version %d but requires %d.\n", dll->fileName.c_str(), majorVersion, MOD_MAJOR_VERSION);
                exit(1);
            }

            if (minorVersion > MOD_MINOR_VERSION) {
                printf("%s has minor version %d but requires %d or lower.\n", dll->fileName.c_str(), minorVersion, MOD_MINOR_VERSION);
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
