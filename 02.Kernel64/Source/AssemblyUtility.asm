[BITS 64]

SECTION .text

global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS
global kReadTSC

;포트로부터 1바이트 읽음
kInPortByte:
    push rdx

    mov rdx, rdi
    mov rax, 0
    in al, dx

    pop rdx
    
    ret

; 포트에 1바이트 씀
kOutPortByte:
    push rdx
    push rax
    
    mov rdx, rdi
    mov rax, rsi
    out dx, al

    pop rax
    pop rdx
    ret


kLoadGDTR:
    lgdt [rdi];
    ret

kLoadTR:
    ltr di
    ret

kLoadIDTR:
    lidt [rdi]
    ret


kEnableInterrupt:
    sti
    ret

kDisableInterrupt:
    cli
    ret

kReadRFLAGS:
    pushfq
    pop rax

    ret

kReadTSC:
    push rdx

    rdtsc

    shl rdx, 32
    or rax, rdx

    pop rdx
    ret

