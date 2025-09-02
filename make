#!/bin/bash
nasm -f elf64 boot/boot.asm -o boot/boot.o
gcc -m64 -c src/kernel.c -o src/kernel.o -ffreestanding -nostdlib -O2 -fno-stack-protector -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2 -mno-avx -mno-avx2 -mno-fma -msoft-float -mgeneral-regs-only -fno-builtin -fno-tree-vectorize
ld -m elf_x86_64 -T boot/linker.ld -o iso/boot/krn.img boot/boot.o src/kernel.o -nostdlib -z noexecstack -static
grub-mkrescue -o livecd64.iso iso
