extern "C" int ZoneGeneratedHandler(void* world, void* zone) {
	for (uint64_t priority = 0; priority <= 5; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnZoneGeneratedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnZoneGenerated(world, zone);
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_ZoneGeneratedHandler_jmpback);
void ASM_ZoneGeneratedHandler() {
    asm(".intel_syntax \n"
		PUSH_ALL

		"mov rcx, r14 \n" // the world*
        "mov rdx, rbp \n" // new zone*

        PREPARE_STACK

        "call ZoneGeneratedHandler \n"

        RESTORE_STACK


        POP_ALL

        // original code
        "mov rax, [rsp+0x48] \n"
        "mov [rax+0x18], rbp \n"
        "lea rcx, [r14+0x3C8] \n"

        DEREF_JMP(ASM_ZoneGeneratedHandler_jmpback)
       );
}
void SetupZoneGeneratedHandler() {
    WriteFarJMP(Offset(base, 0x2AE34F), (void*)&ASM_ZoneGeneratedHandler);
	ASM_ZoneGeneratedHandler_jmpback = Offset(base, 0x2AE35F);
}
