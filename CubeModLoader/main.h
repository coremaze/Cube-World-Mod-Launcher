#ifndef MAIN_H
#define MAIN_H

#include "DLL.h"
#include <vector>
#define no_optimize __attribute__((optimize("O0")))
using namespace std;
vector <DLL*> modDLLs;
void* base;
void WriteFarJMP(void* source, void* destination);

#endif // MAIN_H
