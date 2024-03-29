#pragma once

#include "Types.h"

typedef WORD COLOR;

#define RGB(r, g, b)                                                           \
  (((BYTE)(r) >> 3) << 11 | (((BYTE)(g) >> 2)) << 5 | ((BYTE)(b) >> 3))

typedef struct kRectangleStruct {
  int iX1;
  int iY1;

  int iX2;
  int iY2;
} RECT;

typedef struct kPointStruct {
  int iX;
  int iY;
} POINT;

inline void kInternalDrawPixel(const RECT *pstMemoryArea,
                               COLOR *pstMemoryAddress, int iX, int iY,
                               COLOR stColor);
void kInternalDrawLine(const RECT *pstMemoryArea, COLOR *pstMemoryAddress,
                       int iX1, int iY1, int iX2, int iY2, COLOR stColor);
void kInternalDrawRect(const RECT *pstMemoryArea, COLOR *pstMemoryAddress,
                       int iX1, int iY1, int iX2, int iY2, COLOR stColor,
                       BOOL bFill);
void kInternalDrawCircle(const RECT *pstMemoryArea, COLOR *pstMemoryAddress,
                         int iX, int iY, int iRadius, COLOR stColor,
                         BOOL bFill);
void kInternalDrawText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor, 
        const char* pcString, int iLength );
void kInternalDrawEnglishText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
        const char* pcString, int iLength );
void kInternalDrawHangulText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
        const char* pcString, int iLength );

                  
inline BOOL kIsInRectangle(const RECT *pstArea, int iX, int iY);
inline int kGetRectangleWidth(const RECT *pstArea);
inline int kGetRectangleHeight(const RECT *pstArea);
inline void kSetRectangleData(int iX1, int iY1, int iX2, int iY2,
                              RECT *pstRect);
inline BOOL kGetOverlappedRectangle(const RECT *pstArea1, const RECT *pstArea2,
                                    RECT *pstIntersection);
inline BOOL kIsRectangleOverlapped(const RECT *pstArea1, const RECT *pstArea2);