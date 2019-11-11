extern "C" float PlayerResistanceHandler(void* player, float resistance) {
	float modifiedResistance = resistance;
	for (DLL* dll : modDLLs) {
		modifiedResistance = dll->mod->ModifyPlayerResistance(player, modifiedResistance);
	}
	return modifiedResistance;
}

GETTER_VAR(void*, ASMPlayerResistanceHandler_jmpback);
__attribute__((naked)) void ASMPlayerResistanceHandler() {
	asm(".intel_syntax \n"

		// check if the creature is a player
		"cmp byte ptr [rbx+0x60], 0 \n"
		"jne ASMPlayerResistanceHandlerNotPlayer \n"

		PUSH_ALL

		"mov rcx, rbx \n" // cube::Creature*
		"movss xmm1, xmm0 \n"

		PREPARE_STACK

		"call PlayerResistanceHandler \n"

		RESTORE_STACK

		POP_ALL

		"ASMPlayerResistanceHandlerNotPlayer: \n"

		// original code
		"mov rbx, [rsp+0x40] \n"
		"movaps xmm6, [rsp+0x20] \n"
		"add rsp, 0x30 \n"

		DEREF_JMP(ASMPlayerResistanceHandler_jmpback)
	);
}

void SetupPlayerResistanceHandler() {
	WriteFarJMP(Offset(base, 0x64EF4), (void*)&ASMPlayerResistanceHandler);
	ASMPlayerResistanceHandler_jmpback = Offset(base, 0x64F02);
}
