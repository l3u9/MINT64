[ORG 0X00]
[BITS 16]

SECTION .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Code Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

START:
    mov ax, 0x1000
    mov ds, ax
    mov es, ax

    cli ; setting disable interupt occured
    lgdt [GDTR] ; GDT table load GDTR data Structure to Processor
    
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
    ; Enter to Protected Mode
    ; Disable Paging, Disable Cache, Internal FPU, Disable Align Check, Enable Protected Mode
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    mov eax, 0x4000003b ; PG = 0, CD = 1, NW = 0, AM = 0, WP = 0, NE = 1, ET = 1, TS = 1, EM = 0, MP = 1, PE = 1
    mov cr0, eax
    
    ;change kernel code segment that based on 0x00 and reset eip based 0x00
    ;CS segment selector : EIP
    jmp dword 0x08: (PROTECTEDMODE - $$ + 0X10000)

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Enter to Protected Mode
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[BITS 32]
PROTECTEDMODE:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ss, ax
    mov esp, 0xfffe
    mov ebp, 0xfffe


    ;print message "Switch To Protected Mode Success ~!!"
    push (SWITCHSUCCESSMESSAGE - $$ + 0x10000)
    push 2
    push 0
    call PRINTMESSAGE
    add esp, 12

    jmp $


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; function code section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; print message function
; x value, y value, stream in stack
PRINTMESSAGE:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push eax
    push ecx
    push edx

    ;calc address using x,y
    mov eax, dword [ebp + 12] ;get y value
    mov esi, 160 ;1 line byte = 80 * 2
    mul esi
    mov edi, eax

    mov eax, dword [ebp + 8] ;get x value
    mov esi, 2
    mul esi
    add edi, eax

    mov esi, dword [ebp + 16] ; stream address

.MESSAGELOOP:
    mov cl, byte [esi]
    cmp cl, 0
    je .MESSAGEEND
    
    mov byte [edi + 0xb8000], cl
    add esi, 1
    add edi, 2

    jmp .MESSAGELOOP

.MESSAGEEND:
    pop edx
    pop ecx
    pop eax
    pop edi
    pop esi
    pop ebp
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Data Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 8, db 0 ; 8 byte align

; addition this to sort 8byte at end of GDTR
dw 0x0000

; define GDTR data structure
GDTR:
    dw GDTEND - GDT - 1
    dd (GDT - $$ + 0x10000)

; define GDT table
GDT:
    ; NULL descriptor
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00
    
    ;Protected Mode code segment discriptor in kernel
    CODEDESCRIPTOR:
        dw 0xffff ; limit [15:0]
        dw 0x0000 ; base [15:0]
        db 0x00   ; base [23:16]
        db 0x9a   ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xCF   ; G=1, D=1, L=0, Limit[19:16]
        db 0x00   ; base [31:24]

    ;Protected Mode data segment descriptor in kernel
    DATADESCRIPTOR:
        dw 0xffff ; limit [15:0]
        dw 0x0000 ; base [15:0]
        db 0x00   ; base [23:16]
        db 0x92   ; P=1, DPL=0, Code Segment, Read/Write
        db 0xCF   ; G=1, D=1, L=0, Limit[19:16]
        db 0x00   ; base [31:24]

GDTEND:
    SWITCHSUCCESSMESSAGE: db 'Switch To Protected Mode Success ~!!', 0
    times 512 - ($ - $$) db 0x00





