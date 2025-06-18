all:
	nasm -f elf32 boot/boot.asm -o boot/boot.o
	gcc -m32 -c src/srunix.c -o src/srunix.o -ffreestanding -nostdlib -O2 -fno-stack-protector
	ld -m elf_i386 -T boot/linker.ld -o iso/boot/srunix.kernel boot/boot.o src/srunix.o -nostdlib -z noexecstack
	grub-mkrescue -o srunix.iso iso
