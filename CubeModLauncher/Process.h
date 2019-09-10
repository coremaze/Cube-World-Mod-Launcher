#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <windows.h>
class Process
{
    public:
        Process(std::string path);
        virtual ~Process();
        bool InjectDLL(std::string dllName);
        bool Create();
        void Run();

    protected:

    private:
        std::string path;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

};

#endif // PROCESS_H
