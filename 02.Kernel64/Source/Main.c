#include "Types.h"

//함수 선언
void kPrintString( int iX, int iY, const char* pcString);

// 아래 함수는 C언어 커널의 시작 부분
void Main(void)
{
    kPrintString(0, 10, "Switch To IA-32e Mode Success~!!");
    kPrintString(0, 11, "IA-32e CLanguage Kernel Start..............[Pass]");
}

void kPrintString(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = (CHARACTER*) 0xb8000;
    int i;
    
    pstScreen += (iY * 80) + iX;

    for(i = 0; pcString[i] != 0; i++)
    {
        pstScreen[i].bCharactor = pcString[i];
    }
}