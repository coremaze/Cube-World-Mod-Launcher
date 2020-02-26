extern "C" int ChunkRemeshHandler(void* zone) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnChunkRemeshPriority == (GenericMod::Priority)priority) {
				dll->mod->OnChunkRemesh(zone);
			}
		}
	}
	return 0;
}

GETTER_VAR(void*, ASM_ChunkRemeshHandler_jmpback);
void ASM_ChunkRemeshHandler() {
	asm(".intel_syntax \n"
		PUSH_ALL

		PREPARE_STACK

		"mov rcx, r13 \n"
		"call ChunkRemeshHandler \n"

		RESTORE_STACK


		POP_ALL

		// original code
		"lea ebx, [r14+0x40] \n"
		"mov [rbp-0x54], ebx \n"
		"lea edi, [r15+0x40] \n"
		"mov [rbp-0x64], edi \n"

		DEREF_JMP(ASM_ChunkRemeshHandler_jmpback)
	);
}
void SetupChunkRemeshHandler() {
	WriteFarJMP(Offset(base, 0xE969C), (void*)&ASM_ChunkRemeshHandler);
	ASM_ChunkRemeshHandler_jmpback = Offset(base, 0xE96AA);
}
