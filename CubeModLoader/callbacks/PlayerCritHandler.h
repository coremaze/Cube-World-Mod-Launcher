extern "C" float PlayerCritHandler(void* player, float crit) {
	float modifiedCrit = crit;
	for (DLL* dll : modDLLs) {
		modifiedCrit = dll->mod->ModifyPlayerCrit(player, modifiedCrit);
	}
	return modifiedCrit;
}

GETTER_VAR(void*, ASMPlayerCritHandler_jmpback);
__attribute__((naked)) void ASMPlayerCritHandler() {
	asm(".intel_syntax \n"

		PUSH_ALL

		"movss xmm1, xmm0 \n"

		PREPARE_STACK

		"call PlayerCritHandler \n"

		RESTORE_STACK

		POP_ALL

		// original code
		"movaps xmm6, [rsp+0x20] \n"
		"mov rsi, [rsp+0x48] \n"
		"add rsp, 0x30\n"

		DEREF_JMP(ASMPlayerCritHandler_jmpback)
	);
}

void SetupPlayerCritHandler() {
	WriteFarJMP(Offset(base, 0x50E82), (void*)&ASMPlayerCritHandler);
	ASMPlayerCritHandler_jmpback = Offset(base, 0x50E90);
}
