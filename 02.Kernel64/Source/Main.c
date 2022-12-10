
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
#include "2DGraphics.h"

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

#define ABS(x)      (((x) >= 0) ? (x) : -(x))

void kGetRandomXY(int* piX, int* piY)
{
    int iRandom;

    iRandom = kRandom();
    *piX = ABS(iRandom) % 1000;

    iRandom = kRandom();
    *piY = ABS(iRandom) % 700;
}

COLOR kGetRandomColor(void)
{
    int iR, iG, iB;
    int iRandom;

    iRandom = kRandom();
    iR = ABS(iRandom) % 256;

    iRandom = kRandom();
    iG = ABS(iRandom) % 256;

    iRandom = kRandom();
    iB = ABS(iRandom) % 256;

    return RGB(iR, iG, iB);
}

void kDrawWindowFrame(int iX, int iY, int iWidth, int iHeight, const char* pcTitle)
{
    char* pcTestString1 = "This is MINT64 OS's window prototype~!!!";
    char* pcTestString2 = "Coming soon~!!!";
    
    //윈도우 프레임의 가장자리 2픽셀 두께
    kDrawRect(iX, iY, iX + iWidth, iY + iHeight, RGB(109, 218, 22), FALSE);
    kDrawRect(iX + 1, iY + 1, iX + iWidth - 1, iY + iHeight - 1, RGB(109, 218, 22), FALSE);

    //제목 표시줄
    kDrawRect(iX, iY + 3, iX + iWidth - 1, iY + 21, RGB(79, 204, 11), TRUE);
    kDrawText(iX + 6, iY + 3, RGB(255, 255, 255), RGB(79, 204, 11), pcTitle, kStrLen(pcTitle));

    //윈도우 제목 표시
    kDrawLine(iX + 1, iY + 1, iX + iWidth - 1, iY + 1, RGB(183, 249, 171));
    kDrawLine(iX + 1, iY + 2, iX + iWidth - 1, iY + 2, RGB(150, 210, 140));

    kDrawLine(iX + 1, iY + 2, iX + 1, iY + 20, RGB(183, 249, 171));
    kDrawLine(iX + 2, iY + 2, iX + 1, iY + 20, RGB(150, 210, 140));

    //제목표시줄의 아래쪽에 선을 그림
    kDrawLine(iX + 2, iY + 19, iX + iWidth - 2, iY + 19, RGB(46, 49, 30));
    kDrawLine(iX + 2, iY + 20, iX + iWidth - 2, iY + 20, RGB(46, 49, 30));

    //닫기 버튼 그림 
    kDrawRect(iX + iWidth - 2 - 18, iY + 1, iX + iWidth - 2, iY + 19, RGB(255, 255, 255), TRUE);

    kDrawRect( iX + iWidth - 2, iY + 1, iX + iWidth - 2, iY + 19 - 1, RGB( 86, 86, 86 ), TRUE );
    kDrawRect( iX + iWidth - 2 - 1, iY + 1, iX + iWidth - 2 - 1, iY + 19 - 1, RGB( 86, 86, 86 ), TRUE );
    kDrawRect( iX + iWidth - 2 - 18 + 1, iY + 19, iX + iWidth - 2, iY + 19, RGB( 86, 86, 86 ), TRUE );
    kDrawRect( iX + iWidth - 2 - 18 + 1, iY + 19 - 1, iX + iWidth - 2, iY + 19 - 1, RGB( 86, 86, 86 ), TRUE );

    // 대각선 X를 그림, 3 픽셀로 그림
    kDrawLine( iX + iWidth - 2 - 18 + 4, iY + 1 + 4, iX + iWidth - 2 - 4, iY + 19 - 4, 
            RGB( 71, 199, 21 ) );
    kDrawLine( iX + iWidth - 2 - 18 + 5, iY + 1 + 4, iX + iWidth - 2 - 4, iY + 19 - 5, 
            RGB( 71, 199, 21 ) );
    kDrawLine( iX + iWidth - 2 - 18 + 4, iY + 1 + 5, iX + iWidth - 2 - 5, iY + 19 - 4, 
            RGB( 71, 199, 21 ) );
    
    kDrawLine( iX + iWidth - 2 - 18 + 4, iY + 19 - 4, iX + iWidth - 2 - 4, iY + 1 + 4, 
            RGB( 71, 199, 21 ) );
    kDrawLine( iX + iWidth - 2 - 18 + 5, iY + 19 - 4, iX + iWidth - 2 - 4, iY + 1 + 5, 
            RGB( 71, 199, 21 ) );
    kDrawLine( iX + iWidth - 2 - 18 + 4, iY + 19 - 5, iX + iWidth - 2 - 5, iY + 1 + 4, 
            RGB( 71, 199, 21 ) );


    // 내부를 그림
    kDrawRect( iX + 2, iY + 21, iX + iWidth - 2, iY + iHeight - 2, 
            RGB( 255, 255, 255 ), TRUE );
    
    // 테스트 문자 출력
    kDrawText( iX + 10, iY + 30, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ), pcTestString1,
            kStrLen( pcTestString1 ) );
    kDrawText( iX + 10, iY + 50, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ), pcTestString2,
            kStrLen( pcTestString2 ) );
}

