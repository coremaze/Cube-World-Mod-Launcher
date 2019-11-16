extern "C" int ZoneDestroyHandler(void* zone) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll->mod->OnZoneDestroyPriority == (GenericMod::Priority)priority) {
				dll->mod->OnZoneDestroy(zone);
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_ZoneDestroyHandler_jmpback);
void ASM_ZoneDestroyHandler() {
    asm(".intel_syntax \n"
		PUSH_ALL
        PREPARE_STACK

        "call ZoneDestroyHandler \n"

        RESTORE_STACK
        POP_ALL

        // original code
        "push rsi \n"
        "push rdi \n"
        "push r14 \n"
		"sub rsp, 0x30 \n"
		"mov qword ptr [rsp+0x20], 0x0FFFFFFFFFFFFFFFE \n"

        DEREF_JMP(ASM_ZoneDestroyHandler_jmpback)
       );
}
void SetupZoneDestroyHandler() {
    WriteFarJMP(Offset(base, 0x2F9940), (void*)&ASM_ZoneDestroyHandler);
	ASM_ZoneDestroyHandler_jmpback = Offset(base, 0x2F9952);
}
