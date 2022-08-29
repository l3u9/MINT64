[ORG 0x00] ; start address of code set 0x00
[BITS 16] ; code setting to 16-bit

SECTION .text

mov ax, 0xB800
mov ds, ax

mov byte [0x00] , 'M'
mov byte [0x01] , 0x4A


jmp $   ; infinite loop in current position

times 510 - ( $ -$$ )   db 0x00 ; $: current line address
                                ; $$: Start address of current Section(.text)
                                ; $ - $$: Offset based on current Section
                                ; 510 - ( $ - $$ ): until current address to 510
                                ; db 0x00: define 1bytes value to 0x00
                                ; perform time(loop)
                                ; fill value (0x00) current address to 510 address

db 0x55
db 0xAA ;0x55, 0xAA is BootLoader Signiture