void kStartGraphicModeTest()
{
    VBEMODEINFOBLOCK* pstVBEMode;
    int iX1, iY1, iX2, iY2;
    COLOR stColor1, stColor2;
    int i;
    char* vpcString[] = {"Pixel", "Line", "Rectangle", "Circle", "MINT64 OS ~!!!"};

    kDrawText(0, 0, RGB(255, 255, 255), RGB(0, 0, 0), vpcString[0], kStrLen(vpcString[0]));
    kDrawPixel(1, 20, RGB(255, 255, 255));
    kDrawPixel(2, 20, RGB(255, 255, 255));

    kDrawText(0, 25, RGB(255, 0, 0), RGB(0, 0, 0), vpcString[1], kStrLen(vpcString[1]));

    kDrawLine(20, 50, 1000, 50, RGB(255, 0, 0));
    kDrawLine(20, 50, 1000, 100, RGB(255, 0, 0));
    kDrawLine(20, 50, 1000, 150, RGB(255, 0, 0));
    kDrawLine(20, 50, 1000, 200, RGB(255, 0, 0));
    kDrawLine(20, 50, 1000, 250, RGB(255, 0, 0));
    

    kDrawText(0, 180, RGB(0, 255, 0), RGB(0, 0, 0), vpcString[2], kStrLen(vpcString[2]));
    kDrawRect(20, 200, 70, 250, RGB(0, 255, 0), FALSE);
    kDrawRect(120, 200, 220, 300, RGB(0, 255, 0), TRUE);
    kDrawRect(270, 200, 420, 350, RGB(0, 255, 0), FALSE);
    kDrawRect(470, 200, 670, 400, RGB(0, 255, 0), TRUE);

    kDrawText(0, 550, RGB(0, 0, 255), RGB(0, 0, 0), vpcString[3], kStrLen(vpcString[3]));
    kDrawCircle(45, 600, 25, RGB(0, 0, 255), FALSE);
    kDrawCircle(170, 600, 50, RGB(0, 0, 255), TRUE);
    kDrawCircle(345, 600, 75, RGB(0, 0, 255), FALSE);
    kDrawCircle(570, 600, 100, RGB(0, 0, 255), TRUE);

    kGetCh();

    do
    {
        for(i = 0; i < 100; i++)
        {
            kGetRandomXY(&iX1, &iY1);
            stColor1 = kGetRandomColor();

            kDrawPixel(iX1, iY1, stColor1);
        }

        for(i = 0; i < 100; i++)
        {
            kGetRandomXY(&iX1, &iY1);
            kGetRandomXY(&iX2, &iY2);

            kDrawLine(iX1, iY1, iX2, iY2, stColor1);
        }

        for(i = 0; i < 20; i++)
        {
            kGetRandomXY(&iX1, &iY1);
            kGetRandomXY(&iX2, &iY2);
            stColor1 = kGetRandomColor();

            kDrawRect(iX1, iY1, iX2, iY2, stColor1, kRandom() % 2);
        }

        for(i = 0; i < 100; i++)
        {
            kGetRandomXY(&iX1, &iY1);
            stColor1 = kGetRandomColor();

            kDrawCircle(iX1, iY1, ABS(kRandom() % 50 + 1), stColor1, kRandom() % 2);
        }

        for(i = 0; i < 100; i++)
        {
            kGetRandomXY(&iX1, &iY1);
            stColor1 = kGetRandomColor();
            stColor2 = kGetRandomColor();

            kDrawText(iX1, iY1, stColor1, stColor2, vpcString[4], kStrLen(vpcString[4]));
        }
    }while(kGetCh() != 'q');

    while(1)
    {
        kDrawRect(0, 0, 1024, 768, RGB(232, 255, 232), TRUE);
        for(i = 0; i < 3; i++)
        {
            kGetRandomXY(&iX1, &iY1);
            kDrawWindowFrame(iX1, iY1, 400, 200, "MINT64 OS Test Window");
        }
        kGetCh();
    }

}

// void kStartGraphicModeTest()
// {
//     VBEMODEINFOBLOCK* pstVBEMode;
//     WORD* pwFrameBufferAddress;
//     WORD wColor = 0;
//     int iBandHeight;
//     int i;
//     int j;

//     kGetCh();
    
//     pstVBEMode = kGetVBEModeInfoBlock();
//     pwFrameBufferAddress = (WORD*)((QWORD) pstVBEMode->dwPhysicalBasePointer);

//     iBandHeight = pstVBEMode->wYResolution / 32;

//     while(1)
//     {
//         for(j = 0; j < pstVBEMode->wYResolution; j++)
//         {
//             for(i = 0; i < pstVBEMode->wXResolution; i++)
//             {
//                 pwFrameBufferAddress[(j * pstVBEMode->wXResolution) + i] = wColor;
//             }

//             if((j % iBandHeight) == 0)
//                 wColor = kRandom() & 0xffff;
//         }
//         kGetCh();
//     }

// }
