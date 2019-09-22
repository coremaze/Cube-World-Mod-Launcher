#include "Process.h"

#include <windows.h>

using namespace std;

Process::Process(string path)
{
    this->path = path;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
}

Process::~Process()
{
    //dtor
}

bool Process::InjectDLL(string dllName) {
    LPVOID load_library = (LPVOID) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
    LPVOID remote_string = (LPVOID) VirtualAllocEx(pi.hProcess, NULL, strlen(dllName.c_str()) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(pi.hProcess, remote_string, dllName.c_str(), strlen(dllName.c_str()) + 1, NULL);
    HANDLE thread = CreateRemoteThread(pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE) load_library, remote_string, CREATE_SUSPENDED, NULL);
    ResumeThread(thread);
    return true;
}
bool Process::Create() {
    return CreateProcess(NULL,
                  (char*)path.c_str(),
                  NULL,
                  NULL,
                  true,
                  CREATE_SUSPENDED,
                  NULL,
                  NULL,
                  &si,
                  &pi);
}

void Process::Run() {
    ResumeThread(pi.hThread);
    CloseHandle(pi.hProcess);
}
