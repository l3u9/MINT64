#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "Types.h"
#pragma pack(push, 1)

typedef struct kQueueManagerStruct
{
    int iDataSize;
    int iMaxDataCount;

    void* pvQueueArray;
    int iPutIndex;
    int iGetIndex;

    BOOL bLastPoerationPut;
}QUEUE;

void kInitializeQueue(QUEUE* pstQueue, void* pvQueueBuffer, int iMaxDataCount, int iDataSize);
BOOL kIsQueueFull(const QUEUE* pstQueue);
BOOL kIsQueueEmpty(const QUEUE* pstQueue);
BOOL kPutQueue(QUEUE* pstQueue, const void* pvData);
BOOL kGetQueue(QUEUE* ostQueue, void* pvData);

#endif

