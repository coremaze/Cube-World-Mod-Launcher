extern "C" int ChatHandler(void* msg) {
    for (DLL* dll: modDLLs) {
		if (dll->mod->OnChat(msg)) {
			return 1;
		}
    }
    return 0;
}

GETTER_VAR(void*, ASMChatHandler_jmpback);
GETTER_VAR(void*, ASMChatHandler_bail);
void ASMChatHandler() {
	asm(".intel_syntax \n"
		PUSH_ALL

		"mov rcx, rbx \n" //The (std::wstring*) message

		PREPARE_STACK

		"call ChatHandler \n"

		RESTORE_STACK

		// Did the handler return true?
		"test al, al \n"
		"jnz bail \n"

		POP_ALL

		// original code
		"mov [rsp+0x48], rax \n"
		"mov r8, [rbx+0x10] \n"
		"cmp qword ptr [rbx+0x18], 8 \n"
		"jb 1f \n"
		"mov rbx, [rbx] \n"
		"1: \n"
		DEREF_JMP(ASMChatHandler_jmpback)


		"bail: \n"
		POP_ALL
		DEREF_JMP(ASMChatHandler_bail)
	);
}
void SetupChatHandler() {
	WriteFarJMP(Offset(base, 0x97175), (void*)&ASMChatHandler);
	ASMChatHandler_jmpback = Offset(base, 0x97188);
	ASMChatHandler_bail = Offset(base, 0x9777A);
}
