section .multiboot2
align 8
header_start:
    dd 0xE85250D6
    dd 0
    dd header_end - header_start
    dd -(0xE85250D6 + 0 + (header_end - header_start))

    dw 5
    dw 1
    dd 20
    dd 1920
    dd 1080
    dd 32

    dw 0
    dw 0
    dd 8
header_end:

section .text
bits 32
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
    mov dword [multiboot_info], ebx
    
    call check_cpu_features
    
    call setup_page_tables
    call enable_paging
    
    call _enable_fpu
    call _enable_sse
    
    lgdt [gdt64.pointer]
    jmp gdt64.code:long_mode_start
    
    cli
    hlt

check_cpu_features:
    call _detect_cpuid
    test eax, eax
    jz .no_cpuid
    
    call _detect_long_mode
    test eax, eax
    jz .no_long_mode
    
    ret

.no_cpuid:
    mov esi, no_cpuid_msg
    jmp _fatal_error

.no_long_mode:
    mov esi, no_long_mode_msg

_fatal_error:
    mov edi, 0xB8000
    mov ah, 0x4F
.loop:
    lodsb
    test al, al
    jz .done
    stosw
    jmp .loop
.done:
    jmp _reboot

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

setup_page_tables:
    mov eax, pdp_table
    or eax, 0b11
    mov [pml4_table], eax

    mov eax, pd_table
    or eax, 0b11
    mov [pdp_table], eax

    mov ecx, 0
.map_pd_table:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [pd_table + ecx * 8], eax
    
    inc ecx
    cmp ecx, 512
    jne .map_pd_table
    
    ret

enable_paging:
    mov eax, pml4_table
    mov cr3, eax
    
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    ret

bits 64
long_mode_start:
    mov ax, gdt64.data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    mov rsp, stack_top
    
    mov rdi, [multiboot_info]
    call kernel_main
    
    cli
    hlt

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53)
.data: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .text
bits 32

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

section .data
align 4

multiboot_info: dd 0
no_cpuid_msg: db "CPUID not supported - System halted", 0
no_long_mode_msg: db "64-bit mode not supported - System halted", 0

section .bss
align 4096
pml4_table:
    resb 4096
pdp_table:
    resb 4096
pd_table:
    resb 4096

align 16
stack_bottom:
    resb 16384
stack_top:
