extern "C" int PresentHandler(void* SwapChain, UINT SyncInterval, UINT Flags) { // Note that this hooks a METHOD, so swapchain is the first argument
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnPresentPriority == (GenericMod::Priority)priority) {
				dll->mod->OnPresent(SwapChain, SyncInterval, Flags);
			}
		}
	}
	return 0;
}

GETTER_VAR(void*, ASM_PresentHandler_jmpback);
void ASM_PresentHandler() {
	asm(".intel_syntax \n"

		// Set up for the call (original code)
		"setnz bl \n"
		"xor r8d, r8d \n"
		"mov rax, [rcx] \n"
		"mov edx, ebx \n"

		// Call handlers before calling Present
		PUSH_ALL
		PREPARE_STACK
		"call PresentHandler \n"
		RESTORE_STACK
		POP_ALL

		// Call Present for real now (original code)
		"call [rax+0x40] \n"

		DEREF_JMP(ASM_PresentHandler_jmpback)
	);
}
void SetupPresentHandler() {
	WriteFarJMP(Offset(base, 0x134743), (void*)&ASM_PresentHandler);
	ASM_PresentHandler_jmpback = Offset(base, 0x134751);
}
