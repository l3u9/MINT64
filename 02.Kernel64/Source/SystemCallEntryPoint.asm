[BITS 64]

SECTION .text

extern kProcessSystemCall

global kSystemCallEntryPoint, kSystemCallTestTask

kSystemCallEntryPoint:
    push rcx
    push r11
    mov cx, ds
    push cx
    mov cx, es
    push cx
    push fs
    push gs

    mov cx, 0x10 ;kernel data segment index
    mov dx, cx
    mov es, cx
    mov fs, cx
    mov gs, cx

    call kProcessSystemCall

    pop gs
    pop fs
    pop cx
    mov es, cx
    pop cx
    mov ds, cx
    pop r11
    pop rcx

    o64 sysret

kSystemCallTestTask:
    mov rdi, 0xffffffff
    mov rsi, 0x00
    syscall

    mov rdi, 0xffffffff
    mov rsi, 0x00
    syscall

    mov rdi, 0xffffffff
    mov rsi, 0x00
    syscall

    mov rdi, 24
    mov rsi, 0x00
    syscall
    jmp $

