extern "C" int ChunkRemeshedHandler(void* zone) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnChunkRemeshedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnChunkRemeshed(zone);
			}
		}
	}
	return 0;
}

GETTER_VAR(void*, ASM_ChunkRemeshedHandler_jmpback);
void ASM_ChunkRemeshedHandler() {
	asm(".intel_syntax \n"
		PUSH_ALL

		PREPARE_STACK

		"mov rcx, r8 \n"
		"call ChunkRemeshedHandler \n"

		RESTORE_STACK


		POP_ALL

		// original code
		"mov rdx, [rdi] \n"
		"mov [rdi], rdi \n"
		"mov rax, [rbp+0x20] \n"
		"mov [rax+8], rax \n"

		DEREF_JMP(ASM_ChunkRemeshedHandler_jmpback)
	);
}
void SetupChunkRemeshedHandler() {
	WriteFarJMP(Offset(base, 0xECB5F), (void*)&ASM_ChunkRemeshedHandler);
	ASM_ChunkRemeshedHandler_jmpback = Offset(base, 0xECB6D);
}
