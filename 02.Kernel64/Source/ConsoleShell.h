#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "Types.h"

#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT  300
#define CONSOLESHELL_PROMPTMESSAGE          "13u9> "

typedef void (* CommandFunction) (const char* pcParameter);

#pragma pack(push, 1)

typedef struct kShellCommandEntryStruct
{
    // char *pcComand;
    char *pcCommand;

    char *pcHelp;

    CommandFunction pfFunction;
}SHELLCOMMANDENTRY;

typedef struct kParameterListStruct
{
    const char *pcBuffer;
    int iLength;
    int iCurrentPosition;
}PARAMETERLIST;

#pragma pack(pop)

void kStartConsoleShell(void);
void kExecuteCommand(const char *pcCommandBuffer);
void kInitializeParameter(PARAMETERLIST *pstList, const char *pcParameter);
int kGetNextParameter(PARAMETERLIST *pstList, char *pcParameter);

static void kHelp(const char* pcParameterBuffer);
static void kCls(const char* pcParameterBuffer);
static void kShowTotalRAMSize(const char* pcParameterBuffer);
static void kStringToDecimalHexTest(const char* pcParameterBuffer);
static void kShutdown(const char* pcParameterBuffer);
static void kSetTimer(const char* pcParameterBuffer);
static void kWaitUsingPIT(const char* pcParameterBuffer);
static void kReadTimeStampCounter(const char* pcParameterBuffer);
static void kMeasureProcessorSpeed(const char* pcParameterBuffer);
static void kShowDateAndTime(const char* pcParameterBuffer);
static void kCreateTestTask(const char* pcParameterBuffer);
static void kChangeTaskPriority(const char* pcParameterBuffer);
static void kShowTaskList(const char* pcParameterBuffer);
static void kKillTask(const char* pcParameterBuffer);
static void kCPULoad(const char* pcParameterBuffer);
static void kPrintNumberTask(void);
static void kTestMutex(const char* pcParameterBuffer);
static void kCreateThreadTask(void);
static void kTestThread(const char* pcParameterBuffer);
static void kShowMatrix(const char* pcParameterBuffer);
static void kTestPIE(const char* pcParameterBuffer);

#endif