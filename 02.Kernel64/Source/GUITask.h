#pragma once

#include "Types.h"
#include "Window.h"

#define EVENT_USER_TESTMESSAGE 0x80000001

#define SYSTEMMONITOR_PROCESSOR_WIDTH       150
#define SYSTEMMONITOR_PROCESSOR_MARGIN      20
#define SYSTEMMONITOR_PROCESSOR_HEIGHT      150
#define SYSTEMMONITOR_WINDOW_HEIGHT         310
#define SYSTEMMONITOR_MEMORY_HEIGHT         100
#define SYSTEMMONITOR_BAR_COLOR             RGB(55, 215, 47)

void kBaseGUITask();
void kHelloWorldGUITask();

void kSystemMonitorTask(void);
static void kDrawProcessorInformation(QWORD qwWindowID, int iX, int iY, BYTE bAPICID);
static void kDrawMemoryInformation(QWORD qwWindowID, int iY, int iWindowWidth);

void kGUIConsoleShellTask(void);
static void kProcessConsoleBuffer(QWORD qwWindowID);

void kImageViewerTask(void);
static void kDrawFileName(QWORD qwWindowID, RECT* pstArea, char* pcFileName, int iNameLength);
static BOOL kCreateImageViewerWindowAndExecute(QWORD qwMainWindowID, const char* pcFileName);
