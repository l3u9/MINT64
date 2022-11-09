[BITS 64]

SECTION .text

extern kCommonExceptionHandler, kCommonInterruptHandler, kKeyboardHandler
extern kTimerHandler, kDeviceNotAvailableHandler

global kISRDivideError, kISRDebug, kISRNMI, kISRBreakPoint, kISROverflow
global kISRBoundRangeExceeded, kISRInvalidOpcode, kISRDeviceNotAvailable, kISRDoubleFault,
global kISRCoprocessorSegmentOverrun, kISRInvalidTSS, kISRSegmentNotPresent
global kISRStackSegmentFault, kISRGeneralProtection, kISRPageFault, kISR15
global kISRFPUError, kISRAlignmentCheck, kISRMachineCheck, kISRSIMDError, kISRETCException

global kISRTimer, kISRKeyboard, kISRSlavePIC, kISRSerial2, kISRSerial1, kISRParallel2
global kISRFloppy, kISRParallel1, kISRRTC, kISRReserved, kISRNotUsed1, kISRNotUsed2
global kISRMouse, kISRCoprocessor, kISRHDD1, kISRHDD2, kISRETCInterrupt

%macro KSAVECONTEXT 0
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov ax, ds
    push rax
    mov ax, es
    push rax
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

%endmacro

%macro KLOADCONTEXT 0
    pop gs
    pop fs
    pop rax
    mov es, ax
    pop rax
    mov ds, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp

%endmacro

kISRDivideError:
    KSAVECONTEXT

    mov rdi, 0
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRDebug:
    KSAVECONTEXT
    mov rdi, 1
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRNMI:
    KSAVECONTEXT
    
    mov rdi, 2
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRBreakPoint:
    KSAVECONTEXT
    mov rdi, 3
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISROverflow:
    KSAVECONTEXT
    mov rdi, 4
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRBoundRangeExceeded:
    KSAVECONTEXT
    mov rdi, 5
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRInvalidOpcode:
    KSAVECONTEXT
    mov rdi, 6
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRDeviceNotAvailable:
    KSAVECONTEXT
    mov rdi, 7
    call kDeviceNotAvailableHandler
    KLOADCONTEXT
    iretq

kISRDoubleFault:
    KSAVECONTEXT

    mov rdi, 8
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler

    KLOADCONTEXT
    add rsp, 8
    iretq

kISRCoprocessorSegmentOverrun:
    KSAVECONTEXT
    mov rdi, 9
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRInvalidTSS:
    KSAVECONTEXT
    mov rdi, 10
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT
    add rsp, 8
    iretq

kISRSegmentNotPresent:
    KSAVECONTEXT
    mov rdi, 11
    mov rsi, [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT
    add rsp, 8
    iretq

kISRStackSegmentFault:
    KSAVECONTEXT
    mov rdi, 12
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT
    add rsp, 8
    iretq

kISRGeneralProtection:
    KSAVECONTEXT
    mov rdi, 13
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT
    add rsp, 8
    iretq

kISRPageFault:
    KSAVECONTEXT
    mov rdi, 14
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT
    add rsp, 8
    iretq

kISR15:
    KSAVECONTEXT
    mov rdi, 15
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRFPUError:
    KSAVECONTEXT
    mov rdi, 16
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRAlignmentCheck:
    KSAVECONTEXT
    mov rdi, 17
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT
    add rsp, 8
    iretq

kISRMachineCheck:
    KSAVECONTEXT
    mov rdi, 18
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRSIMDError:
    KSAVECONTEXT
    mov rdi, 19
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRETCException:
    KSAVECONTEXT
    mov rdi, 20
    call kCommonExceptionHandler
    KLOADCONTEXT
    iretq

kISRTimer:
    KSAVECONTEXT
    
    mov rdi, 32
    call kTimerHandler

    KLOADCONTEXT
    iretq
    

kISRKeyboard:
    KSAVECONTEXT
    mov rdi, 33
    call kKeyboardHandler
    KLOADCONTEXT
    iretq

kISRSlavePIC:
    KSAVECONTEXT
    mov rdi, 34
    call kCommonInterruptHandler
    KLOADCONTEXT
    iretq

kISRSerial2:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 35
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #36, 시리얼 포트 1 ISR
kISRSerial1:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 36
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #37, 패러렐 포트 2 ISR
kISRParallel2:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 37
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #38, 플로피 디스크 컨트롤러 ISR
kISRFloppy:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 38
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #39, 패러렐 포트 1 ISR
kISRParallel1:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 39
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #40, RTC ISR
kISRRTC:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 40
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #41, 예약된 인터럽트의 ISR
kISRReserved:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 41
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #42, 사용 안함
kISRNotUsed1:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 42
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #43, 사용 안함
kISRNotUsed2:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 43
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #44, 마우스 ISR
kISRMouse:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 44
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #45, 코프로세서 ISR
kISRCoprocessor:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 45
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #46, 하드 디스크 1 ISR
kISRHDD1:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 46
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #47, 하드 디스크 2 ISR
kISRHDD2:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 47
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #48 이외의 모든 인터럽트에 대한 ISR
kISRETCInterrupt:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 48
    call kCommonInterruptHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원    

    




