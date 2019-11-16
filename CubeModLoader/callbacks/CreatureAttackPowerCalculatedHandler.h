extern "C" int CreatureAttackPowerCalculatedHandler(void* creature, float* power) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureAttackPowerCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureAttackPowerCalculated(creature, power);
			}
		}
	}
	return 0;
}

void ASM_CreatureAttackPowerCalculatedHandler() {
	asm(".intel_syntax \n"
		PUSH_ALL

		// put result on stack
		"movq rax, xmm0 \n"
		"push rax \n"
		"lea rdx, [rsp] \n"

		//Get creature
		"mov rcx, rsi \n"

		PREPARE_STACK
		"call CreatureAttackPowerCalculatedHandler \n"
		RESTORE_STACK

		"pop rax \n"
		"movq xmm0, rax \n"

		POP_ALL

		// original code
		"movaps xmm6, [rsp+0x70] \n"
		"movaps xmm7, [rsp+0x60] \n"
		"mov rsp, r11 \n"
		"pop r14 \n"
		"pop rdi \n"
		"pop rsi \n"
		"ret \n"
	);
}
void SetupCreatureAttackPowerCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x4FD2B), (void*)&ASM_CreatureAttackPowerCalculatedHandler);
}
