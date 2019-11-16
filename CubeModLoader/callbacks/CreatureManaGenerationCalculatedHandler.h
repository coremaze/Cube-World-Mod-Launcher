extern "C" int CreatureManaGenerationCalculatedHandler(void* creature, float* regeneration) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureManaGenerationCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureManaGenerationCalculated(creature, regeneration);
			}
		}
	}
	return 0;
}

void ASM_CreatureManaGenerationCalculatedHandler() {
	asm(".intel_syntax \n"

		"push rcx \n" //Remember cube::Creature*

		// This function is so short that we're going to rewrite the entire thing in order to get:
		// 1: The cube::Creature*, the initial value of rcx
		// 2: The return point for the case when the creature has no buffs
		// 3: The return point for the case when the creature has buffs


		"mov rcx, [rcx+0x980] \n"
		"mov rax, [rcx] \n"
		"cmp rax, rcx \n"
		"jz 2f \n"
		"1: \n"
		"cmp byte ptr [rax+0x10], 0x22 \n"
		"jz 3f \n"
		"mov rax, [rax] \n"
		"cmp rax, rcx \n"
		"jnz 1b \n"
		"2: \n"
		"movss xmm0, [rip+2] \n"
		"jmp 4f \n"
		".float 1.0 \n"
		"4: "
		"jmp 7f \n" //replacement for ret
		"3: \n"
		"add rax, 0x10 \n"
		"jz 2b\n"
		"movss xmm1, dword ptr [rax+4] \n"
		"mulss xmm1, [rip+2] \n"
		"jmp 5f \n"
		".float 0.1 \n"
		"5: \n"
		"movss xmm0, [rip+2] \n"
		"jmp 6f \n"
		".float 1.0 \n"
		"6: \n"
		"subss xmm0, xmm1 \n"
		"jmp 7f \n" //replacement for ret
		

		"7: \n"
		PUSH_ALL

		//Get creature
		"mov rcx, [rsp+0x78] \n"

		// put result on stack
		"movq rax, xmm0 \n"
		"push rax \n"
		"lea rdx, [rsp] \n"

		PREPARE_STACK
		"call CreatureManaGenerationCalculatedHandler \n"
		RESTORE_STACK

		"pop rax \n"
		"movq xmm0, rax \n"

		POP_ALL

		"add rsp, 8 \n" //pop creature

		"ret \n"
	);
}
void SetupCreatureManaGenerationCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x5F8D0), (void*)&ASM_CreatureManaGenerationCalculatedHandler);
}
