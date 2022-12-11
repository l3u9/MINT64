#include "AssemblyUtility.h"
#include "InterruptHandler.h"
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
#include "IOAPIC.h"
#include "VBE.h"
#include "2DGraphics.h"
#include "MPConfigurationTable.h"
#include "Mouse.h"


#define MOUSE_CURSOR_WIDTH              20
#define MOUSE_CURSOR_HEIGHT             20
#define MOUSE_CURSOR_OUTERLINE          RGB(0, 0, 0)
#define MOUSE_CURSOR_OUTER              RGB(79, 204, 11)
#define MOUSE_CURSOR_INNER              RGB(232, 255, 232)

BYTE gs_vwMouseBuffer[MOUSE_CURSOR_WIDTH * MOUSE_CURSOR_HEIGHT] = {
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1,
    0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1,
    0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 0, 0,
    0, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 3, 3, 2, 1, 1, 2, 3, 2, 2, 2, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 3, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0, 0,
    0, 0, 0, 1, 2, 3, 2, 1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
    0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 0,
    0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1,
    0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
};

void MainForApplicationProcessor(void);
BOOL kChangeToMultiCoreMode(void);
void kStartGraphicModeTest();


void Main(void)
{
    int iCursorX, iCursorY;
    BYTE bButton;
    int iX;
    int iY;


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

    kPrintf("Mouse Activate And Queue Initialize.........[    ]");
    if(kInitializeMouse() == TRUE)
    {
        kEnableMouseInterrupt();
        kSetCursor(45, iCursorY++);
        kPrintf("Pass\n");
    }
    else
    {
        kSetCursor(45, iCursorY++);
        kPrintf("Fail\n");
        while(1);
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

    kPrintf("Change To MultiCore Processor Mode..........[    ]");
    if(kChangeToMultiCoreMode() == TRUE)
    {
        kSetCursor(45, iCursorY++);
        kPrintf("Pass\n");
    }
    else
    {
        kSetCursor(45, iCursorY++);
        kPrintf("Fail\n");
    }
    
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
    
//     kPrintf("Application Processor[APIC ID: %d] Is Activated\n\n",
//             kGetAPICID());
    
    kIdleTask();
    
}

BOOL kChangeToMultiCoreMode(void)
{
        MPCONFIGURATIONMANAGER* pstMPManager;
        BOOL bInterruptFlag;
        int i;

        if(kStartUpApplicationProcessor() == FALSE)
                return FALSE;
        
        pstMPManager = kGetMPConfigurationManager();
        if(pstMPManager->bUsePICMode == TRUE)
        {
                kOutPortByte(0x22, 0x70);
                kOutPortByte(0x23, 0x01);
        }
        kMaskPICInterrupt(0xffff);

        kEnableGlobalLocalAPIC();

        kEnableSoftwareLocalAPIC();

        bInterruptFlag = kSetInterruptFlag(FALSE);

        kSetTaskPriority(0);

        kInitializeLocalVectorTable();

        kSetSymmetricIOMode(TRUE);
        
        kInitializeIORedirectionTable();

        kSetInterruptFlag(bInterruptFlag);
        
        kSetInterruptLoadBalancing(TRUE);

        for(i = 0; i < MAXPROCESSORCOUNT; i++)
                kSetTaskLoadBalancing(i, TRUE);

        return TRUE;

        
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

void kDrawWindowFrame( int iX, int iY, int iWidth, int iHeight, const char* pcTitle )
{
    char* pcTestString1 = "This is MINT64 OS's window prototype~!!!";
    char* pcTestString2 = "Coming soon~!!!";
    VBEMODEINFOBLOCK* pstVBEMode;
    COLOR* pstVideoMemory;
    RECT stScreenArea;

    pstVBEMode = kGetVBEModeInfoBlock();
    
    stScreenArea.iX1 = 0;
    stScreenArea.iY1 = 0;
    stScreenArea.iX2 = pstVBEMode->wXResolution - 1;
    stScreenArea.iY2 = pstVBEMode->wYResolution - 1;
    
    pstVideoMemory = ( COLOR* ) ( ( QWORD )pstVBEMode->dwPhysicalBasePointer & 0xFFFFFFFF );
    
    // 윈도우 프레임의 가장자리를 그림, 2 픽셀 두께
    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX, iY, iX + iWidth, iY + iHeight, RGB( 109, 218, 22 ), FALSE );
    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX + 1, iY + 1, iX + iWidth - 1, iY + iHeight - 1, RGB( 109, 218, 22 ),
            FALSE );

    // 제목 표시줄을 채움
    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX, iY + 3, iX + iWidth - 1, iY + 21, RGB( 79, 204, 11 ), TRUE );

    // 윈도우 제목을 표시
    kInternalDrawText( &stScreenArea, pstVideoMemory, 
            iX + 6, iY + 3, RGB( 255, 255, 255 ), RGB( 79, 204, 11 ),
            pcTitle, kStrLen( pcTitle ) );
    
    // 제목 표시줄을 입체로 보이게 위쪽의 선을 그림, 2 픽셀 두께
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + 1, iY + 1, iX + iWidth - 1, iY + 1, RGB( 183, 249, 171 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + 1, iY + 2, iX + iWidth - 1, iY + 2, RGB( 150, 210, 140 ) );

    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + 1, iY + 2, iX + 1, iY + 20, RGB( 183, 249, 171 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + 2, iY + 2, iX + 2, iY + 20, RGB( 150, 210, 140 ) );
    
    // 제목 표시줄의 아래쪽에 선을 그림
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + 2, iY + 19, iX + iWidth - 2, iY + 19, RGB( 46, 59, 30 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + 2, iY + 20, iX + iWidth - 2, iY + 20, RGB( 46, 59, 30 ) );

    // 닫기 버튼을 그림, 오른쪽 상단에 표시
    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18, iY + 1, iX + iWidth - 2, iY + 19,
            RGB( 255, 255, 255 ), TRUE );

    // 닫기 버튼을 입체로 보이게 선을 그림, 2 픽셀 두께로 그림
    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2, iY + 1, iX + iWidth - 2, iY + 19 - 1,
            RGB( 86, 86, 86 ), TRUE );
    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 1, iY + 1, iX + iWidth - 2 - 1, iY + 19 - 1,
            RGB( 86, 86, 86 ), TRUE );
    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 1, iY + 19, iX + iWidth - 2, iY + 19,
            RGB( 86, 86, 86 ), TRUE );
    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 1, iY + 19 - 1, iX + iWidth - 2, iY + 19 - 1,
            RGB( 86, 86, 86 ), TRUE );

    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18, iY + 1, iX + iWidth - 2 - 1, iY + 1,
            RGB( 229, 229, 229 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18, iY + 1 + 1, iX + iWidth - 2 - 2, iY + 1 + 1,
            RGB( 229, 229, 229 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18, iY + 1, iX + iWidth - 2 - 18, iY + 19,
            RGB( 229, 229, 229 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 1, iY + 1, iX + iWidth - 2 - 18 + 1, iY + 19 - 1,
            RGB( 229, 229, 229 ) );
    
    // 대각선 X를 그림, 3 픽셀로 그림
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 4, iY + 1 + 4, iX + iWidth - 2 - 4, iY + 19 - 4, 
            RGB( 71, 199, 21 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 5, iY + 1 + 4, iX + iWidth - 2 - 4, iY + 19 - 5, 
            RGB( 71, 199, 21 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 4, iY + 1 + 5, iX + iWidth - 2 - 5, iY + 19 - 4, 
            RGB( 71, 199, 21 ) );
    
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 4, iY + 19 - 4, iX + iWidth - 2 - 4, iY + 1 + 4, 
            RGB( 71, 199, 21 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 5, iY + 19 - 4, iX + iWidth - 2 - 4, iY + 1 + 5, 
            RGB( 71, 199, 21 ) );
    kInternalDrawLine( &stScreenArea, pstVideoMemory, 
            iX + iWidth - 2 - 18 + 4, iY + 19 - 5, iX + iWidth - 2 - 5, iY + 1 + 4, 
            RGB( 71, 199, 21 ) );



    kInternalDrawRect( &stScreenArea, pstVideoMemory, 
            iX + 2, iY + 21, iX + iWidth - 2, iY + iHeight - 2, 
            RGB( 255, 255, 255 ), TRUE );
    
    // 테스트 문자 출력
    kInternalDrawText( &stScreenArea, pstVideoMemory, 
            iX + 10, iY + 30, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ), pcTestString1,
            kStrLen( pcTestString1 ) );
    kInternalDrawText( &stScreenArea, pstVideoMemory, 
            iX + 10, iY + 50, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ), pcTestString2,
            kStrLen( pcTestString2 ) );
}


