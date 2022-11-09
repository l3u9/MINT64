#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "Types.h"
#include "Task.h"

BYTE kInPortByte( WORD wPort );
void kOutPortByte( WORD wPort, BYTE bData );
void kLoadGDTR( QWORD qwGDTRAddress );
void kLoadTR( WORD wTSSSegmentOffset );
void kLoadIDTR( QWORD qwIDTRAddress);
void kEnableInterrupt(void);
void kDisableInterrupt(void);
QWORD kReadRFLAGS(void);
BOOL kSetInterruptFlag(BOOL bEnableInterrupt);
QWORD kReadTSC(void);
void kSwitchContext(CONTEXT* pstCurrentContext, CONTEXT *pstNextContext);
void kHlt(void);
BOOL kTestAndSet(volatile BYTE* pdDestination, BYTE bCompare, BYTE bSource);
void kInitializeFPU(void);
void kSaveFPUContext(void *pvFPUContext);
void kLoadFPUContext(void *pvFPUContex);
void kSetTS(void);
void kClearTS(void);
WORD kInPortWord(WORD wPort);
void kOutPortWord(WORD wPort, WORD wData);

#endif