#ifndef __INTERRUPTHANDLER_H__
#define __INTERRUPTHANDLER_H__

#include "Types.h"
#include "MultiProcessor.h"

#define INTERRUPT_MAXVECTORCOUNT                16
#define INTERRUPT_LOADBALANCINGDIVIDOR          10

typedef struct kInterruptManagerStruct
{
    QWORD vvqwCoreInterruptCount[MAXPROCESSORCOUNT][INTERRUPT_MAXVECTORCOUNT];
    BOOL bUseLoadBalancing;
    BOOL bSymmetricIOMode;
}INTERRUPTMANAGER;

void kSetSymmetricIOMode(BOOL bSymmetricIOMode);
void kSetInterruptLoadBalancing(BOOL bUseLoadBalancing);
void kIncreaseInterruptCount(int iIRQ);
void kSendEOI(int iIRQ);
INTERRUPTMANAGER* kGetInterruptManager(void);
void kProcessLoadBalancing(int iIRQ);


void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode);
void kCommonInterruptHandlr(int iVectorNumber);
void kKeyboardHandler(int iVectorNumber);
void kTimerHandler(int iVectorNumber);
void kDeviceNotAvailableHandler(int iVectyorNumber);
void kHDDHandler(int iVectorNumber);
void kMouseHandler(int iVectorNumber);

#endif

