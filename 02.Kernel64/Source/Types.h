#pragma once

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long
#define BOOL unsigned char

#define TRUE 1
#define FALSE 0
#define NULL 0

#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)
#define BTOKB(B) B / 1024
#define BTOMB(B) B / 1024 / 1024

#pragma pack(push, 1)

typedef struct kCharactorStruct {
  BYTE bCharactor;
  BYTE bAttribute;
} CHARACTER;

#pragma pack(pop)