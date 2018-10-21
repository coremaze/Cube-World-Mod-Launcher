#include <iostream>
#include <windows.h>
#include <vector>


using namespace std;

bool FileExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);
  return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int main()
{
    vector <std::string> modDLLs;

    //The callback manager is required.
    if ( !FileExists("CallbackManager.dll") ){
        printf("Callback manager not found.\n");
        Sleep(1000);
        return 1;
    }

    modDLLs.push_back( std::string("CallbackManager.dll") );
    const char MOD_PATH[] = "Mods\\*.dll";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    CreateDirectory("Mods", NULL);

    //Create game in suspended state
    printf("Starting Cube.exe...\n\n");
    if (!CreateProcess(NULL,
                  "Cube.exe",
                  NULL,
                  NULL,
                  true,
                  CREATE_SUSPENDED,
                  NULL,
                  NULL,
                  &si,
                  &pi))
    {
        printf("Failed to create process: %lu", GetLastError());
        return 1;
    }
    else {
        printf("Cube.exe was successfully started.\n\n");
    }


    //Find mods
    HANDLE hFind;
    WIN32_FIND_DATA data;

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

    printf("\nAll available mods have been loaded.\n");
    Sleep(1500);
//
//
//    WaitForSingleObject(pi.hThread, INFINITE);
//    for (HANDLE thread : threads){
//        CloseHandle(thread);
//    }
    return 0;
}
