#ifndef __2DGRAPHICS_H__
#define __2DGRAPHICS_H__

#include "Types.h"

typedef WORD                COLOR;

#define RGB(r, g, b)        (((BYTE)(r) >> 3) << 11 | (((BYTE)(g) >> 2)) << 5 | ((BYTE)(b) >> 3))

void kDrawPixel(int iX, int iY, COLOR stColor);
void kDrawLine(int iX1, int iY1, int iX2, int iY2, COLOR stolor);
void kDrawRect(int iX1, int iY1, int iX2, int iY2, COLOR stolor, BOOL bFill);
void kDrawCircle(int iX, int iY, int iRadius, COLOR stColor, BOOL bFill);
void kDrawText(int iX, int iY, COLOR stColor, COLOR stBackgroundColor, const char* pcString, int iLength);

#endif