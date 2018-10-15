#include <iostream>
#include <windows.h>
#include <vector>

using namespace std;

int main()
{
    const char MOD_PATH[] = "Mods\\*.dll";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    //Create game in suspended state
    if (!CreateProcess(NULL,
                  "Cube.exe",
                  NULL,
                  NULL,
                  FALSE,
                  CREATE_SUSPENDED,
                  NULL,
                  NULL,
                  &si,
                  &pi))
    {
        printf("Failed to create process: %lu", GetLastError());
        return 1;
    }


    //Find mods
    HANDLE hFind;
    WIN32_FIND_DATA data;
    vector <std::string> modDLLs;
    hFind = FindFirstFile(MOD_PATH, &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            modDLLs.push_back( std::string("Mods\\") + data.cFileName);
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    }



    //Inject DLLs
    vector <HANDLE> threads;
    for (string S_DLLName : modDLLs){
        printf("Loading %s\n", S_DLLName.c_str());

        LPVOID load_library = (LPVOID) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
        LPVOID remote_string = (LPVOID) VirtualAllocEx(pi.hProcess, NULL, strlen(S_DLLName.c_str()) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        WriteProcessMemory(pi.hProcess, remote_string, S_DLLName.c_str(), strlen(S_DLLName.c_str()) + 1, NULL);

        HANDLE thread = CreateRemoteThread(pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE) load_library, remote_string, CREATE_SUSPENDED, NULL);
        threads.push_back(thread);
        ResumeThread(thread);
    }

    ResumeThread(pi.hThread);
    CloseHandle(pi.hProcess);


    WaitForSingleObject(pi.hThread, INFINITE);
    for (HANDLE thread : threads){
        CloseHandle(thread);
    }
    return 0;
}
