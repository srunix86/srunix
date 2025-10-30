void execute_poweroff() {
    terminal_writestring("");
    outw(0x604, 0x2000);
    outw(0xB004, 0x2000);
    outw(0x4004, 0x3400);
    asm volatile ("cli");
    while (1) asm volatile ("hlt");
}

void execute_reboot() {
    terminal_writestring("Reboot\n");
    uint8_t temp = inb(0x64);
    while (temp & 0x02)
        temp = inb(0x64);
    outb(0x64, 0xFE);
    asm volatile ("cli");
    asm volatile ("int $0xFF");
    while (1);
}
