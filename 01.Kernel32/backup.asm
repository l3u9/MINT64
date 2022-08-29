[ORG 0X00]
[BITS 16]

SECTION .text

jmp 0x1000:START

SECTORCOUNT: dw 0x0000 ;save current sector number to execute
TOTALSECTORCOUNT equ 1024 ;total count sector in virtual os
                            ;maximum 1152 sector (0x90000byte)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; CODE Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

START:
    mov ax, cs
    mov ds, ax
    mov ax, 0xb800
    mov es, ax

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Generate Code each sector
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    %assign i 0
    %rep TOTALSECTORCOUNT
        %assign i i+1

        mov ax, 2

        mul word [SECTORCOUNT]
        mov si, ax
        
        mov byte [es: si + (160 * 2)], '0' + (i % 10)
        add word [SECTORCOUNT], 1

        %if i == TOTALSECTORCOUNT


            jmp $
        %else
            jmp ( 0x1000 + i * 0x20): 0x0000
        %endif

        times ( 512 - ($ - $$) % 512)   db 0x00
    %endrep



; START:
;     mov ax, cs
;     mov ds, ax
;     mov ax, 0xb800
;     mov es, ax

;     mov ax, 2
;     mul word [SECTORCOUNT]
;     mov si, ax
;     mov byte [es: si + (160*2)], 0

;     add word [SECTORCOUNT], 1

;     jmp $

;     times 512 - ($ - $$) db 0x00
