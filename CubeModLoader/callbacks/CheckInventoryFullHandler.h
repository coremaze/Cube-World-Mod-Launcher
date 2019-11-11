extern "C" int CheckInventoryFullHandler(void* player, void* item) {
    for (DLL* dll: modDLLs) {
        if ( int result = dll->mod->OnCheckInventoryFull(player, item) ){
            return result;
        }
    }
    return 0;
}

GETTER_VAR(void*, ASMCheckInventoryFullHandler_jmpback);
GETTER_VAR(void*, ASMCheckInventoryFullHandler_retn);
__attribute__((naked)) void ASMCheckInventoryFullHandler() {
    asm(".intel_syntax \n"
		PUSH_ALL

        PREPARE_STACK
        "call CheckInventoryFullHandler \n"

        RESTORE_STACK

        // Did the handler return 1? true
        "cmp eax, 1 \n"
        "je 1f \n"

        // Did the handler return 2? false
        "cmp eax, 2 \n"
        "je 2f \n"

        // Otherwise? Do nothing
        POP_ALL

        // original code
        "mov     [rsp+0x20], rbp \n"
        "push    r12 \n"
        "push    r14 \n"
        "push    r15 \n"
        "sub     rsp, 0x20 \n"
		DEREF_JMP(ASMCheckInventoryFullHandler_jmpback)


        "1: \n"  //full
        POP_ALL
        "xor al,al \n"
		DEREF_JMP(ASMCheckInventoryFullHandler_retn)

        "2: \n" //not
        POP_ALL
        "mov al,1 \n"
		DEREF_JMP(ASMCheckInventoryFullHandler_retn)
       );
}
void SetupCheckInventoryFullHandler() {
    WriteFarJMP(Offset(base, 0x50670), (void*)&ASMCheckInventoryFullHandler);
    ASMCheckInventoryFullHandler_jmpback = Offset(base, 0x5067F);
    ASMCheckInventoryFullHandler_retn = Offset(base, 0x507A0);
}
