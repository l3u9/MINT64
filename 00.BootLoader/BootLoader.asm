[ORG 0x00] ; start address of code set 0x00
[BITS 16] ; code setting to 16-bit

SECTION .text

jmp 0x07c0:START ;copy 0x07c0 to CS segment and jump START LABLE

START:
    mov ax, 0x07c0
    mov ds, ax
    mov ax, 0xb800
    mov es, ax
    
    mov si, 0

.SCREENCLEARLOOP:
    mov byte [es: si], 0
    
    mov byte [es: si+1], 0x0a
    
    add si, 2

    cmp si, 80*25*2

    jl .SCREENCLEARLOOP


    mov si, 0
    mov di, 0

.MESSAGELOOP:
    mov cl, byte [si + .MESSAGE1]
    
    cmp cl, 0
    je .MESSAGEEND

    mov byte [es: di], cl

    add si, 1
    add di, 2

    jmp .MESSAGELOOP

.MESSAGEEND:
    jmp $

.MESSAGE1:
    db 'MINT64 OS Boot Loader Start~!!', 0





times 510 - ( $ -$$ )   db 0x00 ; $: current line address
                                ; $$: Start address of current Section(.text)
                                ; $ - $$: Offset based on current Section
                                ; 510 - ( $ - $$ ): until current address to 510
                                ; db 0x00: define 1bytes value to 0x00
                                ; perform time(loop)
                                ; fill value (0x00) current address to 510 address

db 0x55
db 0xAA ;0x55, 0xAA is BootLoader Signiture






