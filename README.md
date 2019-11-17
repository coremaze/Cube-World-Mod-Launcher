# Cube-World-Mod-Launcher

Supports injecting Cube World mods. DLLs must go in a folder called "Mods".

## Installing
Get the latest .fip from Releases and place it in the same folder as cubeworld.exe

https://github.com/ChrisMiuchiz/Cube-World-Mod-Launcher/releases

## Installing Mods
A "Mods" folder should be created in the same folder as cubeworld.exe, and mods should be installed by moving them into the Mods folder. Upon starting the game, you will be asked whether you want to launch with mods. If you click "Yes" a dialog box will display all the mods which have been loaded. If you click "No" then the vanilla game will launch.

## Building the launcher or mods

All mods MUST include cwmods.h from [cwsdk](https://github.com/ChrisMiuchiz/CWSDK) to function. The building environment required for the launcher and loader are the same as for cwsdk.

