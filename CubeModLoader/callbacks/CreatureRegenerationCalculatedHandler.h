extern "C" int CreatureRegenerationCalculatedHandler(void* creature, float* regeneration) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureRegenerationCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureRegenerationCalculated(creature, regeneration);
			}
		}
	}
	return 0;
}

void ASM_CreatureRegenerationCalculatedHandler() {
	asm(".intel_syntax \n"

		"movaps xmm0, xmm6 \n" // get result

		PUSH_ALL

		// put result on stack
		"movq rax, xmm0 \n"
		"push rax \n"
		"lea rdx, [rsp] \n"

		//Get creature
		"mov rcx, rbx \n"

		PREPARE_STACK
		"call CreatureRegenerationCalculatedHandler \n"
		RESTORE_STACK

		"pop rax \n"
		"movq xmm0, rax \n"

		POP_ALL

		// original code
		"movaps xmm6, [rsp+0x20] \n"
		"add rsp, 0x30 \n"
		"pop rbx \n"
		"ret \n"
	);
}
void SetupCreatureRegenerationCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x648A1), (void*)&ASM_CreatureRegenerationCalculatedHandler);
}
