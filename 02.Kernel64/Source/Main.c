// // #include "Types.h"
// // #include "Keyboard.h"
// // #include "Descriptor.h"
// // #include "PIC.h"


// // void kPrintString( int iX, int iY, const char* pcString);

// // void Main( void )
// // {
// //     int iCursorX, iCursorY;

// //     // �ܼ��� ���� �ʱ�ȭ�� ��, ���� �۾��� ����
// //     kInitializeConsole( 0, 10 );
// //     kPrintf( "Switch To IA-32e Mode Success~!!\n" );
// //     kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
// //     kPrintf( "Initialize Console..........................[Pass]\n" );

// //     // ���� ��Ȳ�� ȭ�鿡 ���
// //     kGetCursor( &iCursorX, &iCursorY );
// //     kPrintf( "GDT Initialize And Switch For IA-32e Mode...[    ]" );
// //     kInitializeGDTTableAndTSS();
// //     kLoadGDTR( GDTR_STARTADDRESS );
// //     kSetCursor( 45, iCursorY++ );
// //     kPrintf( "Pass\n" );

// //     kPrintf( "TSS Segment Load............................[    ]" );
// //     kLoadTR( GDT_TSSSEGMENT );
// //     kSetCursor( 45, iCursorY++ );
// //     kPrintf( "Pass\n" );

// //     kPrintf( "IDT Initialize..............................[    ]" );
// //     kInitializeIDTTables();
// //     kLoadIDTR( IDTR_STARTADDRESS );
// //     kSetCursor( 45, iCursorY++ );
// //     kPrintf( "Pass\n" );

// //     kPrintf( "Total RAM Size Check........................[    ]" );
// //     kCheckTotalRAMSize();
// //     kSetCursor( 45, iCursorY++ );
// //     kPrintf( "Pass], Size = %d MB\n", kGetTotalRAMSize() );

// //     kPrintf( "Keyboard Activate And Queue Initialize......[    ]" );
// //     // Ű���带 Ȱ��ȭ
// //     if( kInitializeKeyboard() == TRUE )
// //     {
// //         kSetCursor( 45, iCursorY++ );
// //         kPrintf( "Pass\n" );
// //         kChangeKeyboardLED( FALSE, FALSE, FALSE );
// //     }
// //     else
// //     {
// //         kSetCursor( 45, iCursorY++ );
// //         kPrintf( "Fail\n" );
// //         while( 1 ) ;
// //     }

// //     kPrintf( "PIC Controller And Interrupt Initialize.....[    ]" );
// //     // PIC ��Ʈ�ѷ� �ʱ�ȭ �� ��� ���ͷ�Ʈ Ȱ��ȭ
// //     kInitializePIC();
// //     kMaskPICInterrupt( 0 );
// //     kEnableInterrupt();
// //     kSetCursor( 45, iCursorY++ );
// //     kPrintf( "Pass\n" );

// //     // ���� ����
// //     kStartConsoleShell();
// // }

// // // void Main(void)
// // // {

// // //     char vcTemp[2] = {0, };
// // //     BYTE bFlags;
// // //     BYTE bTemp;
// // //     int i = 0;
// // //     KEYDATA stData;


// // //     kPrintString( 0, 10, "Switch To IA-32e Mode Success~!!" );
// // //     kPrintString( 0, 11, "IA-32e C Language Kernel Start..............[Pass]" );
    
// // //     kPrintString( 0, 12, "GDT Initialize And Switch For IA-32e Mode...[    ]" );
// // //     kInitializeGDTTableAndTSS();
// // //     kLoadGDTR( GDTR_STARTADDRESS );
// // //     kPrintString( 45, 12, "Pass" );
    
// // //     kPrintString( 0, 13, "TSS Segment Load............................[    ]" );
// // //     kLoadTR( GDT_TSSSEGMENT );
// // //     kPrintString( 45, 13, "Pass" );
    
// // //     kPrintString( 0, 14, "IDT Initialize..............................[    ]" );
// // //     kInitializeIDTTables();    
// // //     kLoadIDTR( IDTR_STARTADDRESS );
// // //     kPrintString( 45, 14, "Pass" );
    
// // //     // kPrintString( 0, 15, "Keyboard Activate...........................[    ]" );
// // //     kPrintString( 0, 15, "Keyboard Activate And Queue Initialize......[    ]" );



// // //     // if(kActivateKeyboard() == TRUE)
// // //     if(kInitializeKeyboard() == TRUE)
// // //     {
// // //         kPrintString(45,15,"Pass");
// // //         kChangeKeyboardLED(FALSE, FALSE, FALSE);
// // //     }
// // //     else
// // //     {
// // //         kPrintString(45, 15, "Fail");
// // //         while(1);
// // //     }

// // //     kPrintString(0, 16, "PIC Controller And Interrupt Initialize.....[    ]");
// // //     kInitializePIC();
// // //     kMaskPICInterrupt(0);
// // //     kEnableInterrupt();
// // //     kPrintString(45, 16, "Pass");
    

// // //     while(1)
// // //     {
// // //         if(kGetKeyFromKeyQueue(&stData) == TRUE)
// // //         {

            
// // //             if(stData.bFlags & KEY_FLAGS_DOWN)
// // //             {
// // //                 vcTemp[0] = stData.bASCIICode;
// // //                 kPrintString(i++, 17, vcTemp);

