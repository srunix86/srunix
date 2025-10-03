#!/bin/bash

if [ "$1" = "-h" ]; then
    nasm -f elf64 boot/boot.asm -o boot/boot.o
    gcc -m64 -c src/kernel/kernel.c -o src/sbin/kernel.o -ffreestanding -nostdlib -O2
    ld -m elf_x86_64 -T boot/linker.ld -o iso/boot/krn.img boot/boot.o src/sbin/kernel.o -nostdlib -static
    grub-mkrescue -o srunix86.iso iso
    exit 0
fi

if [ "$1" = "-s" ]; then
    nasm -f elf64 boot/boot.asm -o boot/boot.o
    gcc -m64 -c src/kernel/kernel.c -o src/sbin/kernel.o -ffreestanding -nostdlib -O2
    ld -m elf_x86_64 -T boot/linker.ld -o iso/boot/krn.img boot/boot.o src/sbin/kernel.o -nostdlib -static
    grub-mkrescue -o srunix86.iso iso > /dev/null 2>&1 &
    
    pid=$!
    while kill -0 $pid 2>/dev/null; do
        echo -ne "\rProgress: [                    ]"
        sleep 0.1
        echo -ne "\rProgress: [####                ]"
        sleep 0.1
        echo -ne "\rProgress: [########            ]"
        sleep 0.1
        echo -ne "\rProgress: [############        ]"
        sleep 0.1
        echo -ne "\rProgress: [################    ]"
        sleep 0.1
        echo -ne "\rProgress: [####################]"
        sleep 0.1
    done
    
    echo -e "\rCompilation successful             "
    qemu-system-x86_64 srunix86.iso
    exit 0
fi

nasm -f elf64 boot/boot.asm -o boot/boot.o
gcc -m64 -c src/kernel/kernel.c -o src/sbin/kernel.o -ffreestanding -nostdlib -O2
ld -m elf_x86_64 -T boot/linker.ld -o iso/boot/krn.img boot/boot.o src/sbin/kernel.o -nostdlib -static
grub-mkrescue -o srunix86.iso iso > /dev/null 2>&1 &

pid=$!
while kill -0 $pid 2>/dev/null; do
    echo -ne "\rProgress: [                      ]"
    sleep 0.6
    echo -ne "\rProgress: [ ###                  ]"
    sleep 0.2
    echo -ne "\rProgress: [ ########             ]"
    sleep 0.5
    echo -ne "\rProgress: [ ############         ]"
    sleep 0.2
    echo -ne "\rProgress: [ ################     ]"
    sleep 0.4
    echo -ne "\rProgress: [ #################### ]\n\n"
    sleep 0.1
    echo "Building bootloader... [ok]"
    sleep 0.3
    echo "Compiling kernel...    [ok]"
    sleep 0.2
    echo "Linking objects...     [ok]"
    sleep 0.1
    echo "Creating ISO image...  [ok]"
done

echo -e "\r \nCompilation successful!           "
