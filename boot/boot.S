.global _start
_start:
    call kernel_main
    cli
    hlt
