#define ERROR 0
#include<stdio.h>

void HandleDiskError()
{
    printf("DISK Error~!!");
    while(1);
}

int main(int argc, char* argv[])
{
    int iTotalSectorCount = 1024;
    int iSectorNumber = 2;
    int iHeadNumber = 0;
    int ITrackNumber = 0;
    char* pcTargetAddress = (char*) 0x10000;

    while(1)
    {
        if (iTotalSectorCount == 0)
        {
            break;
        }

        iTotalSectorCount = iTotalSectorCount - 1;

        if( BIOSReadOneSector( iSectorNumber, iHeadNumber, ITrackNumber, pcTargetAddress) == ERROR)
        {
            HandleDiskError();
        }

        pcTargetAddress = pcTargetAddress + 0x200;

        iSectorNumber = iSectorNumber + 1;

        if(iSectorNumber < 19)
        {
            continue;
        }

        ITrackNumber = ITrackNumber + 1;


    }
    return 0;
}

