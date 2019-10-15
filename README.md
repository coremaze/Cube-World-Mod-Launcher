# Cube-World-Mod-Launcher

Supports injecting Cube World mods. DLLs must go in a folder called "Mods".

## Installing
Get the latest .fib from Releases and place it in the same folder as cubeworld.exe

https://github.com/ChrisMiuchiz/Cube-World-Mod-Launcher/releases

## Installing Mods
A "Mods" folder should be created in the same folder as cubeworld.exe, and mods should be installed by moving them into the Mods folder.

## Building the launcher or mods
This project currently only supports GCC, but will likely move to clang soon. This program will not even build using MSVC, not only because the inline assembly syntax is different, but because support for inline assembly was removed for x86-64.

All mods MUST include cwmods.h from [cwsdk](https://github.com/ChrisMiuchiz/CWSDK) to function.

Compiler/Linker flags: `-m64 -masm=intel -static -static-libgcc -static-libstdc++`
