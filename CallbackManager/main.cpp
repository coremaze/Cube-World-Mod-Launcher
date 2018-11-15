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
    asm("pushad");

    asm("push eax");
    asm("call [_HandleZoneLoaded_ptr]");

    asm("popad");

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
    asm("pushad");

    asm("push ecx"); //Zone ptr
    asm("call [_HandleZoneDelete_ptr]");

    asm("popad");

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





//Packets
MakeCallback(PacketCallback, int, RegisterPacketCallback, packet_callbacks);
int __stdcall no_shenanigans HandlePacket(unsigned int packet_ID, unsigned int socket_ptr){
    int handled = 0;
    for (PacketCallback func : packet_callbacks){
        unsigned int handled_2 = func(packet_ID, socket_ptr);
        if (handled_2 != 0){
            handled = handled_2;
        }
    }
    return handled;
}
DWORD HandlePacket_ptr = (DWORD)&HandlePacket;


unsigned int ASMHandlePacket_Invalid_Packet_JMP;
unsigned int ASMHandlePacket_Valid_Packet_JMP;
unsigned int ASMHandlePacket_Already_Handled_JMP;
_declspec(naked) void __declspec(dllexport) ASMHandlePacket(){
    asm("pushad");

    asm("push [ebp-0x11D8]"); //socket
    asm("push [ebp-0x1220]"); //Packet ID
    asm("call [_HandlePacket_ptr]");

    asm("cmp eax, 0");
    asm("je 0f"); //The packet was not handled

    asm("popad"); //The packet WAS handled
    asm("mov cl, [ebp-0x11D1]");
    asm("jmp [_ASMHandlePacket_Already_Handled_JMP]");

    asm("0:"); //The packet was not handled
    asm("popad");
    asm("mov eax, [ebp-0x1220]");//original code
    asm("cmp eax, 0xF");
    asm("ja 1f");
    asm("jmp [_ASMHandlePacket_Valid_Packet_JMP]");

    asm("1:");
    asm("jmp [_ASMHandlePacket_Invalid_Packet_JMP]");

}

//ready to send a packet
MakeCallback(ReadyToSendCallback, void, RegisterReadyToSendCallback, ready_to_send_callbacks);
void __stdcall no_shenanigans HandleReadyToSend(SOCKET s){
    for (ReadyToSendCallback func : ready_to_send_callbacks){
       func(s);
    }
}
DWORD HandleReadyToSend_ptr = (DWORD)&HandleReadyToSend;

unsigned int ASMHandleReadyToSend_JMP_Back;
void no_shenanigans ASMHandleReadyToSend(){
    asm("pushad");

    asm("mov eax, [edi]"); //GameController
    asm("push dword ptr [eax+0x8006CC]"); //socket
    asm("call [_HandleReadyToSend_ptr]");

    asm("popad");

    asm("push 0"); //original code
    asm("push 4"); //len
    asm("lea eax, [ebp-0x2444]");

    asm("jmp [_ASMHandleReadyToSend_JMP_Back]");


}

//Finish crafting
MakeCallback(FinishCraftingCallback, void, RegisterFinishCraftingCallback, finish_crafting_callbacks);
void __stdcall no_shenanigans HandleFinishCrafting(){
    for (FinishCraftingCallback func : finish_crafting_callbacks){
       func();
    }
}
DWORD HandleFinishCrafting_ptr = (DWORD)&HandleFinishCrafting;

unsigned int ASMHandleFinishCrafting_JMP_Back;
void no_shenanigans ASMHandleFinishCrafting(){
    asm("pushad");

    asm("call [_HandleFinishCrafting_ptr]");

    asm("popad");

    asm("mov ecx, [ebp-0xC]"); //original code
    asm("mov dword ptr fs:[0], ecx");

    asm("jmp [_ASMHandleFinishCrafting_JMP_Back]");


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

            //Handle packet
            ASMHandlePacket_Already_Handled_JMP = base + 0x6D0E3;
            ASMHandlePacket_Invalid_Packet_JMP = base + 0x6D0DD;
            ASMHandlePacket_Valid_Packet_JMP = base + 0x6B8B0;
            WriteJMP((BYTE*)(base + 0x6B8A7), (BYTE*)&ASMHandlePacket);

            //Ready to send packet
            ASMHandleReadyToSend_JMP_Back = base + 0x69C92;
            WriteJMP((BYTE*)(base + 0x69C88), (BYTE*)&ASMHandleReadyToSend);

            ASMHandleFinishCrafting_JMP_Back = base + 0x70D6B;
            WriteJMP((BYTE*)(base + 0x70D61), (BYTE*)&ASMHandleFinishCrafting);

            break;
    }
    return true;
}
