#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void run_command(const char* command) {
    printf("%s\n", command);
    int result = system(command);
    if (result != 0) {
        printf("Command failed with exit code: %d\n", result);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "clean") == 0) {
        run_command("rm -f mkfs.bkfs");
        run_command("rm -f boot/boot.o");
        run_command("rm -f src/kernel.o");
        run_command("rm -f iso/boot/krn.img");
        run_command("rm -f srunix86.iso");
        return 0;
    }

    run_command("gcc -O2 -o mkfs.bkfs fs/mkfs_bkfs.c");
    run_command("nasm -f elf64 boot/boot.asm -o boot/boot.o");
    run_command("gcc -m64 -c src/kernel/kernel.c -o src/kernel.o -ffreestanding -nostdlib -O2");
    run_command("ld -m elf_x86_64 -T boot/linker.ld -o iso/boot/krn.img boot/boot.o src/kernel.o -nostdlib -static");
    run_command("grub-mkrescue -o srunix86.iso iso");
    
    return 0;
}
