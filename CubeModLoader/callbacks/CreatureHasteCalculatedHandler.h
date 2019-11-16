extern "C" int CreatureHasteCalculatedHandler(void* creature, float* haste) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnCreatureHasteCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureHasteCalculated(creature, haste);
			}
		}
	}
	return 0;
}

void ASM_CreatureHasteCalculatedHandler() {
	asm(".intel_syntax \n"


		"mov [rsp-0x80], rbx \n" //remember cube::Creature*

		// Due to an awkward function, we need to copy much of the original code
		"mov rbx, [rsp+0x50] \n"
		"shr eax, 9 \n"
		"test al, 1 \n"
		"jz 0f \n"

		"mulss xmm7, [rip+0x02] \n" //0.75
		"jmp 0f \n"
		".float 0.75 \n"
		"0: \n"

		"mov eax, ecx \n"
		"shr eax, 0x12 \n"
		"test al, 1 \n"
		"jz 1f \n"

		"mulss xmm7, [rip+0x02] \n" //0.6
		"jmp 1f \n"
		".float 0.6 \n"
		"1: \n"

		"bt ecx, 0x1A \n"
		"jnb 2f \n"

		"mulss xmm7, [rip+0x02] \n" //1.1
		"jmp 2f \n"
		".float 1.1 \n"
		"2: \n"

		"movaps xmm0, xmm7 \n"

		//Okay, now handle.


		PUSH_ALL

		"mov rcx, [rsp-0x8] \n" //Get the cube::Creature* back

		// put result on stack
		"movq rax, xmm0 \n"
		"push rax \n"
		"lea rdx, [rsp] \n"

		PREPARE_STACK
		"call CreatureHasteCalculatedHandler \n"
		RESTORE_STACK

		"pop rax \n"
		"movq xmm0, rax \n"

		POP_ALL

		// original code
		"movaps xmm7, [rsp+0x30] \n"
		"add rsp, 0x58 \n"
		"ret \n"
	);
}
void SetupCreatureHasteCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x66652), (void*)&ASM_CreatureHasteCalculatedHandler);
}