void kDrawCursor(RECT* pstArea, COLOR* pstVideoMemory, int iX, int iY)
{
	int i, j;
	BYTE* pbCurrentPos;

	pbCurrentPos = gs_vwMouseBuffer;

	for(j = 0; j < MOUSE_CURSOR_HEIGHT; j++)
	{
		for(i = 0; i < MOUSE_CURSOR_WIDTH; i++)
		{
			switch(*pbCurrentPos)
			{
				case 0:
					break;
				case 1:
					kInternalDrawPixel(pstArea, pstVideoMemory, i+ iX , j + iY, MOUSE_CURSOR_OUTERLINE);
					break;
				case 2:
					kInternalDrawPixel(pstArea, pstVideoMemory, i+ iX , j + iY, MOUSE_CURSOR_OUTER);
					break;
				case 3:
					kInternalDrawPixel(pstArea, pstVideoMemory, i+ iX , j + iY, MOUSE_CURSOR_INNER);
					break;
			}
			pbCurrentPos++;
		}
	}
}

void kStartGraphicModeTest()
{
	VBEMODEINFOBLOCK* pstVBEMode;
	int iX, iY;
	COLOR* pstVideoMemory;
	RECT stScreenArea;
	int iRelativeX, iRelativeY;
	BYTE bButton;

	pstVBEMode = kGetVBEModeInfoBlock();

	stScreenArea.iX1 = 0;
	stScreenArea.iY1 = 0;
	stScreenArea.iX2 = pstVBEMode->wXResolution - 1;
	stScreenArea.iY2 = pstVBEMode->wYResolution - 1;

	pstVideoMemory = (COLOR*) pstVBEMode->dwPhysicalBasePointer;

	iX = pstVBEMode->wXResolution / 2;
	iY = pstVBEMode->wYResolution / 2;

	kInternalDrawRect(&stScreenArea, pstVideoMemory, stScreenArea.iX1, stScreenArea.iY1, 
					stScreenArea.iX2, stScreenArea.iY2, RGB(232, 255, 232), TRUE);

	kDrawCursor(&stScreenArea, pstVideoMemory, iX, iY);

	while(1)
	{
		if(kGetMouseDataFromMouseQueue(&bButton, &iRelativeX, &iRelativeY) == FALSE)
		{
			kSleep(0);
			continue;
		}
		kInternalDrawRect(&stScreenArea, pstVideoMemory, iX, iY, iX + MOUSE_CURSOR_WIDTH, iY + MOUSE_CURSOR_HEIGHT, RGB(232, 255, 232), TRUE);

		iX += iRelativeX;
		iY += iRelativeY;

		if(iX < stScreenArea.iX1)
			iX = stScreenArea.iX1;
		else if(iX > stScreenArea.iX2)
			iX = stScreenArea.iX2;
		
		if(iY < stScreenArea.iY1)
			iY = stScreenArea.iY1;
		else if(iY > stScreenArea.iY2)
			iY = stScreenArea.iY2;
	
		if(bButton & MOUSE_LBUTTONDOWN)
			kDrawWindowFrame(iX - 200 , iY - 100 , 400, 200, "MINT64 OS Test Window");
		else if(bButton & MOUSE_RBUTTONDOWN)
		{
			kInternalDrawRect(&stScreenArea, pstVideoMemory, stScreenArea.iX1, stScreenArea.iY1, 
						stScreenArea.iX2, stScreenArea.iY2, RGB(232, 255, 232), TRUE);
		}
		kDrawCursor(&stScreenArea, pstVideoMemory, iX, iY);
	}

}

