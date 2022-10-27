


#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "AssemblyUtility.h"


// 커맨드 테이블 정의
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", kHelp },
        { "cls", "Clear Screen", kCls },
        { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
        { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
        { "shutdown", "Shutdown And Reboot OS", kShutdown },
        { "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)", kSetTimer},
        { "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT},
        { "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter},
        { "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed},
        { "date", "Show Date And Time", kShowDateAndTime},
        { "createtask", "Create Task, ex)createtask 1(type) 10(count)", kCreateTestTask},
};

//==============================================================================
//  실제 셸을 구성하는 코드
//==============================================================================

void kStartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;

    kPrintf( CONSOLESHELL_PROMPTMESSAGE );

    while( 1 )
    {
        bKey = kGetCh();
        if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {

                kGetCursor( &iCursorX, &iCursorY );
                kPrintStringXY( iCursorX - 1, iCursorY, " " );
                kSetCursor( iCursorX - 1, iCursorY );
                iCommandBufferIndex--;
            }
        }
        else if( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );

            if( iCommandBufferIndex > 0 )
            {
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                kExecuteCommand( vcCommandBuffer );
            }

            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else
        {
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }

            // 버퍼에 공간이 남아있을 때만 가능
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}


void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;

    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }

    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( ( iCommandLength == iSpaceIndex ) &&
            ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    if( i >= iCount )
    {
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
    }
}


void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }

    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }

    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}

//==============================================================================
//  커맨드를 처리하는 코드
//==============================================================================

void kHelp( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;


    kPrintf( "=========================================================\n" );
    kPrintf( "                    MINT64 Shell Help                    \n" );
    kPrintf( "=========================================================\n" );

    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }

    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );
    }
}


void kCls( const char* pcParameterBuffer )
{
    kClearScreen();
    kSetCursor( 0, 1 );
}


void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}


void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;

    // 파라미터 초기화
    kInitializeParameter( &stList, pcParameterBuffer );

    while( 1 )
    {

        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }


        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1,
                 vcParameter, iLength );

        if( kMemCmp( vcParameter, "0x", 2 ) == 0 )
        {
            lValue = kAToI( vcParameter + 2, 16 );
            kPrintf( "HEX Value = %q\n", lValue );
        }
        else
        {
            lValue = kAToI( vcParameter, 10 );
            kPrintf( "Decimal Value = %d\n", lValue );
        }

        iCount++;
    }
}


void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );

    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();
}

void kSetTimer(const char* pcParameterBuffer)
{
    char vcParameter[100];
    PARAMETERLIST stList;
    long lValue;
    BOOL bPeriodic;

    kInitializeParameter(&stList, pcParameterBuffer);

    //extract milisecond
    if(kGetNextParameter(&stList, vcParameter) == 0)
    {
        kPrintf("ex)settimer 10(ms) 1(periodic)\n");
        return;
    }
    lValue = kAToI(vcParameter, 10);

    //extract periodic 
    if(kGetNextParameter(&stList, vcParameter) == 0)
    {
        kPrintf("ex)settimer 10(ms) 1(periodic)\n");
        return;
    }
    bPeriodic = kAToI(vcParameter, 10);

    kInitializePIT(MSTOCOUNT(lValue), bPeriodic);
    kPrintf("Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic);
}


void kWaitUsingPIT(const char* pcParameterBuffer)
{
    char vcParameter[100];
    int iLength;
    PARAMETERLIST stList;
    long lMillisecond;
    int i;

    kInitializeParameter(&stList, pcParameterBuffer);
    if(kGetNextParameter(&stList, vcParameter) == 0)
    {
        kPrintf("ex)wait 100(ms)\n");
        return;
    }
    lMillisecond = kAToI(pcParameterBuffer, 10);
    kPrintf("%d ms Sleep Start...\n", lMillisecond);

    kDisableInterrupt();
    for(i = 0; i < lMillisecond / 30; i++)
    {
        kWaitUsingPIT(MSTOCOUNT(30));
    }
    kWaitUsingPIT(MSTOCOUNT(lMillisecond % 30));
    kEnableInterrupt();
    kPrintf("%d ms Sleep Complete\n", lMillisecond);

    kInitializePIT(MSTOCOUNT(1), TRUE);
}

void kReadTimeStampCounter(const char* pcParameterBuffer)
{
    QWORD qwTSC;
    qwTSC = kReadTSC();
    kPrintf("Time Stamp Counter = %q\n", qwTSC);
}

void kMeasureProcessorSpeed(const char* pcParameterBuffer)
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;

    kPrintf("Now Measuring.");

    kDisableInterrupt();
    for(i = 0; i < 200; i++)
    {
        qwLastTSC = kReadTSC();
        kWaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += kReadTSC() - qwLastTSC;

        kPrintf(".");
    }

    kInitializePIT(MSTOCOUNT(1), TRUE);
    kEnableInterrupt();

    kPrintf("\nCPU Speed = %d MHZ\n", qwTotalTSC / 10 / 1000/ 1000);
}

void kShowDateAndTime(const char* pcParameterBuffer)
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    kReadRTCTime(&bHour, &bMinute, &bSecond);
    kReadRTCDate(&wYear, &bMonth, &bDayOfMonth, &bDayOfWeek);

    kPrintf("Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth, kConvertDayOfWeekToString(bDayOfWeek));
    kPrintf("Time: %d:%d:%d\n", bHour, bMinute, bSecond);
}

static TCB gs_vstTask[2] = {0,};
static QWORD gs_vstStack[1024] = {0,};

