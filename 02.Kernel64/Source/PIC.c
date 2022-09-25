#include "PIC.h"

void kInitializePIC(void)
{
    // ICW1(0x20 포트), IC4 비트 0 = 1
    kOutPortByte(PIC_MASTER_PORT1, 0x11);
    
    kOutPortByte(PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);

    kOutPortByte(PIC_MASTER_PORT2, 0x04);

    kOutPortByte(PIC_MASTER_PORT2, 0x01);

    kOutPortByte(PIC_SLAVE_PORT1, 0x11);

    // ICW2(포트 0xA1), 인터럽트 벡터 (0x20 + 8)
    kOutPortByte(PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8);

    //ICW3(포트 0xA1), 마스터 PIC 컨트롤러에 연결된 위치
    // 마스터 PIC 컨트롤러의 2번 핀에 연결되어 있으므로 0x02로 설정
    kOutPortByte(PIC_SLAVE_PORT2, 0x02);
    
    //ICW4(포트 0xA1), uPM 비트 0 = 1
    kOutPortByte(PIC_SLAVE_PORT2, 0x01);


}

void kMaskPICInterrupt(WORD wIRQBitMask)
{
    // 마스터 PIC 컨트롤러에 IMR 설정
    // OCW1(포트 0x21), IRQ 0 ~ 7
    kOutPortByte(PIC_MASTER_PORT2, (BYTE) wIRQBitMask);

    // OCW1(포트 0x21), IRQ 8 ~ 15
    kOutPortByte(PIC_SLAVE_PORT2, (BYTE)wIRQBitMask >> 8);
}

void kSendEOIToPIC(int iIRQNumber)
{
    kOutPortByte(PIC_MASTER_PORT1, 0x20);

    if(iIRQNumber >= 8)
        kOutPortByte(PIC_SLAVE_PORT1, 0x20);


}
