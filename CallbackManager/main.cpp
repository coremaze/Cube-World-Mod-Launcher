#include "main.h"
#include <vector>
#define no_shenanigans __attribute__((noinline)) __declspec(dllexport)
#define MakeCallback(callbackName, callbackReturnType, registrarFunctionName, vectorName)\
    typedef callbackReturnType (__stdcall *callbackName)( ... );\
    std::vector<callbackName> vectorName;\
    extern "C" void DLL_EXPORT registrarFunctionName(callbackName func){\
    vectorName.push_back(func);\
    }

UINT_PTR base;

//Chat Events
MakeCallback(ChatEventCallback, bool, RegisterChatEventCallback, chat_event_callbacks);
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

//Zone Loaded
MakeCallback(ZoneLoadedCallback, void, RegisterZoneLoadedCallback, zone_loaded_callbacks);
void __stdcall no_shenanigans HandleZoneLoaded(unsigned int zone_ptr){
    for (ZoneLoadedCallback func : zone_loaded_callbacks){
        func(zone_ptr);
    }
}
DWORD HandleZoneLoaded_ptr = (DWORD)&HandleZoneLoaded;

unsigned int ASMHandleZoneLoaded_JMP_back;
_declspec(naked) void DLL_EXPORT ASMHandleZoneLoaded(){
    asm("push eax");
    asm("push ebx");
    asm("push ecx");
    asm("push edx");
    asm("push edi");
    asm("push esi");

    asm("push eax");
    asm("call [_HandleZoneLoaded_ptr]");

    asm("pop esi");
    asm("pop edi");
    asm("pop edx");
    asm("pop ecx");
    asm("pop ebx");
    asm("pop eax");

    asm("mov ecx, [ebx]"); //Original code
    asm("add ecx, 0x800D44");

    asm("jmp [_ASMHandleZoneLoaded_JMP_back]");
}


//Zone Destructed
MakeCallback(ZoneDeleteCallback, void, RegisterZoneDeleteCallback, zone_delete_callbacks);
void __stdcall no_shenanigans HandleZoneDelete(unsigned int zone_ptr){
    for (ZoneDeleteCallback func : zone_delete_callbacks){
        func(zone_ptr);
    }
}
DWORD HandleZoneDelete_ptr = (DWORD)&HandleZoneDelete;

unsigned int ASMHandleZoneDelete_JMP_back;
_declspec(naked) void DLL_EXPORT ASMHandleZoneDelete(){
    asm("push eax");
    asm("push ebx");
    asm("push ecx");
    asm("push edx");
    asm("push edi");
    asm("push esi");

    asm("push ecx"); //Zone ptr
    asm("call [_HandleZoneDelete_ptr]");

    asm("pop esi");
    asm("pop edi");
    asm("pop edx");
    asm("pop ecx");
    asm("pop ebx");
    asm("pop eax");

    asm("push ebp"); //Original code
    asm("mov ebp, esp");
    asm("push esi");
    asm("mov esi, ecx");

    asm("jmp [_ASMHandleZoneDelete_JMP_back]");
}

//Check M3. returning -1 means force disable. 0 means act as normal. 1 means force enable
MakeCallback(DodgeAttemptCheckCallback, int, RegisterDodgeAttemptCheckCallback, dodge_attempt_check_callbacks);
int __stdcall no_shenanigans HandleDodgeAttemptCheck(unsigned int attempting_to_dodge){
    int need_to_dodge = 0;
    for (DodgeAttemptCheckCallback func : dodge_attempt_check_callbacks){
        unsigned int need_to_dodge_2 = func(attempting_to_dodge);
        if (need_to_dodge_2 != 0){
            need_to_dodge = need_to_dodge_2;
        }
    }
    return need_to_dodge;
}
DWORD HandleDodgeAttemptCheck_ptr = (DWORD)&HandleDodgeAttemptCheck;

unsigned int ASMM3Check_JMP_back;
_declspec(naked) void DLL_EXPORT ASMM3Check(){
    asm("movss dword ptr [esp+0x18], xmm6"); //original code
    asm("movss dword ptr [esp+0x80], xmm6");

    asm("push ebx");
    asm("push eax");

    asm("push ebx");
    asm("call [_HandleDodgeAttemptCheck_ptr]");


    asm("mov ebx, eax"); //result

    asm("cmp ebx, -1");
    asm("je 0f"); //Disable

    asm("cmp ebx, 0");
    asm("je 1f"); //Do nothing

    //Enable
    asm("pop ebx");
    asm("pop eax");
    asm("mov ebx, 0");
    asm("jmp [_ASMM3Check_JMP_back]");

    asm("0:"); //Disable
    asm("pop ebx");
    asm("pop eax");
    asm("mov ebx, 1");
    asm("jmp [_ASMM3Check_JMP_back]");

    asm("1:"); //Do nothing
    asm("pop eax");
    asm("pop ebx");
    asm("jmp [_ASMM3Check_JMP_back]");
}

