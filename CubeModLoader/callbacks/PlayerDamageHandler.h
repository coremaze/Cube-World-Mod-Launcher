extern "C" float PlayerDamageHandler(void* player, float damage) {
	float modifiedDamage = damage;
	for (DLL* dll : modDLLs) {
		modifiedDamage = dll->mod->ModifyPlayerDamage(player, modifiedDamage);
	}
	return modifiedDamage;
}

GETTER_VAR(void*, ASMPlayerDamageHandler_jmpback);
__attribute__((naked)) void ASMPlayerDamageHandler() {
	asm(".intel_syntax \n"

		// check if the creature is a player
		"cmp byte ptr [rsi+0x60], 0 \n"
		"jne ASMPlayerDamageHandlerNotPlayer \n"

		"push rdx \n"
		"push rdi \n"

		"movq rdx, xmm1 \n"
		"movq rdi, xmm2 \n"

		PUSH_ALL

		"mov rcx, rsi \n" // cube::Creature*
		"movss xmm1, xmm0 \n"

		PREPARE_STACK

		"call PlayerDamageHandler \n"

		RESTORE_STACK

		POP_ALL

		"movq xmm1, rdx \n"
		"movq xmm2, rdi \n"

		"pop rdi \n"
		"pop rdx \n"

		"ASMPlayerDamageHandlerNotPlayer: \n"

		// original code
		"movaps xmm6, [rsp+0x70] \n"
		"movaps xmm7, [rsp+0x60] \n"
		"mov rsp, r11 \n"
		"pop r14 \n"

		DEREF_JMP(ASMPlayerDamageHandler_jmpback)
	);
}

void SetupPlayerDamageHandler() {
	WriteFarJMP(Offset(base, 0x4FD2B), (void*)&ASMPlayerDamageHandler);
	ASMPlayerDamageHandler_jmpback = Offset(base, 0x4FD3A);
}
