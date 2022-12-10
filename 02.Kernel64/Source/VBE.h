#ifndef __VBE_H__
#define __VBE_H__

#include "Types.h"

#define VBE_MODEINFOBLOCKADDRESS            0x7e00
#define VBE_STARTGRAPHICMODEFLAGADDRESS     0x7c0a

#pragma pack(push, 1)

typedef struct kVBEInfoBlockStruct
{
    WORD wModeAttribute;
    BYTE bWinAAttribute;
    BYTE bWinBAttribute;
    WORD wWinGranulity;
    WORD wWinSize;
    WORD wWInAsegment;
    WORD wWinBSegment;
    DWORD dwWinFuncPtr;
    WORD wBytePerScanLine;

    WORD wXResolution;
    WORD wYResolution;
    BYTE bXCharSize;
    BYTE bYCharSize;
    BYTE bNumberOfPlane;
    BYTE bBitsPerPixel;
    BYTE bNumberOfBanks;
    BYTE bMemoryModel;
    BYTE bBankSize;
    BYTE bNumberOfImagePages;
    BYTE bReserved;

    BYTE bRedMaskSize;
    BYTE bRedFieldPosition;

    BYTE bGreenMaskSize;
    BYTE bGreenFieldPosition;
    
    BYTE bBlueMaskSize;
    BYTE bBlueFieldPosition;

    BYTE bReservedMaskSize;
    BYTE bReservedFieldPosition;
    BYTE bDirectColorModeInfo;

    //VBE version 2 same field
    DWORD dwPhysicalBasePointer;
    DWORD dwReserved1;
    DWORD dwReserved2;

    //VBE version 3 same field
    WORD wLinearBytesPerScanLine;

    BYTE bBankNumberOfImagePages;
    BYTE bLinearNumberOfImagePages;
    
    BYTE bLinearRedMaskSize;
    BYTE bLinearRedFieldPosition;
    BYTE bLinearGreenMaskSize;
    BYTE bLinearGreenFieldPosition;
    BYTE bLinearBlueMaskSize;
    BYTE bLinearBlueFieldPosition;
    BYTE bLinearReservedMaskSize;
    BYTE bLinearReservedFieldPosition;
    DWORD dwMaxPixelClock;

    BYTE vbReserved[189];
}VBEMODEINFOBLOCK;

#pragma pack(pop)

VBEMODEINFOBLOCK* kGetVBEModeInfoBlock(void);



#endif