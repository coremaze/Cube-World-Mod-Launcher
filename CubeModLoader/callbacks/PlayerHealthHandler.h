extern "C" float PlayerHealthHandler(void* player, float health) {
	float modifiedHealth = health;
	for (DLL* dll : modDLLs) {
		modifiedHealth = dll->mod->ModifyPlayerHealth(player, modifiedHealth);
	}
	return modifiedHealth;
}

GETTER_VAR(void*, ASMPlayerHealthHandler_jmpback);
__attribute__((naked)) void ASMPlayerHealthHandler() {
	asm(".intel_syntax \n"

		// check if the creature is a player
		"cmp byte ptr [rbx+0x60], 0 \n"
		"jne ASMPlayerHealthHandlerNotPlayer \n"

		PUSH_ALL

		"mov rcx, rbx \n" // cube::Creature*
		"movss xmm1, xmm6 \n"

		PREPARE_STACK

		"call PlayerHealthHandler \n"

		RESTORE_STACK

		POP_ALL

		"ASMPlayerHealthHandlerNotPlayer: \n"

		// original code
		"movaps xmm6, [rsp+0x20] \n"
		"add rsp, 0x30 \n"
		"pop rbx \n"

		"ret \n" //Also overwrite the return byte because otherwise there is not enough space.
	);
}

void SetupPlayerHealthHandler() {
	WriteFarJMP(Offset(base, 0x5FC11), (void*)&ASMPlayerHealthHandler);
}
