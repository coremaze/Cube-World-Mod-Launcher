#include "main.h"
#include <vector>
#define no_shenanigans __attribute__((noinline)) __declspec(dllexport)
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

//Chunk Loaded
typedef void (__stdcall *ChunkLoadedCallback)(unsigned int zone_ptr);
std::vector<ChunkLoadedCallback> chunk_loaded_callbacks;
extern "C" void DLL_EXPORT RegisterChunkLoadedCallback(ChunkLoadedCallback func){
    chunk_loaded_callbacks.push_back(func);
}
void __stdcall no_shenanigans HandleChunkLoaded(unsigned int zone_ptr){
    for (ChunkLoadedCallback func : chunk_loaded_callbacks){
        func(zone_ptr);
    }
}
DWORD HandleChunkLoaded_ptr = (DWORD)&HandleChunkLoaded;

unsigned int ASMHandleChunkLoaded_JMP_back;
_declspec(naked) void DLL_EXPORT ASMHandleChunkLoaded(){
    asm("push eax");
    asm("push ebx");
    asm("push ecx");
    asm("push edx");
    asm("push edi");
    asm("push esi");

    asm("push eax");
    asm("call [_HandleChunkLoaded_ptr]");

    asm("pop esi");
    asm("pop edi");
    asm("pop edx");
    asm("pop ecx");
    asm("pop ebx");
    asm("pop eax");

    asm("mov ecx, [ebx]"); //Original code
    asm("add ecx, 0x800D44");

    asm("jmp [_ASMHandleChunkLoaded_JMP_back]");
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

            ASMHandleChunkLoaded_JMP_back = base + 0x6ACDE;
            WriteJMP((BYTE*)(base + 0x6ACD6), (BYTE*)&ASMHandleChunkLoaded);

            break;
    }
    return true;
}
