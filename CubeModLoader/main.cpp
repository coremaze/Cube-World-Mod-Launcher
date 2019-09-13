#include <iostream>
#include <windows.h>
#include <vector>

using namespace std;

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(0, "hello!", "DLL Message", MB_OK | MB_ICONINFORMATION);

        vector <std::string> modDLLs;

        //Find mods
        HANDLE hFind;
        WIN32_FIND_DATA data;

        CreateDirectory("Mods", NULL);
        hFind = FindFirstFile("Mods\\*.dll", &data);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                modDLLs.push_back( std::string("Mods\\") + data.cFileName);
            } while (FindNextFile(hFind, &data));
            FindClose(hFind);
        }

        // We should be loaded into the application's address space, so we can just LoadLibraryA
        for (std::string modName : modDLLs) {
            LoadLibraryA(modName.c_str());
        }
    }
    return true;
}
