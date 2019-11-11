extern "C" float PlayerHasteHandler(void* player, float haste) {
	float modifiedHaste = haste;
	for (DLL* dll : modDLLs) {
		modifiedHaste = dll->mod->ModifyPlayerHaste(player, modifiedHaste);
	}
	return modifiedHaste;
}

GETTER_VAR(void*, ASMPlayerHasteHandler_jmpback);
__attribute__((naked)) void ASMPlayerHasteHandler() {
	asm(".intel_syntax \n"

		// check if the creature is a player
		"cmp byte ptr [rbx+0x60], 0 \n"
		"jne ASMPlayerHasteHandlerNotPlayer \n"

		PUSH_ALL

		"mov rcx, rbx \n" // cube::Creature*
		"movss xmm1, xmm7 \n"

		PREPARE_STACK

		"call PlayerHasteHandler \n"

		RESTORE_STACK

		"movss xmm7, xmm0 \n"

		POP_ALL

		"ASMPlayerHasteHandlerNotPlayer: \n"

		"mov ecx, [rbx+0x80] \n"
		"mov eax, ecx \n"
		"mov rbx, [rsp+0x050] \n"
		"shr eax, 0x09 \n"

		DEREF_JMP(ASMPlayerHasteHandler_jmpback)
	);
}

void SetupPlayerHasteHandler() {
	WriteFarJMP(Offset(base, 0x6664A), (void*)&ASMPlayerHasteHandler);
	ASMPlayerHasteHandler_jmpback = Offset(base, 0x6665A);
}
