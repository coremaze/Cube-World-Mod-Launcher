extern "C" int CreatureSpellPowerCalculatedHandler(void* creature, float* power) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureSpellPowerCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureSpellPowerCalculated(creature, power);
			}
		}
	}
	return 0;
}

void ASM_CreatureSpellPowerCalculatedHandler() {
	asm(".intel_syntax \n"
		PUSH_ALL

		// put result on stack
		"movq rax, xmm0 \n"
		"push rax \n"
		"lea rdx, [rsp] \n"

		//Get creature
		"mov rcx, rsi \n"

		PREPARE_STACK
		"call CreatureSpellPowerCalculatedHandler \n"
		RESTORE_STACK

		"pop rax \n"
		"movq xmm0, rax \n"

		POP_ALL

		// original code
		"mov rbp, [r11+0x30] \n"
		"movaps xmm6, [rsp+0x60] \n"
		"mov rsp, r11 \n"
		"pop r14 \n"
		"pop rdi \n"
		"pop rsi \n"
		"ret \n"
	);
}
void SetupCreatureSpellPowerCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x65E84), (void*)&ASM_CreatureSpellPowerCalculatedHandler);
}
