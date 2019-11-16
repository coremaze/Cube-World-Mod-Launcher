extern "C" int CreatureArmorCalculatedHandler(void* creature, float* armor) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureArmorCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureArmorCalculated(creature, armor);
			}
		}
	}
	return 0;
}

void ASM_CreatureArmorCalculatedHandler() {
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
		"call CreatureArmorCalculatedHandler \n"
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
void SetupCreatureArmorCalculatedHandler() {
	// cube::Creature::GetArmor has two returns
	WriteFarJMP(Offset(base, 0x4F1E1), (void*)&ASM_CreatureArmorCalculatedHandler);
	WriteFarJMP(Offset(base, 0x4F1FF), (void*)&ASM_CreatureArmorCalculatedHandler);
}
