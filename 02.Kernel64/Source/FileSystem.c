#include "FileSystem.h"
#include "HardDisk.h"
#include "DynamicMemory.h"

static FILESYSTEMMANAGER gs_stFileSystemManager;
static BYTE gs_vbTempBuffer[FILESYSTEM_SECTORSPERCLUSTER * 512];

fReadHDDInformation gs_pfReadHDDInformation = NULL;
fReadHDDSector gs_pfReadHDDSector = NULL;
fWriteHDDSector gs_pfWriteHDDSector = NULL;

BOOL kInitializeFileSystem(void)
{
    kMemSet(&gs_stFileSystemManager, 0, sizeof(gs_stFileSystemManager));
    kInitializeMutex(&(gs_stFileSystemManager.stMutex));

    if(kInitializeHDD() == TRUE)
    {
        gs_pfReadHDDInformation = kReadHDDInformation;
        gs_pfReadHDDSector = kReadHDDSector;
        gs_pfWriteHDDSector = kWriteHDDSector;
    }
    else
        return FALSE;
    
    if(kMount() == FALSE)
        return FALSE;

    return TRUE;
}


BOOL kMount(void)
{
    MBR* pstMBR;

    kLock(&(gs_stFileSystemManager.stMutex));

    if(gs_pfReadHDDSector(TRUE, TRUE, 0, 1, gs_vbTempBuffer) == FALSE)
    {
        kUnlock(&(gs_stFileSystemManager.stMutex));
        return FALSE;
    }

    pstMBR = (MBR*) gs_vbTempBuffer;
    if(pstMBR->dwSignature != FILESYSTEM_SIGNATURE)
    {
        kUnlock(&(gs_stFileSystemManager.stMutex));
        return FALSE;
    }

    gs_stFileSystemManager.bMounted = TRUE;

    gs_stFileSystemManager.dwReservedSectorCount = pstMBR->dwReservedSectorCount;
    gs_stFileSystemManager.dwClusterLinkAreaStartAddress = pstMBR->dwReservedSectorCount + 1;
    gs_stFileSystemManager.dwClusterLinkAreaSize = pstMBR->dwClusterLinkSectorCount;
    gs_stFileSystemManager.dwDataAreaStartAddress = pstMBR->dwReservedSectorCount + pstMBR->dwClusterLinkSectorCount + 1;
    gs_stFileSystemManager.dwTotalClusterCount = pstMBR->dwTotalClusterCount;

    kUnlock(&(gs_stFileSystemManager.stMutex));
    return TRUE;

}

BOOL kFormat( void )
{
    HDDINFORMATION* pstHDD;
    MBR* pstMBR;
    DWORD dwTotalSectorCount, dwRemainSectorCount;
    DWORD dwMaxClusterCount, dwClsuterCount;
    DWORD dwClusterLinkSectorCount;
    DWORD i;
    
    kLock(&(gs_stFileSystemManager.stMutex));


    pstHDD = (HDDINFORMATION*) gs_vbTempBuffer;
    if(gs_pfReadHDDInformation(TRUE, TRUE, pstHDD) == FALSE)
    {
        // 동기화 처리
        kUnlock(&(gs_stFileSystemManager.stMutex));
        return FALSE;
    }    
    dwTotalSectorCount = pstHDD->dwTotalSectors;
    
    // 전체 섹터 수를 4Kbyte, 즉 클러스터 크기로 나누어 최대 클러스터 수를 계산
    dwMaxClusterCount = dwTotalSectorCount / FILESYSTEM_SECTORSPERCLUSTER;
    
    // 최대 클러스터의 수에 맞추어 클러스터 링크 테이블의 섹터 수를 계산
    // 링크 데이터는 4바이트이므로, 한 섹터에는 128개가 들어감. 따라서 총 개수를
    // 128로 나눈 후 올림하여 클러스터 링크의 섹터 수를 구함
    dwClusterLinkSectorCount = (dwMaxClusterCount + 127) / 128;
    
    // 예약된 영역은 현재 사용하지 않으므로, 디스크 전체 영역에서 MBR 영역과 클러스터
    // 링크 테이블 영역의 크기를 뺀 나머지가 실제 데이터 영역이 됨
    // 해당 영역을 클러스터 크기로 나누어 실제 클러스터의 개수를 구함
    dwRemainSectorCount = dwTotalSectorCount - dwClusterLinkSectorCount - 1;
    dwClsuterCount = dwRemainSectorCount / FILESYSTEM_SECTORSPERCLUSTER;
    
    // 실제 사용 가능한 클러스터 수에 맞추어 다시 한번 계산
    dwClusterLinkSectorCount = (dwClsuterCount + 127) / 128;

    if(gs_pfReadHDDSector(TRUE,TRUE, 0, 1, gs_vbTempBuffer) == FALSE )
    {
        kUnlock( &(gs_stFileSystemManager.stMutex));
        return FALSE;
    }        
    
    pstMBR = (MBR*) gs_vbTempBuffer;
    kMemSet( pstMBR->vstPartition, 0, sizeof(pstMBR->vstPartition));
    pstMBR->dwSignature = FILESYSTEM_SIGNATURE;
    pstMBR->dwReservedSectorCount = 0;
    pstMBR->dwClusterLinkSectorCount = dwClusterLinkSectorCount;
    pstMBR->dwTotalClusterCount = dwClsuterCount;
    
    if( gs_pfWriteHDDSector(TRUE, TRUE, 0, 1, gs_vbTempBuffer) == FALSE )
    {
        kUnlock(&(gs_stFileSystemManager.stMutex) );
        return FALSE;
    }
    
    kMemSet(gs_vbTempBuffer, 0, 512);
    for(i = 0 ; i < (dwClusterLinkSectorCount + FILESYSTEM_SECTORSPERCLUSTER); i++)
    {

        if( i == 0 )
            ((DWORD*) (gs_vbTempBuffer) )[0] = FILESYSTEM_LASTCLUSTER;
        else
            ((DWORD*) (gs_vbTempBuffer))[0] = FILESYSTEM_FREECLUSTER;
        
        if(gs_pfWriteHDDSector(TRUE, TRUE, i + 1, 1, gs_vbTempBuffer) == FALSE)
        {
            kUnlock(&(gs_stFileSystemManager.stMutex));
            return FALSE;
        }
    }    
    
    kUnlock(&(gs_stFileSystemManager.stMutex));
    return TRUE;
}

