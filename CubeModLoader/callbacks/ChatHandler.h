extern "C" int ChatHandler(wchar_t* msg) {
    for (DLL* dll: modDLLs) {
        if (dll->HandleChat) {
            if (  ((int(*)(wchar_t*))dll->HandleChat)(msg)  ){
                return 1;
            }
        }
    }
    return 0;
}

GETTER_VAR(void*, ASMChatHandler_jmpback);
GETTER_VAR(void*, ASMChatHandler_bail);
void ASMChatHandler() {
    asm(".intel_syntax \n"
		PUSH_ALL

        "mov rcx, rbx \n" // The message

        PREPARE_STACK

        "call ChatHandler \n"

        RESTORE_STACK

        // Did the handler return true?
        "test al, al \n"
        "jnz bail \n"

        POP_ALL

        // original code
        "mov qword ptr [rbp+0x88], 7 \n"
        "mov [rbp+0x80], r12 \n"
        "mov [rbp+0x70], r12w \n"
		DEREF_JMP(ASMChatHandler_jmpback)


        "bail: \n"
        POP_ALL
		DEREF_JMP(ASMChatHandler_bail)
       );
}
void SetupChatHandler() {
    WriteFarJMP(Offset(base, 0x97198), (void*)&ASMChatHandler);
    ASMChatHandler_jmpback = Offset(base, 0x971B0);
    ASMChatHandler_bail = Offset(base, 0x9777A);
}
