#include "main.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include "Process.h"
#include "crc.h"

#define CUBE_VERSION "1.0.0-0"
#define CUBE_CRC 0x00000000

using namespace std;

bool FileExists(const char* fileName) {
    DWORD dwAttrib = GetFileAttributes(fileName);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int main()
{
    vector <std::string> modDLLs;

    //Cube world is obviously required
    if (!FileExists("Cube.exe")){
        printf("Cube World not found.\n");
        Sleep(1000);
        return 1;
    }

    unsigned int checksum = crc32_file("Cube.exe");
    if (checksum != CUBE_CRC){
        printf("Cube World was found, but it is not version %s.\n"
               "(Found CRC %08X, expected %08X)\n"
               "Please update your game.\n",
               CUBE_VERSION, checksum, CUBE_CRC);
        printf("Press enter to exit.\n");
        cin.ignore();
        return 1;
    }

    //The callback manager is required.
    if ( !FileExists("CallbackManager.dll") ){
        printf("Callback manager not found.\n");
        Sleep(1000);
        return 1;
    }

    modDLLs.push_back( std::string("CallbackManager.dll") );

    Process process("Cube.exe");

    //Create game in suspended state
    printf("Starting Cube.exe...\n\n");
    if (!process.Create())
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

    CreateDirectory("Mods", NULL);
    hFind = FindFirstFile("Mods\\*.dll", &data);
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
        process.InjectDLL(S_DLLName);
    }

    process.Run();

    printf("\nAll available mods have been loaded.\n");
    Sleep(1500);

    return 0;
}
