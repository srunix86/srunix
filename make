#!/bin/bash
nasm -f elf32 boot/boot.asm -o boot/boot.o
gcc -m32 -c src/kernel.c -o src/kernel.o -ffreestanding -nostdlib -O2 -fno-stack-protector
gcc -m32 -c src/install.c -o src/install.o -ffreestanding -nostdlib -O2 -fno-stack-protector
ld -m elf_i386 -T boot/linker.ld -o iso/boot/inll.img boot/boot.o src/install.o -nostdlib -z noexecstack
ld -m elf_i386 -T boot/linker.ld -o iso/boot/krn.img boot/boot.o src/kernel.o -nostdlib -z noexecstack
grub-mkrescue -o livecd64.iso iso
