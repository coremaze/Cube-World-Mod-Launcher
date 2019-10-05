int P2PRequestHandler(long long steamID) {
    for (DLL* dll: modDLLs) {
        if (dll->HandleP2PRequest) {
            if ( int result = ((int(*)(long long))dll->HandleP2PRequest)(steamID)  ){
                return result;
            }
        }
    }
    return 0;
}
void* P2PRequestHandler_ptr = (void*)&P2PRequestHandler;

void* ASMP2PRequestHandler_jmpback;
void* ASMP2PRequestHandler_block;
void* ASMP2PRequestHandler_allow;
void no_optimize ASMP2PRequestHandler() {
    asm(PUSH_ALL

        "mov rcx, [rdi] \n" //incoming steam id

        PREPARE_STACK

        "call [P2PRequestHandler_ptr] \n"

        RESTORE_STACK

        // Did the handler return 1? block
        "cmp eax, 1 \n"
        "je 1f \n"

        // Did the handler return 2? allow
        "cmp eax, 2 \n"
        "je 2f \n"

        // Otherwise? Do nothing

        POP_ALL

        // original code
        "mov edx, 4 \n"
        "mov rcx, [rax] \n"
        "mov rax, [rcx] \n"
        "call qword ptr [rax+0x18] \n"

        "jmp [ASMP2PRequestHandler_jmpback] \n"


        "1: \n"  //block
        POP_ALL
        "jmp [ASMP2PRequestHandler_block] \n"

        "2: \n" //allow
        POP_ALL
        "jmp [ASMP2PRequestHandler_allow] \n"
       );
}
void SetupP2PRequestHandler() {
    WriteFarJMP(base+0x9F6DF, (void*)&ASMP2PRequestHandler);
    ASMP2PRequestHandler_jmpback = (void*)base+0x9F6ED;
    ASMP2PRequestHandler_block = (void*)base+0x9F7A6;
    ASMP2PRequestHandler_allow = (void*)base+0x9F783;
}