BOOL kGetHDDInformation(HDDINFORMATION* pstInformation)
{
    BOOL bResult;

    kLock(&(gs_stFileSystemManager.stMutex));
    
    bResult -= gs_pfReadHDDInformation(TRUE, TRUE, pstInformation);

    kUnlock(&(gs_stFileSystemManager.stMutex));

    return bResult;
}

BOOL kReadClusterLinkTable(DWORD dwOffset, BYTE* pbBuffer)
{
    return gs_pfReadHDDSector(TRUE, TRUE, dwOffset + gs_stFileSystemManager.dwClusterLinkAreaStartAddress, 1, pbBuffer);
}

BOOL kWriteClusterLinkTable(DWORD dwOffset, BYTE* pbBuffer)
{
    return gs_pfWriteHDDSector(TRUE, TRUE, dwOffset + gs_stFileSystemManager.dwClusterLinkAreaStartAddress, 1, pbBuffer);
}

BOOL kReadCluster(DWORD dwOffset, BYTE* pbBuffer)
{
return gs_pfReadHDDSector( TRUE, TRUE, ( dwOffset * FILESYSTEM_SECTORSPERCLUSTER ) + 
                        gs_stFileSystemManager.dwDataAreaStartAddress,
                        FILESYSTEM_SECTORSPERCLUSTER, pbBuffer );
            
}

BOOL kWriteCluster(DWORD dwOffset, BYTE* pbBuffer)
{
    return gs_pfWriteHDDSector(TRUE, TRUE, (dwOffset * FILESYSTEM_SECTORSPERCLUSTER) + gs_stFileSystemManager.dwDataAreaStartAddress, FILESYSTEM_SECTORSPERCLUSTER, pbBuffer);
}

DWORD kFindFreeCluster(void)
{
    DWORD dwLinkCountInSector;
    DWORD dwLastSectorOffset, dwCurrentSectorOffset;
    DWORD i, j;

    if(gs_stFileSystemManager.bMounted == FALSE)
        return FILESYSTEM_LASTCLUSTER;

    dwLastSectorOffset = gs_stFileSystemManager.dwLastAllocatedClusterLinkSectorOffset;
    for(i = 0; i < gs_stFileSystemManager.dwClusterLinkAreaSize; i++)
    {
        if((dwLastSectorOffset + i) == (gs_stFileSystemManager.dwClusterLinkAreaSize - 1))
            dwLinkCountInSector = gs_stFileSystemManager.dwTotalClusterCount % 128;
        else
            dwLinkCountInSector = 128;
        
        dwCurrentSectorOffset = (dwLastSectorOffset + i) % gs_stFileSystemManager.dwClusterLinkAreaSize;
        if(kReadClusterLinkTable(dwCurrentSectorOffset, gs_vbTempBuffer) == FALSE)
            return FILESYSTEM_LASTCLUSTER;
        
        for(j = 0; j < dwLinkCountInSector; j++)
        {
            if(((DWORD*) gs_vbTempBuffer)[j] == FILESYSTEM_FREECLUSTER)
                break;
        }

        if(j != dwLinkCountInSector)
        {
            gs_stFileSystemManager.dwLastAllocatedClusterLinkSectorOffset = dwCurrentSectorOffset;
            return (dwCurrentSectorOffset * 128) + j;
        }
    }
    return FILESYSTEM_LASTCLUSTER;

}


