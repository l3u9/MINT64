[BITS 64]

SECTION .text

global kInPortByte, kOutPortByte

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
    
