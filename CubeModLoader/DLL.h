#ifndef DLL_H
#define DLL_H
#include <iostream>
#include <windows.h>

class DLL
{
    public:
        std::string fileName;
        HMODULE handle;

        FARPROC ModInitialize;
        FARPROC ModMajorVersion;
        FARPROC ModMinorVersion;

        // Callbacks
        FARPROC HandleNumber;

        DLL(std::string fileName);
        HMODULE Load();
        virtual ~DLL();

    protected:

    private:
};

#endif // DLL_H
