
#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Task.h"
#include "PIT.h"
#include "DynamicMemory.h"
#include "HardDisk.h"
#include "FileSystem.h"
#include "SerialPort.h"
#include "MultiProcessor.h"
#include "VBE.h"

void MainForApplicationProcessor(void);
void kStartGraphicModeTest();

void Main(void)
{
    int iCursorX, iCursorY;

    if(*((BYTE*) BOOTSTRAPPROCESSOR_FLAGADDRESS) == 0)
        MainForApplicationProcessor();

    *((BYTE*)BOOTSTRAPPROCESSOR_FLAGADDRESS) = 0;

    kInitializeConsole(0, 10);
    kPrintf("Switch To IA-32e Mode Success~!!\n");
    kPrintf("IA-32e C Language Kernel Start..............[Pass]\n");
    kPrintf("Initialize Console..........................[Pass]\n");

    kGetCursor(&iCursorX, &iCursorY);
    kPrintf("GDT Initialize And Switch For IA-32e Mode...[    ]");
    kInitializeGDTTableAndTSS();
    kLoadGDTR(GDTR_STARTADDRESS);
    kSetCursor(45, iCursorY++);
    kPrintf("Pass\n");

    kPrintf("TSS Segment Load............................[    ]");
    kLoadTR(GDT_TSSSEGMENT);
    kSetCursor(45, iCursorY++);
    kPrintf("Pass\n");

    kPrintf("IDT Initialize..............................[    ]");
    kInitializeIDTTables();
    kLoadIDTR(IDTR_STARTADDRESS);
    kSetCursor(45, iCursorY++);
    kPrintf("Pass\n");

    kPrintf("Total RAM Size Check........................[    ]");
    kCheckTotalRAMSize();
    kSetCursor(45, iCursorY++);
    kPrintf("Pass], Size = %d MB\n", kGetTotalRAMSize());

    kPrintf("TCB Pool And Scheduler Initialize...........[Pass]\n");
    iCursorY++;
    kInitializeScheduler();

    kPrintf("Dynamic Memory Initialize...................[Pass]\n");
    iCursorY++;
    kInitializeDynamicMemory();

    kInitializePIT(MSTOCOUNT(1), 1);

    kPrintf("Keyboard Activate And Queue Initialize......[    ]");

    if(kInitializeKeyboard() == TRUE)
    {
        kSetCursor(45, iCursorY++);
        kPrintf("Pass\n");
        kChangeKeyboardLED(FALSE, FALSE, FALSE);
    }
    else
    {
        kSetCursor(45, iCursorY++);
        kPrintf("Fail\n");
        while(1) ;
    }

    kPrintf("PIC Controller And Interrupt Initialize.....[    ]");
    // PIC 컨트롤러 초기화 및 모든 인터럽 활성화
    kInitializePIC();
    kMaskPICInterrupt(0);
    kEnableInterrupt();
    kSetCursor(45, iCursorY++);
    kPrintf("Pass\n");

    kPrintf("File System Initialize......................[    ]");
    if(kInitializeFileSystem() == TRUE)
    {
        kSetCursor(45, iCursorY++);
        kPrintf("Pass\n");
    }
    else
    {
        kSetCursor(45, iCursorY++);
        kPrintf("Fail\n");
    }
    

    kPrintf("Serial Port Initialize......................[Pass]\n");
    iCursorY++;
    kInitializeSerialPort();
    
    // 유휴 태스크를 시스템 스레드로 생성하고 쉘을 시작
    kCreateTask(TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM, 0, 0, (QWORD)kIdleTask, kGetAPICID());

    if(*(BYTE*)VBE_STARTGRAPHICMODEFLAGADDRESS == 0)
        kStartConsoleShell();
    else
        kStartGraphicModeTest();
}

void MainForApplicationProcessor(void)
{
    QWORD qwTickCount;

    kLoadGDTR(GDTR_STARTADDRESS);

    kLoadTR(GDT_TSSSEGMENT + (kGetAPICID() * sizeof(GDTENTRY16)));

    kLoadIDTR(IDTR_STARTADDRESS);

    kInitializeScheduler();
    
    kEnableSoftwareLocalAPIC();

    kSetTaskPriority(0);

    kInitializeLocalVectorTable();

    kEnableInterrupt();    
    
    kPrintf("Application Processor[APIC ID: %d] Is Activated\n\n",
            kGetAPICID());
    
    kIdleTask();
    
}

void kStartGraphicModeTest()
{
    VBEMODEINFOBLOCK* pstVBEMode;
    WORD* pwFrameBufferAddress;
    WORD wColor = 0;
    int iBandHeight;
    int i;
    int j;

    kGetCh();
    
    pstVBEMode = kGetVBEModeInfoBlock();
    pwFrameBufferAddress = (WORD*)((QWORD) pstVBEMode->dwPhysicalBasePointer);

    iBandHeight = pstVBEMode->wYResolution / 32;

    while(1)
    {
        for(j = 0; j < pstVBEMode->wYResolution; j++)
        {
            for(i = 0; i < pstVBEMode->wXResolution; i++)
            {
                pwFrameBufferAddress[(j * pstVBEMode->wXResolution) + i] = wColor;
            }

            if((j % iBandHeight) == 0)
                wColor = kRandom() & 0xffff;
        }
        kGetCh();
    }

}
