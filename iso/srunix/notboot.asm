section .multiboot
align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000001
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000

section .text
global start
extern kernel_main

start:
    cli
    mov esp, stack_top
    push eax
    push ebx
    
    call setup_gdt
    call setup_idt
    call setup_tss
    
    call kernel_main
    cli
    hlt

setup_gdt:
    lgdt [gdt_descriptor]
    ret

setup_idt:
    lidt [idt_descriptor]
    ret

setup_tss:
    mov eax, tss_entry
    mov [gdt_tss + 2], ax
    shr eax, 16
    mov [gdt_tss + 4], al
    mov [gdt_tss + 7], ah
    
    mov ax, 0x28
    ltr ax
    ret

section .data
align 4

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_start:
    dq 0
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xFA
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xF2
    db 0xCF
    db 0x00
    
gdt_tss:
    dw 0x0067
    dw 0x0000
    db 0x00
    db 0x89
    db 0x00
    db 0x00
gdt_end:

idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start

idt_start:
    times 256 dq 0
idt_end:

section .tss
align 4
tss_entry:
    dd 0x00000000
    dd stack_top
    dd 0x10
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dw 0x0000
    dw 0x0068

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:section .multiboot
align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000001
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000

section .text
global start
extern kernel_main

start:
    cli
    mov esp, stack_top
    push eax
    push ebx
    
    call setup_gdt
    call setup_idt
    call setup_tss
    
    call kernel_main
    cli
    hlt

setup_gdt:
    lgdt [gdt_descriptor]
    ret

setup_idt:
    lidt [idt_descriptor]
    ret

setup_tss:
    mov eax, tss_entry
    mov [gdt_tss + 2], ax
    shr eax, 16
    mov [gdt_tss + 4], al
    mov [gdt_tss + 7], ah
    
    mov ax, 0x28
    ltr ax
    ret

section .data
align 4

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_start:
    dq 0
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xFA
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xF2
    db 0xCF
    db 0x00
    
gdt_tss:
    dw 0x0067
    dw 0x0000
    db 0x00
    db 0x89
    db 0x00
    db 0x00
gdt_end:

idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start

idt_start:
    times 256 dq 0
idt_end:

section .tss
align 4
tss_entry:
    dd 0x00000000
    dd stack_top
    dd 0x10
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dw 0x0000
    dw 0x0068

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:section .multiboot
align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000001
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000

section .text
global start
extern kernel_main

start:
    cli
    mov esp, stack_top
    push eax
    push ebx
    
    call setup_gdt
    call setup_idt
    call setup_tss
    
    call kernel_main
    cli
    hlt

setup_gdt:
    lgdt [gdt_descriptor]
    ret

setup_idt:
    lidt [idt_descriptor]
    ret

setup_tss:
    mov eax, tss_entry
    mov [gdt_tss + 2], ax
    shr eax, 16
    mov [gdt_tss + 4], al
    mov [gdt_tss + 7], ah
    
    mov ax, 0x28
    ltr ax
    ret

section .data
align 4

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_start:
    dq 0
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xFA
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xF2
    db 0xCF
    db 0x00
    
gdt_tss:
    dw 0x0067
    dw 0x0000
    db 0x00
    db 0x89
    db 0x00
    db 0x00
gdt_end:

idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start

idt_start:
    times 256 dq 0
idt_end:

section .tss
align 4
tss_entry:
    dd 0x00000000
    dd stack_top
    dd 0x10
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dw 0x0000
    dw 0x0068

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:section .multiboot
align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000001
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000

section .text
global start
extern kernel_main

start:
    cli
    mov esp, stack_top
    push eax
    push ebx
    
    call setup_gdt
    call setup_idt
    call setup_tss
    
    call kernel_main
    cli
    hlt

setup_gdt:
    lgdt [gdt_descriptor]
    ret

setup_idt:
    lidt [idt_descriptor]
    ret

setup_tss:
    mov eax, tss_entry
    mov [gdt_tss + 2], ax
    shr eax, 16
    mov [gdt_tss + 4], al
    mov [gdt_tss + 7], ah
    
    mov ax, 0x28
    ltr ax
    ret

section .data
align 4

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_start:
    dq 0
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xFA
    db 0xCF
    db 0x00
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xF2
    db 0xCF
    db 0x00
    
gdt_tss:
    dw 0x0067
    dw 0x0000
    db 0x00
    db 0x89
    db 0x00
    db 0x00
gdt_end:

idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start

idt_start:
    times 256 dq 0
idt_end:

section .tss
align 4
tss_entry:
    dd 0x00000000
    dd stack_top
    dd 0x10
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dw 0x0000
    dw 0x0068

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