BOOL kSetClusterLinkData(DWORD dwClusterIndex, DWORD dwData)
{
    DWORD dwSectorOffset;

    if(gs_stFileSystemManager.bMounted == FALSE)
        return FALSE;
    
    dwSectorOffset = dwClusterIndex / 128;

    if(kReadClusterLinkTable(dwSectorOffset, gs_vbTempBuffer) == FALSE)
        return FALSE;
    
    ((DWORD*) gs_vbTempBuffer)[dwClusterIndex % 128] = dwData;

    if(kWriteClusterLinkTable(dwSectorOffset, gs_vbTempBuffer) == FALSE)
        return FALSE;
    
    return TRUE;
}

BOOL kGetClusterLinkData(DWORD dwClusterIndex, DWORD* pdwData)
{
    DWORD dwSectorOFfset;

    if(gs_stFileSystemManager.bMounted == FALSE)
        return FALSE;
    
    dwSectorOFfset = dwClusterIndex / 128;

    if(dwSectorOFfset > gs_stFileSystemManager.dwClusterLinkAreaSize)
        return FALSE;
    
    if(kReadClusterLinkTable(dwSectorOFfset, gs_vbTempBuffer) == FALSE)
        return FALSE;
    
    *pdwData = ((DWORD*) gs_vbTempBuffer)[dwClusterIndex % 128];
    return TRUE;
}

int kFindFreeDirectoryEntry(void)
{
    DIRECTORYENTRY* pstEntry;
    int i;

    if(gs_stFileSystemManager.bMounted == FALSE)
        return -1;
    
    if(kReadCluster(0, gs_vbTempBuffer) == FALSE)
        return -1;
    
    pstEntry = (DIRECTORYENTRY*) gs_vbTempBuffer;
    for(i = 0; i <FILESYSTEM_MAXDIRECTORYENTRYCOUNT; i++)
    {
        if(pstEntry[i].dwStartClusterIndex == 0)
            return i;
    }
    return -1;
}


BOOL kSetDirectoryEntryData(int iIndex, DIRECTORYENTRY* pstEntry)
{
    DIRECTORYENTRY* pstRootEntry;

    if((gs_stFileSystemManager.bMounted == FALSE) || (iIndex < 0) || (iIndex >= FILESYSTEM_MAXDIRECTORYENTRYCOUNT))
        return FALSE;
    
    if(kReadCluster(0, gs_vbTempBuffer) == FALSE)
        return FALSE;
    
    pstRootEntry = (DIRECTORYENTRY*) gs_vbTempBuffer;
    kMemSet(pstRootEntry + iIndex, pstEntry, sizeof(DIRECTORYENTRY));

    if(kWriteCluster(0, gs_vbTempBuffer) == FALSE)
        return FALSE;
    return TRUE;
}

BOOL kGetDirectoryEntryData(int iIndex, DIRECTORYENTRY* pstEntry)
{
    DIRECTORYENTRY* pstRootEntry;

    if((gs_stFileSystemManager.bMounted == FALSE) || (iIndex < 0) || (iIndex < FILESYSTEM_MAXDIRECTORYENTRYCOUNT))
        return FALSE;

    if(kReadCluster(0, gs_vbTempBuffer) == FALSE)
        return FALSE;
    
    pstRootEntry = (DIRECTORYENTRY*) gs_vbTempBuffer;
    kMemSet(pstEntry, pstRootEntry + iIndex, sizeof(DIRECTORYENTRY));
    return TRUE;
}


int kFindDirectoryEntry(const char* pcFileName, DIRECTORYENTRY* pstEntry)
{
    DIRECTORYENTRY* pstRootEntry;
    int i;
    int iLength;

    if(gs_stFileSystemManager.bMounted == FALSE)
        return -1;

    if(kReadCluster(0, gs_vbTempBuffer) == FALSE)
        return -1;
    
    iLength = kStrLen(pcFileName);

    pstRootEntry = (DIRECTORYENTRY*)gs_vbTempBuffer;
    for(i = 0; i < FILESYSTEM_MAXDIRECTORYENTRYCOUNT; i++)
    {
        if(kMemCmp(pstRootEntry[i].vcFileName, pcFileName, iLength) == 0)
        {
            kMemCpy(pstEntry, pstRootEntry + i, sizeof(DIRECTORYENTRY));
            return i;
        }
    }  
    return -1;
}

void kGetFileSystemInformation(FILESYSTEMMANAGER* pstManager)
{
    kMemCpy(pstManager, &gs_stFileSystemManager, sizeof(gs_stFileSystemManager));
}






