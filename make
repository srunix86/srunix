#!/bin/bash
nasm -f elf64 boot/boot.asm -o boot/boot.o
gcc -m64 -c src/kernel.c -o src/kernel.o -ffreestanding -nostdlib -O2 -fno-stack-protector
gcc -m64 -c src/install.c -o src/install.o -ffreestanding -nostdlib -O2 -fno-stack-protector
ld -m elf_x86_64 -T boot/linker.ld -o iso/boot/inll.img boot/boot.o src/install.o -nostdlib -z noexecstack
ld -m elf_x86_64 -T boot/linker.ld -o iso/boot/krn.img boot/boot.o src/kernel.o -nostdlib -z noexecstack
grub-mkrescue -o livecd64.iso iso