//Check M1. returning -1 means force disable. 0 means act as normal. 1 means force enable
MakeCallback(PrimaryAttackAttemptCheckCallback, int, RegisterPrimaryAttackAttemptCheckCallback, primary_attack_attempt_callbacks);
int __stdcall no_shenanigans HandlePrimaryAttackAttemptCheck(unsigned int attempting_to_attack){
    int need_to_attack = 0;
    for (PrimaryAttackAttemptCheckCallback func : primary_attack_attempt_callbacks){
        unsigned int need_to_attack_2 = func(attempting_to_attack);
        if (need_to_attack_2 != 0){
            need_to_attack = need_to_attack_2;
        }
    }
    return need_to_attack;
}
DWORD HandlePrimaryAttackAttemptCheck_ptr = (DWORD)&HandlePrimaryAttackAttemptCheck;

unsigned int ASMM1Check_JMP_Enable;
unsigned int ASMM1Check_JMP_Disable;
_declspec(naked) void DLL_EXPORT ASMM1Check(){
    asm("push eax");

    asm("movzx eax, byte ptr [edi+0x4]");
    asm("push eax");
    asm("call [_HandlePrimaryAttackAttemptCheck_ptr]");

    asm("cmp eax, -1");
    asm("je 0f"); //Disable

    asm("cmp eax, 0");
    asm("je 1f"); //Do nothing

    //Enable
    asm("pop eax");
    asm("jmp [_ASMM1Check_JMP_Enable]");

    asm("0:"); //Disable
    asm("pop eax");
    asm("jmp [_ASMM1Check_JMP_Disable]");

    asm("1:"); //Do nothing
    asm("pop eax");
    asm("cmp byte ptr [edi+0x4], 0"); //original code
    asm("jz 2f");
    asm("jmp [_ASMM1Check_JMP_Enable]");

    asm("2:"); //the disable option from original code
    asm("jmp [_ASMM1Check_JMP_Disable]"); //Can't do this with jz
}

//Check abilities. returning -1 means force disable. 0 means act as normal. 1 means force enable
//I don't know why you would want to enable this, but for consistency it will be like this.
MakeCallback(AbilityAttackAttemptCheckCallback, int, RegisterAbilityAttackAttemptCheckCallback, ability_attack_attempt_callbacks);
int __stdcall no_shenanigans HandleAbilityAttackAttemptCheck(unsigned int attempting_to_attack, unsigned int keyNumber){
    int need_to_attack = 0;
    for (AbilityAttackAttemptCheckCallback func : ability_attack_attempt_callbacks){
        unsigned int need_to_attack_2 = func(attempting_to_attack, keyNumber);
        if (need_to_attack_2 != 0){
            need_to_attack = need_to_attack_2;
        }
    }
    return need_to_attack;
}
DWORD HandleAbilityAttackAttemptCheck_ptr = (DWORD)&HandleAbilityAttackAttemptCheck;

unsigned int ASMAbilitiesCheck_JMP_Back;
_declspec(naked) void DLL_EXPORT ASMAbilitiesCheck(){
    asm("push eax");


    asm("push esi"); //keyNumber
    asm("movzx eax, byte ptr [edi+esi+0x4]"); //attempting to attack
    asm("push eax");
    asm("call [_HandleAbilityAttackAttemptCheck_ptr]");

    asm("cmp eax, -1");
    asm("je 0f"); //Disable

    asm("cmp eax, 0"); //Do nothing
    asm("je 1f");

    //Enable
    asm("mov eax, 1");
    asm("cmp eax, 0");
    asm("pop eax");
    asm("jmp [_ASMAbilitiesCheck_JMP_Back]");

    asm("0:"); //Disable
    asm("mov eax, 0");
    asm("cmp eax, 0");
    asm("pop eax");
    asm("jmp [_ASMAbilitiesCheck_JMP_Back]");

    asm("1:"); //Do nothing
    asm("cmp byte ptr [edi+esi+0x4], 0"); //original code
    asm("pop eax");
    asm("jmp [_ASMAbilitiesCheck_JMP_Back]");
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
            //Chat
            WriteJMP((BYTE*)(base + 0x7E61A), (BYTE*)&ASMHandleMessage);

            //Zone Load
            ASMHandleZoneLoaded_JMP_back = base + 0x6ACDE;
            WriteJMP((BYTE*)(base + 0x6ACD6), (BYTE*)&ASMHandleZoneLoaded);

            //Zone Delete
            ASMHandleZoneDelete_JMP_back = base + 0x224766;
            WriteJMP((BYTE*)(base + 0x224760), (BYTE*)&ASMHandleZoneDelete);

            //Check M3
            ASMM3Check_JMP_back = base + 0xA6CD2;
            WriteJMP((BYTE*)(base + 0xA6CC3), (BYTE*)&ASMM3Check);

            //Check M1
            ASMM1Check_JMP_Enable = base + 0x9BF82;
            ASMM1Check_JMP_Disable = base + 0x9BFC8;
            WriteJMP((BYTE*)(base + 0x9BF7C), (BYTE*)&ASMM1Check);

            //Check abilities
            ASMAbilitiesCheck_JMP_Back = base + 0x9B63A;
            WriteJMP((BYTE*)(base + 0x9B635), (BYTE*)&ASMAbilitiesCheck);

            break;
    }
    return true;
}
