[BITS 64]

SECTION .text

;외부에서 정의된 함수를 쓸 수 있도록 선언 (import)
extern Main
extern g_qwAPICIDAddress, g_iWakeUpApplicationProcessorCount

;코드 영역
START:
    mov ax, 0x10    ;IA-32e 모드 커널용 데이터 세그먼트 디스크립터를 AX 레지스터에 저장
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ;스택을 0x600000 ~ 0x6fffff 영역에 1MB 크기로 생성
    mov ss, ax
    mov rsp, 0x6ffff8
    mov rbp, 0x6ffff8

    cmp byte [0x7c09], 0x01
    je .BOOTSTRAPPROCESSORSTARTPOINT

    mov rax, 0
    mov rbx, qword [g_qwAPICIDAddress]
    mov eax, dword [rbx]
    shr rax, 24

    mov rbx, 0x10000
    mul rbx

    sub rsp, rax
    sub rbp, rax

    lock inc dword [g_iWakeUpApplicationProcessorCount]

.BOOTSTRAPPROCESSORSTARTPOINT
    call Main
    jmp $


