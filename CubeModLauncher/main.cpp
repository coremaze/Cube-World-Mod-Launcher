#include "main.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include "Process.h"
#include "crc.h"

#define CUBE_VERSION "1.0.0-0"
#define CUBE_CRC 0xDC91320A

using namespace std;

bool FileExists(const char* fileName) {
    DWORD dwAttrib = GetFileAttributes(fileName);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int main() {
    //Cube world is obviously required
    if (!FileExists("Cube.exe")) {
        printf("Cube World not found.\n");
        Sleep(1000);
        return 1;
    }

    unsigned int checksum = crc32_file("Cube.exe");
    if (checksum != CUBE_CRC) {
        printf("Cube World was found, but it is not version %s.\n"
               "(Found CRC %08X, expected %08X)\n"
               "Please update your game.\n",
               CUBE_VERSION, checksum, CUBE_CRC);
        printf("Press enter to exit.\n");
        cin.ignore();
        return 1;
    }

    //Inject our dll
    if ( !FileExists("CubeModLoader.dll") ) {
        printf("Callback manager not found.\n");
        Sleep(1000);
        return 1;
    }

    Process process("Cube.exe");

    //Create game in suspended state
    printf("Starting Cube.exe...\n\n");
    if (!process.Create()) {
        printf("Failed to create process: %lu", GetLastError());
        return 1;
    } else {
        printf("Cube.exe was successfully started.\n\n");
    }

    process.InjectDLL( std::string("CubeModLoader.dll") );

    // Need to give the loader some time to work
    // This is a horrible thing and probably will result in a race condition please help me
    Sleep(250);

    // Let Cube World run!
    process.Run();

    Sleep(3000);

    return 0;
}
