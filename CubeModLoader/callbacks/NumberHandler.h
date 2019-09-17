// Example handler
void NumberHandler(int number) {
    for (DLL* dll: modDLLs) {
        if (dll->HandleNumber) {
            ((void(*)(int))dll->HandleNumber)(number);
        }
    }
}
void* NumberHandler_ptr = (void*)&NumberHandler;

void* ASMNumberHandler_jmpback;
char ASMNumberHandler_formatstring[] = "%d...\n";
void no_optimize ASMNumberHandler() {
    asm("call rax \n"
        "mov eax, [rbp-4] \n"
        "push rax\n push rcx\n push rdx\n push r8\n push r9\n push r10\n push r11\n"

        "sub rsp, 0x20 \n"
        "mov ecx, eax \n"
        "call [NumberHandler_ptr] \n"
        "add rsp, 0x20 \n"

        "pop r11\n pop r10\n pop r9\n pop r8\n pop rdx\n pop rcx\n pop rax\n"

        "mov edx, eax \n"
        "lea rcx, [ASMNumberHandler_formatstring] \n"
        "jmp [ASMNumberHandler_jmpback]"
       );
}
void SetupNumberHandler() {
    WriteFarJMP(base+0x1597, (void*)&ASMNumberHandler);
    ASMNumberHandler_jmpback = (void*)base+0x15A5;
}
