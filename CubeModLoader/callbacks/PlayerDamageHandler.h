extern "C" float PlayerDamageHandler(void* player, float damage) {
	float modifiedDamage = damage;
    for (DLL* dll: modDLLs) {
		modifiedDamage = dll->mod->ModifyPlayerDamage(player, modifiedDamage);
    }
	return modifiedDamage;
}

GETTER_VAR(void*, ASMPlayerDamageHandler_jmpback);
void ASMPlayerDamageHandler() {
	asm(".intel_syntax \n"

		// check if the creature is a player
		"cmp byte ptr [rsi+0x60], 0x0 \n"
		"jne ASMPlayerDamageHandlerNotPlayer \n"

		PUSH_ALL

		"mov rcx, rbx \n" // cube::Creature*

		PREPARE_STACK

		"call PlayerDamageHandler \n"

		RESTORE_STACK

		POP_ALL

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
