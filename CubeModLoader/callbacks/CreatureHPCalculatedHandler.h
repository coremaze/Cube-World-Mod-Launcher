extern "C" int CreatureHPCalculatedHandler(void* creature, float* hp) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureHPCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureHPCalculated(creature, hp);
			}
		}
	}
	return 0;
}

void ASM_CreatureHPCalculatedHandler() {
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
		"call CreatureHPCalculatedHandler \n"
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
void SetupCreatureHPCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x5FC11), (void*)&ASM_CreatureHPCalculatedHandler);
}
