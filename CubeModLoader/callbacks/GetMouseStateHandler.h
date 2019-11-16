extern "C" int GetMouseStateHandler(void* diMouse) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnGetMouseStatePriority == (GenericMod::Priority)priority) {
				dll->mod->OnGetMouseState(diMouse);
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_GetMouseStateHandler_jmpback);
void ASM_GetMouseStateHandler() {
    asm(".intel_syntax \n"
		// original code
		"mov rcx, qword ptr [rbp-0x78] \n"
		"mov rax, [rcx] \n"
		"lea r8, [rbp+0x1B0] \n"
		"mov edx, 0x10 \n"
		"call qword ptr [rax+0x48] \n" // call to the actual function, or at least steam's hook of it.


		PUSH_ALL
		"lea rcx, [rbp+0x1B0] \n" // diMouse
        PREPARE_STACK

        "call GetMouseStateHandler \n"

        RESTORE_STACK
        POP_ALL

        DEREF_JMP(ASM_GetMouseStateHandler_jmpback)
       );
}
void SetupGetMouseStateHandler() {
    WriteFarJMP(Offset(base, 0x13665F), (void*)&ASM_GetMouseStateHandler);
	ASM_GetMouseStateHandler_jmpback = Offset(base, 0x136675);
}
