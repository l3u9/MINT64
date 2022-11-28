#include "DynamicMemory.h"
#include "Utility.h"
#include "Task.h"

static DYNAMICMEMORY gs_stDynamicMemory;

void kInitializeDynamicMemory(void)
{
    QWORD qwDynamicMemorySize;
    int i, j;
    BYTE *pbCurrentBitmapPosition;
    int iBlockCountOfLevel, iMetaBlockCount;

    qwDynamicMemorySize = kCalculateDynamicMemorySize();
    iMetaBlockCount = kCalculateMetaBlockCount(qwDynamicMemorySize);

    gs_stDynamicMemory.iBlockCountOfSmallestBlock = (qwDynamicMemorySize / DYNAMICMEMORY_MIN_SIZE) - iMetaBlockCount;
    
    //최대 몇개의 블록 리스트로 구성되는지 계산
    for(i = 0; (gs_stDynamicMemory.iBlockCountOfSmallestBlock >> i) >0 ; i++)
    {
        ;
    }
    gs_stDynamicMemory.iMaxLevelCount = i;


    //할당된 메모리가 속한 블록 리스트의 인덱스를 저장하는 영역 초기화
    gs_stDynamicMemory.pbAllocatedBlockListIndex = (BYTE*) DYNAMICMEMORY_START_ADDRESS;
    for(i = 0; i < gs_stDynamicMemory.iBlockCountOfSmallestBlock; i++)
    {
        gs_stDynamicMemory.pbAllocatedBlockListIndex[i] = 0xff;
    }

    //비트맵 자료구조의 시작어드레스 지정
    gs_stDynamicMemory.pstBitmapOfLevel = (BITMAP*) (DYNAMICMEMORY_START_ADDRESS + (sizeof(BYTE) * gs_stDynamicMemory.iBlockCountOfSmallestBlock));
    //실제 비트맵의 어드레스 지정
    pbCurrentBitmapPosition = ((BYTE*) gs_stDynamicMemory.pstBitmapOfLevel) + (sizeof(BITMAP) * gs_stDynamicMemory.iMaxLevelCount);

    
    for(j = 0; j < gs_stDynamicMemory.iMaxLevelCount; j++)
    {
        gs_stDynamicMemory.pstBitmapOfLevel[j].pbBitmap = pbCurrentBitmapPosition;
        gs_stDynamicMemory.pstBitmapOfLevel[j].qwExistBitCount = 0;
        iBlockCountOfLevel = gs_stDynamicMemory.iBlockCountOfSmallestBlock >> j;

        for(i = 0; i < iBlockCountOfLevel/8; i++)
        {
            *pbCurrentBitmapPosition = 0x00;
            pbCurrentBitmapPosition++;
        }

        if((iBlockCountOfLevel%8) != 0)
        {
            *pbCurrentBitmapPosition = 0x00;

            i = iBlockCountOfLevel % 8;
            if((i % 2) == 1)
            {
                *pbCurrentBitmapPosition |= (DYNAMICMEMORY_EXIST << (i - 1));
                gs_stDynamicMemory.pstBitmapOfLevel[j].qwExistBitCount = 1;
            }
            pbCurrentBitmapPosition++;
        }
    }

    gs_stDynamicMemory.qwStartAddress = DYNAMICMEMORY_START_ADDRESS + iMetaBlockCount * DYNAMICMEMORY_MIN_SIZE;
    gs_stDynamicMemory.qwEndAddress = kCalculateDynamicMemorySize() + DYNAMICMEMORY_START_ADDRESS;
    gs_stDynamicMemory.qwUsedSize = 0;

    kInitializeSpinLock(&(gs_stDynamicMemory.stSpinLock));

}


static QWORD kCalculateDynamicMemorySize(void)
{
    QWORD qwRAMSize;

    qwRAMSize = (kGetTotalRAMSize() * 1024 * 1024);
    if(qwRAMSize > (QWORD) 3 * 1024 * 1024 * 1024)
    {
        qwRAMSize = (QWORD) 3 * 1024 * 1024 * 1024;
    }
    return qwRAMSize - DYNAMICMEMORY_START_ADDRESS;
}

//동적 메모리 영역을 관리하는데 필요한 정보가 차지하는 공간을 계산
static int kCalculateMetaBlockCount(QWORD qwDynamicRAMSize)
{
    long lBlockCountOfSmallestBlock;
    DWORD dwSizeOfAllocatedBlockListIndex;
    DWORD dwSizeOfBitmap;
    long i;
    
    // 가장 크기가 작은 블록의 개수를 계산하여 이를 기준으로 비트맵 영역과 
    // 할당된 크기를 저장하는 영역을 계산
    lBlockCountOfSmallestBlock = qwDynamicRAMSize / DYNAMICMEMORY_MIN_SIZE;
    // 할당된 블록이 속한 블록 리스트의 인덱스를 저장하는데 필요한 영역을 계산
    dwSizeOfAllocatedBlockListIndex = lBlockCountOfSmallestBlock * sizeof( BYTE );
    
    // 비트맵을 저장하는데 필요한 공간 계산
    dwSizeOfBitmap = 0;
    for( i = 0 ; ( lBlockCountOfSmallestBlock >> i ) > 0 ; i++ )
    {
        // 블록 리스트의 비트맵 포인터를 위한 공간
        dwSizeOfBitmap += sizeof( BITMAP );
        // 블록 리스트의 비트맵 크기, 바이트 단위로 올림 처리
        dwSizeOfBitmap += ( ( lBlockCountOfSmallestBlock >> i ) + 7 ) / 8;
    }
    
    // 사용한 메모리 영역의 크기를 최소 블록 크기로 올림해서 반환
    return ( dwSizeOfAllocatedBlockListIndex + dwSizeOfBitmap + 
        DYNAMICMEMORY_MIN_SIZE - 1 ) / DYNAMICMEMORY_MIN_SIZE;
}



