#pragma once

#include "Synchronization.h"
#include "Types.h"

#define DYNAMICMEMORY_START_ADDRESS                                            \
  ((TASK_STACKPOOLADDRESS + (TASK_STACKSIZE * TASK_MAXCOUNT) + 0xfffff) &      \
   0xfffffffffff00000)

#define DYNAMICMEMORY_MIN_SIZE (1 * 1024)

#define DYNAMICMEMORY_EXIST 0x01
#define DYNAMICMEMORY_EMPTY 0x00

typedef struct kBitmapStruct {
  BYTE *pbBitmap;
  QWORD qwExistBitCount;
} BITMAP;

typedef struct kDynamicMemoryManagerStruct {
  SPINLOCK stSpinLock;

  int iMaxLevelCount;
  int iBlockCountOfSmallestBlock;
  QWORD qwUsedSize;

  QWORD qwStartAddress;
  QWORD qwEndAddress;

  BYTE *pbAllocatedBlockListIndex;
  BITMAP *pstBitmapOfLevel;
} DYNAMICMEMORY;

void kInitializeDynamicMemory();
void *kAllocateMemory(QWORD qwSize);
BOOL kFreeMemory(void *pvAddress);
void kGetDynamicMemoryInformation(QWORD *pqwDynamicMemoryStartAddress,
                                  QWORD *pqwDynamicMemoryTotalSize,
                                  QWORD *pqwMetaDataSize,
                                  QWORD *pqwUsedMemorySize);
DYNAMICMEMORY *kGetDynamicMemoryManager();

static QWORD kCalculateDynamicMemorySize();
static int kCalculateMetaBlockCount(QWORD qwDynamicRAMSize);
static int kAllocationBuddyBlock(QWORD qwAlignedSize);
static QWORD kGetBuddyBlockSize(QWORD qwSize);
static int kGetBlockListIndexOfMatchSize(QWORD qwAlignedSize);
static int kFindFreeBlockInBitmap(int iBlockListIndex);
static void kSetFlagInBitmap(int iBlockListIndex, int iOffset, BYTE bFlag);
static BOOL kFreeBuddyBlock(int iBlockListIndex, int iBlockOffset);
static BYTE kGetFlagInBitmap(int iBlockListIndex, int iOffset);