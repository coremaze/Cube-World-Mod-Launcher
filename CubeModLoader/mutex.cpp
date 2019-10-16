#include "mutex.h"

mutex::mutex(){
    InitializeCriticalSection(&cs);
}

void mutex::lock(){
    EnterCriticalSection(&cs);
}

void mutex::unlock(){
    LeaveCriticalSection(&cs);
}
