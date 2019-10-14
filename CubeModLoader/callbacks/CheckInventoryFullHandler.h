int CheckInventoryFullHandler(void* player, void* item) {
    for (DLL* dll: modDLLs) {
        if (dll->HandleCheckInventoryFull) {
            if ( int result = ((int(*)(void*, void*))dll->HandleCheckInventoryFull)(player, item)  ){
                return result;
            }
        }
    }
    return 0;
}
void* CheckInventoryFullHandler_ptr = (void*)&CheckInventoryFullHandler;

void* ASMCheckInventoryFullHandler_jmpback;
void* ASMCheckInventoryFullHandler_retn;
void no_optimize ASMCheckInventoryFullHandler() {
    asm(PUSH_ALL

        PREPARE_STACK
        "call [CheckInventoryFullHandler_ptr] \n"

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
        "jmp [ASMCheckInventoryFullHandler_jmpback] \n"


        "1: \n"  //full
        POP_ALL
        "xor al,al \n"
        "jmp [ASMCheckInventoryFullHandler_retn] \n"

        "2: \n" //not
        POP_ALL
        "mov al,1 \n"
        "jmp [ASMCheckInventoryFullHandler_retn] \n"
       );
}
void SetupCheckInventoryFullHandler() {
    WriteFarJMP(base+0x50670, (void*)&ASMCheckInventoryFullHandler);
    ASMCheckInventoryFullHandler_jmpback = (void*)base+0x5067F;
    ASMCheckInventoryFullHandler_retn = (void*)base+0x507A0;
}
