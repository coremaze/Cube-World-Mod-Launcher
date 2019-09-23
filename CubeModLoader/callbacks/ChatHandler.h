int ChatHandler(wchar_t* msg) {
    for (DLL* dll: modDLLs) {
        if (dll->HandleChat) {
            if (  ((int(*)(wchar_t*))dll->HandleChat)(msg)  ){
                return 1;
            }
        }
    }
    return 0;
}
void* ChatHandler_ptr = (void*)&ChatHandler;

void* ASMChatHandler_jmpback;
void* ASMChatHandler_bail;
void no_optimize ASMChatHandler() {
    asm(PUSH_ALL

        "mov rcx, rdi \n" // The message

        PREPARE_STACK

        "call [ChatHandler_ptr] \n"

        RESTORE_STACK

        // Did the handler return true?
        "test al, al \n"
        "jnz bail \n"

        POP_ALL

        // original code
        "mov qword ptr [rbp+0x78], 7 \n"
        "mov [rbp+0x70], r12 \n"
        "jmp [ASMChatHandler_jmpback] \n"


        "bail: \n"
        POP_ALL
        "jmp [ASMChatHandler_bail]"
       );
}
void SetupChatHandler() {
    WriteFarJMP(base+0x95F88, (void*)&ASMChatHandler);
    ASMChatHandler_jmpback = (void*)base+0x95F95;
    ASMChatHandler_bail = (void*)base+0x966D3;
}
