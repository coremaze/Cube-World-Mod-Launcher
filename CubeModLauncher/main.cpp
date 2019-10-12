#include "main.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include "Process.h"
#include "crc.h"

#define CUBE_VERSION "1.0.0-1"
#define CUBE_PACKED_CRC 0xC7682619
#define CUBE_UNPACKED_CRC 0xBA092543

#define MODLOADER_CRC 0x39D18E98

char* CUBE_EXECUTABLE = "cubeworld.exe";
char* MODLOADER_DLL = "CubeModLoader.dll";

using namespace std;

bool FileExists(char* fileName) {
    DWORD dwAttrib = GetFileAttributes(fileName);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int Bail(int result){
    printf("Press enter to exit.\n");
    cin.ignore();
    return result;
}

int main(int argc, char** argv) {
    bool testMode = false;
    if (argc >= 2 && !strcmp(argv[1], "test")) {
        testMode = true;
        printf("Test mode enabled. CRC checks will be bypassed.\n");
        if (argc >= 3) {
            CUBE_EXECUTABLE = argv[2];
        }
    }

    //Cube world is obviously required
    if (!FileExists(CUBE_EXECUTABLE)) {
        printf("%s not found.\n", CUBE_EXECUTABLE);
        return Bail(1);
    }

    unsigned int checksum = crc32_file(CUBE_EXECUTABLE);

    if (testMode) {
        printf("%s CRC: %08X\n", CUBE_EXECUTABLE, checksum);
    }

    // Check if the game is still packed
    if (checksum == CUBE_PACKED_CRC && !testMode) {
        printf("Cube World was found, but it is not unpacked.\n"
               "Use Steamless to unpack %s.\n", CUBE_EXECUTABLE);
        return Bail(1);
    }


    if (checksum != CUBE_UNPACKED_CRC && !testMode) {
        printf("Cube World was found, but it is not version %s.\n"
               "(Found CRC %08X, expected %08X)\n"
               "Please update your game.\n",
               CUBE_VERSION, checksum, CUBE_UNPACKED_CRC);
        return Bail(1);
    }

    //Inject our dll
    if ( !FileExists(MODLOADER_DLL) ) {
        printf("%dll not found.\n", MODLOADER_DLL);
        return Bail(1);
    }

    unsigned int loaderChecksum = crc32_file(MODLOADER_DLL);
    if (loaderChecksum != MODLOADER_CRC && !testMode) {
        printf("%dll is the wrong version (%08X)\n", MODLOADER_DLL, loaderChecksum);
        return Bail(1);
    }

    if (testMode) {
        printf("%dll CRC: %08X\n", MODLOADER_DLL, loaderChecksum);
    }

    Process process(CUBE_EXECUTABLE);

    //Create game in suspended state
    printf("Starting %s...\n\n", CUBE_EXECUTABLE);
    if (!process.Create()) {
        printf("Failed to create process: %lu", GetLastError());
        return Bail(1);
    } else {
        printf("%s was successfully started.\n\n", CUBE_EXECUTABLE);
    }

    process.InjectDLL( std::string(MODLOADER_DLL) );

    // Need to give the loader some time to work
    // This is a horrible thing and probably will result in a race condition please help me
    Sleep(250);

    // Let Cube World run!
    process.Run();

    Sleep(3000);

    return 0;
}
