extern "C" int WindowProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnWindowProcPriority == (GenericMod::Priority)priority) {
				if (int result = dll->mod->OnWindowProc(hwnd, uMsg, wParam, lParam)) {
					return result;
				}
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_WindowProcHandler_jmpback);
void ASM_WindowProcHandler() {
    asm(".intel_syntax \n"
		PUSH_ALL
        PREPARE_STACK

        "call WindowProcHandler \n"

        RESTORE_STACK

        // Did the handler return 1? return. Since we're hooking the beginning of a function, this is safe.
        "cmp eax, 1 \n"
        "je 1f \n"

        POP_ALL

        // original code
		"mov [rsp+0x8], rbx \n"
		"push rdi \n"
		"sub rsp, 0x20 \n"
		"mov rbx, r8 \n"
		"mov r10, rcx \n"

        DEREF_JMP(ASM_WindowProcHandler_jmpback)


        "1: \n"  //block
        POP_ALL
		"ret \n"
       );
}
void SetupWindowProcHandler() {
    WriteFarJMP(Offset(base, 0x133C70), (void*)&ASM_WindowProcHandler);
	ASM_WindowProcHandler_jmpback = Offset(base, 0x133C80);
}