void* kAllocateMemory(QWORD qwSize)
{
    QWORD qwAlignedSize;
    QWORD qwRelativeAddress;
    long lOffset;
    int iSizeArrayOffset;
    int iIndexOfBlockList;

    qwAlignedSize = kGetBuddyBlockSize(qwSize);
    if(qwAlignedSize == 0)
        return NULL;
    
    if(gs_stDynamicMemory.qwStartAddress + gs_stDynamicMemory.qwUsedSize + qwAlignedSize > gs_stDynamicMemory.qwEndAddress)
        return NULL;

    lOffset = kAllocationBuddyBlock(qwAlignedSize);
    if(lOffset == -1)
        return NULL;
    
    iIndexOfBlockList = kGetBlockListIndexOfMatchSize(qwAlignedSize);

    qwRelativeAddress = qwAlignedSize * lOffset;
    iSizeArrayOffset = qwRelativeAddress / DYNAMICMEMORY_MIN_SIZE;
    gs_stDynamicMemory.pbAllocatedBlockListIndex[iSizeArrayOffset] = (BYTE)iIndexOfBlockList;
    gs_stDynamicMemory.qwUsedSize += qwAlignedSize;

    return (void*) (qwRelativeAddress + gs_stDynamicMemory.qwStartAddress);
}

static QWORD kGetBuddyBlockSize(QWORD qwSize)
{
    long i;

    for(i = 0; i < gs_stDynamicMemory.iMaxLevelCount; i++)
    {
        if(qwSize <= (DYNAMICMEMORY_MIN_SIZE << i))
            return DYNAMICMEMORY_MIN_SIZE << i;
    }
    return 0;
}

static int kAllocationBuddyBlock(QWORD qwAlignedSize)
{
    int iBlockListIndex, iFreeOffset;
    int i;

    iBlockListIndex = kGetBlockListIndexOfMatchSize(qwAlignedSize);
    if(iBlockListIndex == -1)
        return -1;

    kLockForSpinLock(&(gs_stDynamicMemory.stSpinLock));

    for(i = iBlockListIndex; i < gs_stDynamicMemory.iMaxLevelCount; i++)
    {
        iFreeOffset = kFindFreeBlockInBitmap(i);
        if(iFreeOffset != -1)
            break;
    }
    if(iFreeOffset == -1)
    {
        kUnlockForSpinLock(&(gs_stDynamicMemory.stSpinLock));
        return -1;
    }

    kSetFlagInBitmap(i, iFreeOffset, DYNAMICMEMORY_EMPTY);

    if(i>iBlockListIndex)
    {
        for(i = i -1; i >= iBlockListIndex; i--)
        {
            kSetFlagInBitmap(i, iFreeOffset*2, DYNAMICMEMORY_EMPTY);

            kSetFlagInBitmap(i, iFreeOffset * 2 + 1, DYNAMICMEMORY_EXIST);
            iFreeOffset = iFreeOffset * 2;
        }
    }

    kUnlockForSpinLock(&(gs_stDynamicMemory.stSpinLock));
    return iFreeOffset;
}

static int kGetBlockListIndexOfMatchSize(QWORD qwAlignedSize)
{
    int i;
    for(i = 0; i < gs_stDynamicMemory.iMaxLevelCount; i++)
    {
        if(qwAlignedSize <= (DYNAMICMEMORY_MIN_SIZE << i))
            return i;
    }
    return -1;
}

static int kFindFreeBlockInBitmap(int iBlockListIndex)
{
    int i, iMaxCount;
    BYTE* pbBitmap;
    QWORD* pqwBitmap;

    if(gs_stDynamicMemory.pstBitmapOfLevel[iBlockListIndex].qwExistBitCount == 0)
        return -1;
    
    iMaxCount = gs_stDynamicMemory.iBlockCountOfSmallestBlock >> iBlockListIndex;
    pbBitmap = gs_stDynamicMemory.pstBitmapOfLevel[iBlockListIndex].pbBitmap;
    for(i = 0; i < iMaxCount;)
    {
        if(((iMaxCount - i) / 64) > 0)
        {
            pqwBitmap = (QWORD*)&(pbBitmap[i/8]);

            if(*pqwBitmap == 0)
            {
                i += 64;
                continue;
            }
        }
        if((pbBitmap[i/8] & (DYNAMICMEMORY_EXIST << (i%8))) != 0)
            return i;
        i++;
    }
    return -1;
}

