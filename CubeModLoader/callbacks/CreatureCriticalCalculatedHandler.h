extern "C" int CreatureCriticalCalculatedHandler(void* creature, float* critical) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureCriticalCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureCriticalCalculated(creature, critical);
			}
		}
	}
	return 0;
}

void ASM_CreatureCriticalCalculatedHandler() {
	asm(".intel_syntax \n"

		"movaps xmm0, xmm6 \n" //get result

		PUSH_ALL

		// put result on stack
		"movq rax, xmm0 \n"
		"push rax \n"
		"lea rdx, [rsp] \n"

		//Get creature
		"mov rcx, rbx \n"

		PREPARE_STACK
		"call CreatureCriticalCalculatedHandler \n"
		RESTORE_STACK

		"pop rax \n"
		"movq xmm0, rax \n"

		POP_ALL

		// original code
		"mov rbx, [rsp+0x40] \n"
		"movaps xmm6, [rsp+0x20] \n"
		"mov rsi, [rsp+0x48] \n"
		"add rsp, 0x30 \n"
		"pop rdi \n"
		"ret \n"
	);
}
void SetupCreatureCriticalCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x50E7A), (void*)&ASM_CreatureCriticalCalculatedHandler);
}
