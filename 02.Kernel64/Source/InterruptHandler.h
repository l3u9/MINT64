#pragma once

#include "MultiProcessor.h"
#include "Types.h"

#define INTERRUPT_MAXVECTORCOUNT 16
#define INTERRUPT_LOADBALANCINGDIVIDOR 10

typedef struct kInterruptManagerStruct {
  QWORD vvqwCoreInterruptCount[MAXPROCESSORCOUNT][INTERRUPT_MAXVECTORCOUNT];
  BOOL bUseLoadBalancing;
  BOOL bSymmetricIOMode;
} INTERRUPTMANAGER;

void kSetSymmetricIOMode(BOOL bSymmetricIOMode);
void kSetInterruptLoadBalancing(BOOL bUseLoadBalancing);
void kIncreaseInterruptCount(int iIRQ);
void kSendEOI(int iIRQ);
INTERRUPTMANAGER *kGetInterruptManager(void);
void kProcessLoadBalancing(int iIRQ);
void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode);
void kCommonIntteruptHandler(int iVectorNumber);
void kKeyboardHandler(int iVectorNumber);
void kTimerHandler(int iVectorNumber);
void kDeviceNotAvailableHandler(int iVectorNumber);
void kHDDHandler(int iVectorNumber);
void kMouseHandler(int iVectorNumber);