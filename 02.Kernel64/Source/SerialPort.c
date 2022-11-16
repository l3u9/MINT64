#include "SerialPort.h"
#include "Utility.h"

static SERIALMANAGER gs_stSerialManager;

void kInitializeSerialPort(void)
{
    WORD wPortBaseAddress;

    kInitializeMutex(&(gs_stSerialManager.stLock));

    wPortBaseAddress = SERIAL_PORT_COM1;

    kOutPortByte(wPortBaseAddress + SERIAL_PORT_INDEX_INTERRUPTENABLE, 0);

    kOutPortByte(wPortBaseAddress + SERIAL_PORT_INDEX_LINECONTROL, SERIAL_LINECONTROL_DLAB);

    kOutPortByte(wPortBaseAddress + SERIAL_PORT_INDEX_DIVISORLATCHLSB, SERIAL_DIVISORLATCH_115200);

    kOutPortByte(wPortBaseAddress + SERIAL_PORT_INDEX_DIVISORLATCHMSB, SERIAL_DIVISORLATCH_115200 >> 8);

    kOutPortByte(wPortBaseAddress + SERIAL_PORT_INDEX_LINECONTROL, SERIAL_LINECONTROL_8BIT | SERIAL_LINECONTROL_NOPARITY | SERIAL_LINECONTROL_1BITSTOP);

    kOutPortByte(wPortBaseAddress + SERIAL_PORT_INDEX_FIFOCONTROL, SERIAL_FIFOCONTROL_FIFOENABLE | SERIAL_FIFOCONTROL_14BYTEFIFO);
}

static BOOL kIsSerialTransmitterBufferEmpty(void)
{
    BYTE bData;

    // 라인 상태 레지스터를 읽은 뒤 TBE 비트를 확인하여 송신 FIFO가 비어있는지 확인
    bData = kInPortByte(SERIAL_PORT_COM1 + SERIAL_PORT_INDEX_LINESTATUS);

    if((bData & SERIAL_LINESTATUS_TRANSMITBUFFEREMPTY) == SERIAL_LINESTATUS_TRANSMITBUFFEREMPTY)
        return TRUE;
    return FALSE;
}

void kSendSerialData(BYTE* pbBuffer, int iSize)
{
    int iSentByte;
    int iTempSize;
    int j;

    kLock(&(gs_stSerialManager.stLock));

    iSentByte = 0;

    while(iSentByte < iSize)
    {
        while(kIsSerialTransmitterBufferEmpty() == FALSE)
            kSleep(0);
        
        iTempSize = MIN(iSize - iSentByte, SERIAL_FIFOMAXSIZE);
        for(j = 0; j < iTempSize; j++)
            kOutPortByte(SERIAL_PORT_COM1 + SERIAL_PORT_INDEX_TRANSMITBUFFER, pbBuffer[iSentByte + j]);
        iSentByte += iTempSize;
    }
    kUnlock(&(gs_stSerialManager.stLock));
}

static BOOL kIsSerialReceiveBufferFull(void)
{
    BYTE bData;

    //라인 상태 레지스터를 읽은 뒤 RxRD 비트를 확인하여 수신 FIFO 데이터가 있는지 확인
    bData = kInPortByte(SERIAL_PORT_COM1 + SERIAL_PORT_INDEX_LINESTATUS);
    if((bData & SERIAL_LINESTATUS_RECEIVEDDATAREADY) == SERIAL_LINESTATUS_RECEIVEDDATAREADY)
        return TRUE;
    return FALSE;
}

int kReceiveSerialData(BYTE* pbBuffer, int iSize)
{
    int i;

    kLock(&(gs_stSerialManager.stLock));

    for(i = 0; i < iSize; i++)
    {
        if(kIsSerialReceiveBufferFull() == FALSE)
            break;
        pbBuffer[i] = kInPortByte(SERIAL_PORT_COM1 + SERIAL_PORT_INDEX_RECEIVEBUFFER);
    }

    kUnlock(&(gs_stSerialManager.stLock));

    return i;
}

void kClearSerialFIFO(void)
{
    kLock(&(gs_stSerialManager.stLock));

    //송수신 FIFO를 모두 비우고 버퍼에 데이터가 14바이트가 찼을 때 인터럽이 발생하도록 FIFO 제어 레지스터에 설정한 다음 전송
    kOutPortByte(SERIAL_PORT_COM1 + SERIAL_PORT_INDEX_FIFOCONTROL, SERIAL_FIFOCONTROL_FIFOENABLE | SERIAL_FIFOCONTROL_14BYTEFIFO | SERIAL_FIFOCONTROL_CLEARRECEIVEFIFO | SERIAL_FIFOCONTROL_CLEARTRANSMITFIFO);

    kUnlock(&(gs_stSerialManager.stLock));
}