void kTestTask(void)
{
    int i =0;

    while(1)
    {
        kPrintf("[%d] This message is from kTestTask. Press any key to switch kConsoleShell~!!\n", i++);

        kGetCh();

        kSwitchContext( &( gs_vstTask[ 1 ].stContext ), &( gs_vstTask[ 0 ].stContext ) );
    }
}

// void kCreateTestTask(const char* pcParameterBuffer)
// {
//     KEYDATA stData;
//     int i =0;

//     kSetUpTask(&(gs_vstTask[1]), 1, 0, (QWORD)kTestTask, &(gs_vstStack), sizeof(gs_vstStack));

//     while(1)
//     {
//         kPrintf("[%d] This message is from kConsoleShell. Press any key to switch TestTask~!!\n", i++);

//         if(kGetCh() == 'q')
//         {
//             break;
//         }
//         kSwitchContext( &( gs_vstTask[ 0 ].stContext ), &( gs_vstTask[ 1 ].stContext ) );    
//     }
// }

// void kTestTask1(void)
// {
//     BYTE bData;
//     int i = 0, iX = 0, iY = 0, iMargin;
//     CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;
//     TCB* pstRunningTask;

//     pstRunningTask = kGetRunningTask();
//     iMargin = (pstRunningTask->stLink.qwID & 0xffffffff) % 10;

//     while(1)
//     {
//         switch( i )
//         {
//         case 0:
//             iX++;
//             if( iX >= ( CONSOLE_WIDTH - iMargin ) )
//             {
//                 i = 1;
//             }
//             break;

//         case 1:
//             iY++;
//             if( iY >= ( CONSOLE_HEIGHT - iMargin ) )
//             {
//                 i = 2;
//             }
//             break;

//         case 2:
//             iX--;
//             if( iX < iMargin )
//             {
//                 i = 3;
//             }
//             break;

//         case 3:
//             iY--;
//             if( iY < iMargin )
//             {
//                 i = 0;
//             }
//             break;
//         }

//         pstScreen[iY * CONSOLE_WIDTH + iX].bCharactor = bData;
//         pstScreen[iY * CONSOLE_WIDTH + iX].bAttribute = bData & 0xf;
//         bData++;

//         kSchedule();
//     }
// }

// void kTestTask2(void)
// {
//     int i = 0, iOffset;
//     CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;
//     TCB* pstRunningTask;
//     char vcData[4] = {'-', '\\', '|', '/'};

//     pstRunningTask = kGetRunningTask();
//     iOffset = (pstRunningTask->stLink.qwID & 0xffffffff) * 2;
//     iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - (iOffset % (CONSOLE_WIDTH * CONSOLE_HEIGHT));

//     while(1)
//     {
//         pstScreen[iOffset].bCharactor = vcData[i%4];

//         pstScreen[iOffset].bAttribute = (iOffset % 15) + 1;
//         i++;

//         kSchedule();
//     }
// }

void kTestTask1( void )
{
    BYTE bData;
    int i = 0, iX = 0, iY = 0, iMargin;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;

    // 자신의 ID를 얻어서 화면 오프셋으로 사용
    pstRunningTask = kGetRunningTask();
    iMargin = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) % 10;

    // 화면 네 귀퉁이를 돌면서 문자 출력
    while( 1 )
    {
        switch( i )
        {
        case 0:
            iX++;
            if( iX >= ( CONSOLE_WIDTH - iMargin ) )
            {
                i = 1;
            }
            break;

        case 1:
            iY++;
            if( iY >= ( CONSOLE_HEIGHT - iMargin ) )
            {
                i = 2;
            }
            break;

        case 2:
            iX--;
            if( iX < iMargin )
            {
                i = 3;
            }
            break;

        case 3:
            iY--;
            if( iY < iMargin )
            {
                i = 0;
            }
            break;
        }

        // 문자 및 색깔 지정
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bCharactor = bData;
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bAttribute = bData & 0x0F;
        bData++;

        // 다른 태스크로 전환
        kSchedule();
    }
}

/**
 *  태스크 2
 *      자신의 ID를 참고하여 특정 위치에 회전하는 바람개비를 출력
 */
void kTestTask2( void )
{
    int i = 0, iOffset;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[ 4 ] = { '-', '\\', '|', '/' };

    // 자신의 ID를 얻어서 화면 오프셋으로 사용
    pstRunningTask = kGetRunningTask();
    iOffset = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT -
        ( iOffset % ( CONSOLE_WIDTH * CONSOLE_HEIGHT ) );

    while( 1 )
    {
        // 회전하는 바람개비를 표시
        pstScreen[ iOffset ].bCharactor = vcData[ i % 4 ];
        // 색깔 지정
        pstScreen[ iOffset ].bAttribute = ( iOffset % 15 ) + 1;
        i++;

        // 다른 태스크로 전환
        kSchedule();
    }
}


void kCreateTestTask(const char* pcParameterBuffer)
{
    PARAMETERLIST stList;
    char vcType[30];
    char vcCount[30];
    int i;

    kInitializeParameter(&stList, pcParameterBuffer);
    kGetNextParameter(&stList, vcType);
    kGetNextParameter(&stList, vcCount);

    switch (kAToI(vcType, 10)) 
    {
        case 1:
            for(i = 0; i < kAToI(vcCount, 10); i++)
            {
                if(kCreateTask(0, (QWORD)kTestTask1) == NULL)
                {
                    break;
                }
            }
            kPrintf("Task1 %d Created\n", i);
            break;
        case 2:
        default:
            for(i = 0; i < kAToI(vcCount, 10); i++)
            {
                if(kCreateTask(0, (QWORD) kTestTask2) == NULL)
                {
                    break;
                }
            }
            kPrintf("Task2 %d Created\n", i);
            break;
    }
}