static void kSetFlagInBitmap(int iBlockListIndex, int iOffset, BYTE bFlag)
{
    BYTE* pbBitmap;

    pbBitmap = gs_stDynamicMemory.pstBitmapOfLevel[iBlockListIndex].pbBitmap;
    if(bFlag == DYNAMICMEMORY_EXIST)
    {
        if((pbBitmap[iOffset / 8] & (0x01 << (iOffset % 8))) == 0)
            gs_stDynamicMemory.pstBitmapOfLevel[iBlockListIndex].qwExistBitCount++;

        pbBitmap[iOffset/8] |= (0x1 << (iOffset % 8));
    }
    else
    {
        if((pbBitmap[iOffset/8] & (0x01 << (iOffset%8))) != 0)
            gs_stDynamicMemory.pstBitmapOfLevel[iBlockListIndex].qwExistBitCount--;
        pbBitmap[iOffset/8] &= ~(0x1 << (iOffset%8));
    }
}

BOOL kFreeMemory(void* pvAddress)
{
    QWORD qwRelativeAddress;
    int iSizeArrayOffset;
    QWORD qwBlockSize;
    int iBlockListIndex;
    int iBitmapOffset;

    if(pvAddress == NULL)
        return FALSE;
    
    qwRelativeAddress = ((QWORD) pvAddress) - gs_stDynamicMemory.qwStartAddress;
    iSizeArrayOffset = qwRelativeAddress / DYNAMICMEMORY_MIN_SIZE;

    if(gs_stDynamicMemory.pbAllocatedBlockListIndex[iSizeArrayOffset] == 0xff)
        return FALSE;
    
    iBlockListIndex = (int) gs_stDynamicMemory.pbAllocatedBlockListIndex[iSizeArrayOffset];
    gs_stDynamicMemory.pbAllocatedBlockListIndex[iSizeArrayOffset] = 0xff;
    qwBlockSize = DYNAMICMEMORY_MIN_SIZE << iBlockListIndex;

    iBitmapOffset = qwRelativeAddress / qwBlockSize;
    if(kFreeBuddyBlock(iBlockListIndex, iBitmapOffset) == TRUE)
    {
        gs_stDynamicMemory.qwUsedSize -= qwBlockSize;
        return TRUE;
    }
    return FALSE;
}

static BOOL kFreeBuddyBlock(int iBlockListIndex, int iBlockOffset)
{
    int iBuddyBlockOffset;
    int i;
    BOOL bFlag;

    kLockForSpinLock(&(gs_stDynamicMemory.stSpinLock));

   for( i = iBlockListIndex; i < gs_stDynamicMemory.iMaxLevelCount; i++)
   {
    kSetFlagInBitmap(i, iBlockOffset, DYNAMICMEMORY_EXIST);

    if((iBlockOffset % 2) == 0)
        iBuddyBlockOffset = iBlockOffset + 1;
    else
        iBuddyBlockOffset = iBlockOffset - 1;
    
    bFlag = kGetFlagInBitmap(i, iBuddyBlockOffset);

    if(bFlag == DYNAMICMEMORY_EMPTY)
        break;
    
    kSetFlagInBitmap(i, iBuddyBlockOffset, DYNAMICMEMORY_EMPTY);
    kSetFlagInBitmap(i, iBlockOffset, DYNAMICMEMORY_EMPTY);

    iBlockOffset = iBlockOffset / 2;
   }

    kUnlockForSpinLock(&(gs_stDynamicMemory.stSpinLock));
    return TRUE;
}

static BYTE kGetFlagInBitmap(int iBlockListIndex, int iOffset)
{
    BYTE* pbBitmap;

    pbBitmap = gs_stDynamicMemory.pstBitmapOfLevel[iBlockListIndex].pbBitmap;
    if((pbBitmap[iOffset/ 8] & (0x1 << (iOffset % 8))) != 0x0)
        return DYNAMICMEMORY_EXIST;
        
    return DYNAMICMEMORY_EMPTY;
}

void kGetDynamicMemoryInformation(QWORD* pqwDynamicMemoryStartAddress, QWORD* pqwDynamicMemoryTotalSize, QWORD* pqwMetaDataSize, QWORD* pqwUsedMemorySize)
{
    *pqwDynamicMemoryStartAddress = DYNAMICMEMORY_START_ADDRESS;
    *pqwDynamicMemoryTotalSize = kCalculateDynamicMemorySize();
    *pqwMetaDataSize = kCalculateMetaBlockCount(*pqwDynamicMemoryTotalSize) * DYNAMICMEMORY_MIN_SIZE;
    *pqwUsedMemorySize = gs_stDynamicMemory.qwUsedSize;
}

DYNAMICMEMORY* kGetDynamicMemoryManager(void)
{
    return &gs_stDynamicMemory;
}