// // //                 if(vcTemp[0] == '0')
// // //                 {
// // //                     bTemp = bTemp / 0;
// // //                 }
// // //             }
    
// // //         }
// // //     }

// // // }

// // // void kPrintString(int iX, int iY, const char* pcString)
// // // {
// // //     CHARACTER* pstScreen = (CHARACTER*) 0xb8000;
// // //     int i;
    
// // //     pstScreen += (iY * 80) + iX;

// // //     for(i = 0; pcString[i] != 0; i++)
// // //     {
// // //         pstScreen[i].bCharactor = pcString[i];
// // //     }
// // // }

// #include "Types.h"
// #include "Keyboard.h"
// #include "Descriptor.h"
// #include "PIC.h"
// #include "Console.h"
// #include "ConsoleShell.h"

// /**
//  *  �Ʒ� �Լ��� C ��� Ŀ���� ���� �κ���
//  */
// void Main( void )
// {
//     int iCursorX, iCursorY;

//     // �ܼ��� ���� �ʱ�ȭ�� ��, ���� �۾��� ����
//     kInitializeConsole( 0, 10 );
//     kPrintf( "Switch To IA-32e Mode Success~!!\n" );
//     kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
//     kPrintf( "Initialize Console..........................[Pass]\n" );

//     // ���� ��Ȳ�� ȭ�鿡 ���
//     kGetCursor( &iCursorX, &iCursorY );
//     kPrintf( "GDT Initialize And Switch For IA-32e Mode...[    ]" );
//     kInitializeGDTTableAndTSS();
//     kLoadGDTR( GDTR_STARTADDRESS );
//     kSetCursor( 45, iCursorY++ );
//     kPrintf( "Pass\n" );

//     kPrintf( "TSS Segment Load............................[    ]" );
//     kLoadTR( GDT_TSSSEGMENT );
//     kSetCursor( 45, iCursorY++ );
//     kPrintf( "Pass\n" );

//     kPrintf( "IDT Initialize..............................[    ]" );
//     kInitializeIDTTables();
//     kLoadIDTR( IDTR_STARTADDRESS );
//     kSetCursor( 45, iCursorY++ );
//     kPrintf( "Pass\n" );

//     kPrintf( "Total RAM Size Check........................[    ]" );
//     kCheckTotalRAMSize();
//     kSetCursor( 45, iCursorY++ );
//     kPrintf( "Pass], Size = %d MB\n", kGetTotalRAMSize() );

//     kPrintf( "Keyboard Activate And Queue Initialize......[    ]" );
//     // Ű���带 Ȱ��ȭ
//     if( kInitializeKeyboard() == TRUE )
//     {
//         kSetCursor( 45, iCursorY++ );
//         kPrintf( "Pass\n" );
//         kChangeKeyboardLED( FALSE, FALSE, FALSE );
//     }
//     else
//     {
//         kSetCursor( 45, iCursorY++ );
//         kPrintf( "Fail\n" );
//         while( 1 ) ;
//     }

//     kPrintf( "PIC Controller And Interrupt Initialize.....[    ]" );
//     // PIC ��Ʈ�ѷ� �ʱ�ȭ �� ��� ���ͷ�Ʈ Ȱ��ȭ
//     kInitializePIC();
//     kMaskPICInterrupt( 0 );
//     kEnableInterrupt();
//     kSetCursor( 45, iCursorY++ );
//     kPrintf( "Pass\n" );

//     // ���� ����
//     kStartConsoleShell();
// }
#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"

/**
 *  �Ʒ� �Լ��� C ��� Ŀ���� ���� �κ���
 */
void Main( void )
{
    int iCursorX, iCursorY;

    // �ܼ��� ���� �ʱ�ȭ�� ��, ���� �۾��� ����
    kInitializeConsole( 0, 10 );
    kPrintf( "Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );

    // ���� ��Ȳ�� ȭ�鿡 ���
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf( "GDT Initialize And Switch For IA-32e Mode...[    ]" );
    kInitializeGDTTableAndTSS();
    kLoadGDTR( GDTR_STARTADDRESS );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    kPrintf( "TSS Segment Load............................[    ]" );
    kLoadTR( GDT_TSSSEGMENT );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    kPrintf( "IDT Initialize..............................[    ]" );
    kInitializeIDTTables();
    kLoadIDTR( IDTR_STARTADDRESS );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    kPrintf( "Total RAM Size Check........................[    ]" );
    kCheckTotalRAMSize();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass], Size = %d MB\n", kGetTotalRAMSize() );

    kPrintf( "Keyboard Activate And Queue Initialize......[    ]" );
    // Ű���带 Ȱ��ȭ
    if( kInitializeKeyboard() == TRUE )
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Pass\n" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Fail\n" );
        while( 1 ) ;
    }

    kPrintf( "PIC Controller And Interrupt Initialize.....[    ]" );
    // PIC ��Ʈ�ѷ� �ʱ�ȭ �� ��� ���ͷ�Ʈ Ȱ��ȭ
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    // ���� ����
    kStartConsoleShell();
}