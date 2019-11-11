extern "C" float PlayerStaminaRegenHandler(void* player, float staminaRegen) {
	float modifiedStaminaRegen = staminaRegen;
	for (DLL* dll : modDLLs) {
		modifiedStaminaRegen = dll->mod->ModifyPlayerStaminaRegen(player, modifiedStaminaRegen);
	}
	return modifiedStaminaRegen;
}

GETTER_VAR(void*, ASMPlayerStaminaRegenHandler_jmpback);
__attribute__((naked)) void ASMPlayerStaminaRegenHandler() {
	asm(".intel_syntax \n"

		// check if the creature is a player
		"cmp byte ptr [rbx+0x60], 0 \n"
		"jne ASMPlayerStaminaRegenHandlerNotPlayer \n"

		PUSH_ALL

		"mov rcx, rbx \n" // cube::Creature*
		"movss xmm1, xmm6 \n"

		PREPARE_STACK

		"call PlayerStaminaRegenHandler \n"

		RESTORE_STACK

		POP_ALL

		"ASMPlayerStaminaRegenHandlerNotPlayer: \n"

		// original code
		"movaps xmm6, [rsp+0x20] \n"
		"add rsp, 0x30 \n"
		"pop rbx \n"

		"ret \n"
	);
}

void SetupPlayerStaminaRegenHandler() {
	WriteFarJMP(Offset(base, 0x648A1), (void*)&ASMPlayerStaminaRegenHandler);
}
