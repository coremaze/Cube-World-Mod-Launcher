#include "main.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include "Process.h"

char* CUBE_EXECUTABLE = "cubeworld.exe";
char* MODLOADER_DLL = "CubeModLoader.dll";

using namespace std;

bool FileExists(const char* fileName) {
    DWORD dwAttrib = GetFileAttributes(fileName);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int Bail(int result){
    printf("Press enter to exit.\n");
    cin.ignore();
    return result;
}

int main(int argc, char** argv) {
    if (argc >= 2) {
        CUBE_EXECUTABLE = argv[1];
    }

    //Cube world is obviously required
    if (!FileExists(CUBE_EXECUTABLE)) {
        printf("%s not found.\n", CUBE_EXECUTABLE);
        return Bail(1);
    }

    //Inject our dll
    if ( !FileExists(MODLOADER_DLL) ) {
        printf("%s not found.\n", MODLOADER_DLL);
        return Bail(1);
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
