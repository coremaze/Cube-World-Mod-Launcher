#ifndef GCC_MUTEX_H
#define GCC_MUTEX_H
#include <windows.h>
// GCC doesn't have a std::mutex, and as of the time of writing, I'm still using GCC.
class mutex {
private:
    CRITICAL_SECTION cs;
public:
    mutex();
    void lock();
    void unlock();
};

#endif // GCC_MUTEX_H
