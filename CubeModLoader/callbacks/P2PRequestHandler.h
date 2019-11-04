extern "C" int P2PRequestHandler(long long steamID) {
    for (DLL* dll: modDLLs) {
        if (int result = dll->mod->OnP2PRequest(steamID)) {
			return result;
        }
    }
    return 0;
}

GETTER_VAR(void*, ASMP2PRequestHandler_jmpback);
GETTER_VAR(void*, ASMP2PRequestHandler_block);
GETTER_VAR(void*, ASMP2PRequestHandler_allow);
void ASMP2PRequestHandler() {
    asm(".intel_syntax \n"
		PUSH_ALL

        "mov rcx, [rdi] \n" //incoming steam id

        PREPARE_STACK

        "call P2PRequestHandler \n"

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

        DEREF_JMP(ASMP2PRequestHandler_jmpback)


        "1: \n"  //block
        POP_ALL
		DEREF_JMP(ASMP2PRequestHandler_block)

        "2: \n" //allow
        POP_ALL
        DEREF_JMP(ASMP2PRequestHandler_allow)
       );
}
void SetupP2PRequestHandler() {
    WriteFarJMP(Offset(base, 0x9F6DF), (void*)&ASMP2PRequestHandler);
    ASMP2PRequestHandler_jmpback = Offset(base, 0x9F6ED);
    ASMP2PRequestHandler_block = Offset(base, 0x9F7A6);
    ASMP2PRequestHandler_allow = Offset(base, 0x9F783);
}
