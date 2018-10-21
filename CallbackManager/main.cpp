#include "main.h"
#include <vector>
UINT_PTR base;

//Chat Events
typedef bool (__stdcall *ChatEventCallback)(wchar_t buf[], unsigned int msg_size);
std::vector<ChatEventCallback> chat_event_callbacks;
extern "C" void DLL_EXPORT RegisterChatEventCallback(ChatEventCallback func){
    chat_event_callbacks.push_back(func);
}
bool DLL_EXPORT HandleChatEvent(wchar_t buf[], unsigned int msg_size){
    bool cancelChatDisplay = false;
    wchar_t msg[1024] = { 0 };
    memcpy(msg, buf, msg_size * 2); //the message should be null terminated
    for (ChatEventCallback func : chat_event_callbacks){
        bool cancel = func(msg, msg_size);
        if (cancel){
            cancelChatDisplay = true;
        }
    }
    return cancelChatDisplay;
}

DWORD HandleChatEvent_ptr = (DWORD)&HandleChatEvent;
_declspec(naked) void DLL_EXPORT ASMHandleMessage(){

    asm("mov eax, [_base]");
    asm("add eax, 0x36B1C8");
    asm("mov eax, [eax]"); //eax points to gamecontroller
    asm("mov eax, dword ptr [eax + 0x800A14]"); //eax points to ChatWidget
    asm("mov eax, dword ptr [eax + 0x178]"); //get message size
    asm("push eax");

    asm("lea eax, [ebp - 0x128 + 0x4]");
    asm("mov eax, [eax]"); //get message
    asm("push eax");

    asm("call [_HandleChatEvent_ptr]");

    asm("cmp eax, 0"); //message ptr
    asm("je 0f");

    asm("1:");
    asm("mov ecx, [_base]"); //jump to end
    asm("add ecx, 0x7E6BF");
    asm("jmp ecx");


    asm("0:"); //exit normally
    asm("mov eax, [_base]"); //jump back
    asm("add eax, 0x7E621");
    asm("cmp dword ptr [edi + 0x8006CC], 0"); //original comparison
    asm("jmp eax");
}


void WriteJMP(BYTE* location, BYTE* newFunction){
	DWORD dwOldProtection;
	VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
	VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    base = (UINT_PTR)GetModuleHandle(NULL);
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            WriteJMP((BYTE*)(base + 0x7E61A), (BYTE*)&ASMHandleMessage);
            break;
    }
    return true;
}
