extern "C" int CreatureResistanceCalculatedHandler(void* creature, float* resistance) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureResistanceCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureResistanceCalculated(creature, resistance);
			}
		}
	}
	return 0;
}

void ASM_CreatureResistanceCalculatedHandler() {
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
		"call CreatureResistanceCalculatedHandler \n"
		RESTORE_STACK

		"pop rax \n"
		"movq xmm0, rax \n"

		POP_ALL

		// original code
		"mov rbx, [rsp+0x40] \n"
		"movaps xmm6, [rsp+0x20] \n"
		"add rsp, 0x30 \n"
		"pop rdi \n"
		"ret \n"
	);
}
void SetupCreatureResistanceCalculatedHandler() {
	// cube::Creature::GetResistance has two returns
	WriteFarJMP(Offset(base, 0x64EF1), (void*)&ASM_CreatureResistanceCalculatedHandler);
	WriteFarJMP(Offset(base, 0x64F0F), (void*)&ASM_CreatureResistanceCalculatedHandler);
}
