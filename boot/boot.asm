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
    dd 1920
    dd 1080
    dd 32

section .text
global start
global gdt_flush
global idt_flush
global tss_flush
global load_page_directory
global enable_paging
global _read_cr0
global _write_cr0
global _read_cr2
global _read_cr3
global _write_cr3
global _enable_sse
global _enable_fpu
global _disable_interrupts
global _enable_interrupts
global _cli
global _sti
global _hlt
global _reboot
global _io_wait
global _detect_cpuid
global _detect_long_mode
extern kernel_main

start:
    cli
    mov esp, stack_top
    call _detect_cpuid
    test eax, eax
    jz .no_cpuid
    call _detect_long_mode
    test eax, eax
    jz .no_long_mode
    push eax
    push ebx
    call _enable_fpu
    call _enable_sse
    call setup_gdt
    call setup_idt
    call setup_tss
    mov eax, [esp + 4]
    mov ebx, eax
    add ebx, 48
    push ebx
    push dword 1920
    push dword 1080
    push eax
    push dword [esp + 20]
    call kernel_main
    cli
    hlt

.no_cpuid:
    mov esi, no_cpuid_msg
    call _print_error
    jmp _reboot

.no_long_mode:
    mov esi, no_long_mode_msg
    call _print_error
    jmp _reboot

_print_error:
    mov edi, 0xB8000
    mov ah, 0x4F
.loop:
    lodsb
    test al, al
    jz .done
    stosw
    jmp .loop
.done:
    ret

_detect_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 0x00200000
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    shr eax, 21
    and eax, 1
    ret

_detect_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    mov eax, 1
    ret
.no_long_mode:
    xor eax, eax
    ret

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

tss_flush:
    mov ax, 0x2B
    ltr ax
    ret

load_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

enable_paging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

_read_cr0:
    mov eax, cr0
    ret

_write_cr0:
    mov eax, [esp + 4]
    mov cr0, eax
    ret

_read_cr2:
    mov eax, cr2
    ret

_read_cr3:
    mov eax, cr3
    ret

_write_cr3:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

_enable_sse:
    mov eax, cr0
    and ax, 0xFFFB
    or ax, 0x2
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9
    mov cr4, eax
    ret

_enable_fpu:
    finit
    ret

_disable_interrupts:
    cli
    ret

_enable_interrupts:
    sti
    ret

_cli:
    cli
    ret

_sti:
    sti
    ret

_hlt:
    hlt
    ret

_reboot:
    mov al, 0xFE
    out 0x64, al
    ret

_io_wait:
    out 0x80, al
    ret

setup_gdt:
    lgdt [gdt_descriptor]
    ret

setup_idt:
    lidt [idt_descriptor]
    ret

setup_tss:
    mov ax, 0x2B
    ltr ax
    ret

section .data
align 4

no_cpuid_msg: db "Error: CPUID not supported", 0
no_long_mode_msg: db "Error: Long mode not supported", 0

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start

gdt_start:
    dd 0x00000000
    dd 0x00000000
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
    dw 0x0068
    dw 0x0000
    db 0x00
    db 0x89
    db 0x00
    db 0x00
gdt_end:

idt_start:
    times 256 dd 0x00000000
idt_end:

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
