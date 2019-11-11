extern "C" float PlayerArmorHandler(void* player, float armor) {
	float modifiedArmor = armor;
	for (DLL* dll : modDLLs) {
		modifiedArmor = dll->mod->ModifyPlayerArmor(player, modifiedArmor);
	}
	return modifiedArmor;
}

GETTER_VAR(void*, ASMPlayerArmorHandler_jmpback);
__attribute__((naked)) void ASMPlayerArmorHandler() {
	asm(".intel_syntax \n"

		// check if the creature is a player
		"cmp byte ptr [rbx+0x60], 0 \n"
		"jne ASMPlayerArmorHandlerNotPlayer \n"

		PUSH_ALL

		"mov rcx, rbx \n" // cube::Creature*
		"movss xmm1, xmm0 \n"

		PREPARE_STACK

		"call PlayerArmorHandler \n"

		RESTORE_STACK

		POP_ALL

		"ASMPlayerArmorHandlerNotPlayer: \n"

		// original code
		"mov rbx, [rsp+0x40] \n"
		"movaps xmm6, [rsp+0x20] \n"
		"add rsp, 0x30 \n"

		DEREF_JMP(ASMPlayerArmorHandler_jmpback)
	);
}

void SetupPlayerArmorHandler() {
	WriteFarJMP(Offset(base, 0x4F1E4), (void*)&ASMPlayerArmorHandler);
	ASMPlayerArmorHandler_jmpback = Offset(base, 0x4F1F2);
}
