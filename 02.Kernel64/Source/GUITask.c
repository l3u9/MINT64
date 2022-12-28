#include "GUITask.h"
#include "Console.h"
#include "Utility.h"
#include "Window.h"
#include "MultiProcessor.h"
#include "Font.h"

void kBaseGUITask(void) {
  QWORD qwWindowID;
  int iMouseX, iMouseY;
  int iWindowWidth, iWindowHeight;
  EVENT stReceivedEvent;
  MOUSEEVENT *pstMouseEvent;
  KEYEVENT *pstKeyEvent;
  WINDOWEVENT *pstWindowEvent;

  if (kIsGraphicMode() == FALSE) {

    kPrintf("This task can run only GUI mode~!!!\n");
    return;
  }

  kGetCursorPosition(&iMouseX, &iMouseY);

  iWindowWidth = 500;
  iWindowHeight = 200;

  qwWindowID = kCreateWindow(iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2,
                             iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT,
                             "Hello World Window");

  if (qwWindowID == WINDOW_INVALIDID)
    return;

  while (1) {

    if (kReceiveEventFromWindowQueue(qwWindowID, &stReceivedEvent) == FALSE) {
      kSleep(0);
      continue;
    }

    switch (stReceivedEvent.qwType) {

    case EVENT_MOUSE_MOVE:
    case EVENT_MOUSE_LBUTTONDOWN:
    case EVENT_MOUSE_LBUTTONUP:
    case EVENT_MOUSE_RBUTTONDOWN:
    case EVENT_MOUSE_RBUTTONUP:
    case EVENT_MOUSE_MBUTTONDOWN:
    case EVENT_MOUSE_MBUTTONUP:

      pstMouseEvent = &(stReceivedEvent.stMouseEvent);
      break;

    case EVENT_KEY_DOWN:
    case EVENT_KEY_UP:

      pstKeyEvent = &(stReceivedEvent.stKeyEvent);
      break;

    case EVENT_WINDOW_SELECT:
    case EVENT_WINDOW_DESELECT:
    case EVENT_WINDOW_MOVE:
    case EVENT_WINDOW_RESIZE:
    case EVENT_WINDOW_CLOSE:

      pstWindowEvent = &(stReceivedEvent.stWindowEvent);

      if (stReceivedEvent.qwType == EVENT_WINDOW_CLOSE) {

        kDeleteWindow(qwWindowID);
        return;
      }
      break;

    default:

      break;
    }
  }
}

void kHelloWorldGUITask(void) {
  QWORD qwWindowID;
  int iMouseX, iMouseY;
  int iWindowWidth, iWindowHeight;
  EVENT stReceivedEvent;
  MOUSEEVENT *pstMouseEvent;
  KEYEVENT *pstKeyEvent;
  WINDOWEVENT *pstWindowEvent;
  int iY;
  char vcTempBuffer[50];
  static int s_iWindowCount = 0;

  char *vpcEventString[] = {
      "Unknown",           "MOUSE_MOVE       ", "MOUSE_LBUTTONDOWN",
      "MOUSE_LBUTTONUP  ", "MOUSE_RBUTTONDOWN", "MOUSE_RBUTTONUP  ",
      "MOUSE_MBUTTONDOWN", "MOUSE_MBUTTONUP  ", "WINDOW_SELECT    ",
      "WINDOW_DESELECT  ", "WINDOW_MOVE      ", "WINDOW_RESIZE    ",
      "WINDOW_CLOSE     ", "KEY_DOWN         ", "KEY_UP           "};
  RECT stButtonArea;
  QWORD qwFindWindowID;
  EVENT stSendEvent;
  int i;

  if (kIsGraphicMode() == FALSE) {

    kPrintf("This task can run only GUI mode~!!!\n");
    return;
  }

  kGetCursorPosition(&iMouseX, &iMouseY);

  iWindowWidth = 500;
  iWindowHeight = 200;

  kSPrintf(vcTempBuffer, "Hello World Window %d", s_iWindowCount++);
  qwWindowID = kCreateWindow(iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2,
                             iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT,
                             vcTempBuffer);

  if (qwWindowID == WINDOW_INVALIDID)
    return;

  iY = WINDOW_TITLEBAR_HEIGHT + 10;

  kDrawRect(qwWindowID, 10, iY + 8, iWindowWidth - 10, iY + 70, RGB(0, 0, 0),
            FALSE);
  kSPrintf(vcTempBuffer, "GUI Event Information[Window ID: 0x%Q]", qwWindowID);
  kDrawText(qwWindowID, 20, iY, RGB(0, 0, 0), RGB(255, 255, 255), vcTempBuffer,
            kStrLen(vcTempBuffer));

  kSetRectangleData(10, iY + 80, iWindowWidth - 10, iWindowHeight - 10,
                    &stButtonArea);

  kDrawButton(qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND,
              "User Message Send Button(Up)", RGB(0, 0, 0));

  kShowWindow(qwWindowID, TRUE);

  while (1) {

    if (kReceiveEventFromWindowQueue(qwWindowID, &stReceivedEvent) == FALSE) {
      kSleep(0);
      continue;
    }

    kDrawRect(qwWindowID, 11, iY + 20, iWindowWidth - 11, iY + 69,
              WINDOW_COLOR_BACKGROUND, TRUE);

    switch (stReceivedEvent.qwType) {

    case EVENT_MOUSE_MOVE:
    case EVENT_MOUSE_LBUTTONDOWN:
    case EVENT_MOUSE_LBUTTONUP:
    case EVENT_MOUSE_RBUTTONDOWN:
    case EVENT_MOUSE_RBUTTONUP:
    case EVENT_MOUSE_MBUTTONDOWN:
    case EVENT_MOUSE_MBUTTONUP:

      pstMouseEvent = &(stReceivedEvent.stMouseEvent);

      kSPrintf(vcTempBuffer, "Mouse Event: %s",
               vpcEventString[stReceivedEvent.qwType]);
      kDrawText(qwWindowID, 20, iY + 20, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
                vcTempBuffer, kStrLen(vcTempBuffer));

      kSPrintf(vcTempBuffer, "Data: X = %d, Y = %d, Button = %X",
               pstMouseEvent->stPoint.iX, pstMouseEvent->stPoint.iY,
               pstMouseEvent->bButtonStatus);
      kDrawText(qwWindowID, 20, iY + 40, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
                vcTempBuffer, kStrLen(vcTempBuffer));

      if (stReceivedEvent.qwType == EVENT_MOUSE_LBUTTONDOWN) {

        if (kIsInRectangle(&stButtonArea, pstMouseEvent->stPoint.iX,
                           pstMouseEvent->stPoint.iY) == TRUE) {

          kDrawButton(qwWindowID, &stButtonArea, RGB(79, 204, 11),
                      "User Message Send Button(Down)", RGB(255, 255, 255));
          kUpdateScreenByID(qwWindowID);

          stSendEvent.qwType = EVENT_USER_TESTMESSAGE;
          stSendEvent.vqwData[0] = qwWindowID;
          stSendEvent.vqwData[1] = 0x1234;
          stSendEvent.vqwData[2] = 0x5678;

          for (i = 0; i < s_iWindowCount; i++) {

            kSPrintf(vcTempBuffer, "Hello World Window %d", i);
            qwFindWindowID = kFindWindowByTitle(vcTempBuffer);

            if ((qwFindWindowID != WINDOW_INVALIDID) &&
                (qwFindWindowID != qwWindowID))

              kSendEventToWindow(qwFindWindowID, &stSendEvent);
          }
        }
      }

      else if (stReceivedEvent.qwType == EVENT_MOUSE_LBUTTONUP) {

        kDrawButton(qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND,
                    "User Message Send Button(Up)", RGB(0, 0, 0));
      }
      break;

    case EVENT_KEY_DOWN:
    case EVENT_KEY_UP:

      pstKeyEvent = &(stReceivedEvent.stKeyEvent);

      kSPrintf(vcTempBuffer, "Key Event: %s",
               vpcEventString[stReceivedEvent.qwType]);
      kDrawText(qwWindowID, 20, iY + 20, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
                vcTempBuffer, kStrLen(vcTempBuffer));

      kSPrintf(vcTempBuffer, "Data: Key = %c, Flag = %X",
               pstKeyEvent->bASCIICode, pstKeyEvent->bFlags);
      kDrawText(qwWindowID, 20, iY + 40, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
                vcTempBuffer, kStrLen(vcTempBuffer));
      break;

    case EVENT_WINDOW_SELECT:
    case EVENT_WINDOW_DESELECT:
    case EVENT_WINDOW_MOVE:
    case EVENT_WINDOW_RESIZE:
    case EVENT_WINDOW_CLOSE:

      pstWindowEvent = &(stReceivedEvent.stWindowEvent);

      kSPrintf(vcTempBuffer, "Window Event: %s",
               vpcEventString[stReceivedEvent.qwType]);
      kDrawText(qwWindowID, 20, iY + 20, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
                vcTempBuffer, kStrLen(vcTempBuffer));

      kSPrintf(vcTempBuffer, "Data: X1 = %d, Y1 = %d, X2 = %d, Y2 = %d",
               pstWindowEvent->stArea.iX1, pstWindowEvent->stArea.iY1,
               pstWindowEvent->stArea.iX2, pstWindowEvent->stArea.iY2);
      kDrawText(qwWindowID, 20, iY + 40, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
                vcTempBuffer, kStrLen(vcTempBuffer));

      if (stReceivedEvent.qwType == EVENT_WINDOW_CLOSE) {

        kDeleteWindow(qwWindowID);
        return;
      }
      break;

    default:

      kSPrintf(vcTempBuffer, "Unknown Event: 0x%X", stReceivedEvent.qwType);
      kDrawText(qwWindowID, 20, iY + 20, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
                vcTempBuffer, kStrLen(vcTempBuffer));

      kSPrintf(vcTempBuffer, "Data0 = 0x%Q, Data1 = 0x%Q, Data2 = 0x%Q",
               stReceivedEvent.vqwData[0], stReceivedEvent.vqwData[1],
               stReceivedEvent.vqwData[2]);
      kDrawText(qwWindowID, 20, iY + 40, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
                vcTempBuffer, kStrLen(vcTempBuffer));
      break;
    }

    kShowWindow(qwWindowID, TRUE);
  }
}

void kSystemMonitorTask(void)
{
  QWORD qwWindowID;
  int i;
  int iWindowWidth;
  int iProcessorCount;
  DWORD vdwLastCPULoad[MAXPROCESSORCOUNT];
  int viLastTaskCount[MAXPROCESSORCOUNT];
  QWORD qwLastTickCount;
  EVENT stReceivedEvent;
  WINDOWMANAGER* pstWindowManager;
  WINDOWEVENT* pstWindowEvent;
  BOOL bChanged;
  RECT stScreenArea;
  QWORD qwLastDynamicMemooryUsedSize;
  QWORD qwDynamicMemoryUsedSize;
  QWORD qwTemp;

  if(kIsGraphicMode() == FALSE)
  {
    kPrintf("This task can run only GUI mode\n");
    return;
  }

  kGetScreenArea(&stScreenArea);

  pstWindowManager = kGetWindowManager();

  iProcessorCount = kGetProcessorCount();
  iWindowWidth = iProcessorCount * (SYSTEMMONITOR_PROCESSOR_WIDTH + SYSTEMMONITOR_PROCESSOR_MARGIN) + SYSTEMMONITOR_PROCESSOR_MARGIN;

  qwWindowID = kCreateWindow((stScreenArea.iX2 - iWindowWidth) / 2, 
    (stScreenArea.iY2 - SYSTEMMONITOR_WINDOW_HEIGHT) / 2 , iWindowWidth, 
    SYSTEMMONITOR_WINDOW_HEIGHT, WINDOW_FLAGS_DEFAULT & ~WINDOW_FLAGS_SHOW, "System Monitor");

  if(qwWindowID == WINDOW_INVALIDID)
    return ;
  
  kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 15, iWindowWidth - 5, 
          WINDOW_TITLEBAR_HEIGHT + 15, RGB( 0, 0, 0 ) );
  kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 16, iWindowWidth - 5, 
          WINDOW_TITLEBAR_HEIGHT + 16, RGB( 0, 0, 0 ) );
  kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 17, iWindowWidth - 5, 
          WINDOW_TITLEBAR_HEIGHT + 17, RGB( 0, 0, 0 ) );
  kDrawText( qwWindowID, 9, WINDOW_TITLEBAR_HEIGHT + 8, RGB( 0, 0, 0 ), 
          WINDOW_COLOR_BACKGROUND, "Processor Information", 21 );

  kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 50, 
          iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 50, 
          RGB( 0, 0, 0 ) );
  kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 51, 
              iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 51, 
              RGB( 0, 0, 0 ) );
  kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 52, 
              iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 52, 
              RGB( 0, 0, 0 ) );
  kDrawText( qwWindowID, 9, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 43, 
          RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, "Memory Information", 18 );

  kShowWindow(qwWindowID, TRUE);

  qwLastTickCount = 0;

  kMemSet(vdwLastCPULoad, 0, sizeof(vdwLastCPULoad));
  kMemSet(viLastTaskCount, 0, sizeof(viLastTaskCount));
  qwLastDynamicMemooryUsedSize = 0;

  while(1)
  {
    if(kReceiveEventFromWindowQueue(qwWindowID, &stReceivedEvent) == TRUE)
    {
      switch(stReceivedEvent.qwType)
      {
        case EVENT_WINDOW_CLOSE:
          kDeleteWindow(qwWindowID);
          return;
          break;
        
        default:
          break;
      }
    }

    if((kGetTickCount() - qwLastTickCount) < 500)
    {
      kSleep(1);
      continue;
    }

    qwLastTickCount = kGetTickCount();

    for(i = 0; i < iProcessorCount; i++)
    {
      bChanged = FALSE;

      if(vdwLastCPULoad[i] != kGetProcessorLoad(i))
      {
        vdwLastCPULoad[i] = kGetProcessorLoad(i);
        bChanged = TRUE;
      }
      else if(viLastTaskCount[i] != kGetTaskCount(i))
      {
        viLastTaskCount[i] = kGetTaskCount(i);
        bChanged = TRUE;
      }

      if(bChanged == TRUE)
        kDrawProcessorInformation(qwWindowID, i * SYSTEMMONITOR_PROCESSOR_WIDTH + (i + 1) * SYSTEMMONITOR_PROCESSOR_MARGIN, WINDOW_TITLEBAR_HEIGHT + 28, i);
    }

    kGetDynamicMemoryInformation(&qwTemp, &qwTemp, &qwTemp, &qwDynamicMemoryUsedSize);

    if(qwDynamicMemoryUsedSize != qwLastDynamicMemooryUsedSize)
    {
      qwLastDynamicMemooryUsedSize = qwDynamicMemoryUsedSize;
      kDrawMemoryInformation(qwWindowID, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 60, iWindowWidth);
    }
  }
}



static void kDrawProcessorInformation(QWORD qwWindowID, int iX, int iY, BYTE bAPICID)
{
  char vcBuffer[100];
  RECT stArea;
  QWORD qwProcessorLoad;
  int iUsageBarHeight;
  int iMiddleX; 

  kSPrintf( vcBuffer, "Processor ID: %d", bAPICID );
    kDrawText( qwWindowID, iX + 10, iY, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND,
    vcBuffer, kStrLen( vcBuffer ) );
 
  kSPrintf( vcBuffer, "Task Count: %d ", kGetTaskCount( bAPICID ) );
    kDrawText( qwWindowID, iX + 10, iY + 18, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND,
    vcBuffer, kStrLen( vcBuffer ) );

  qwProcessorLoad = kGetProcessorLoad(bAPICID);
  if(qwProcessorLoad > 100)
    qwProcessorLoad = 100;

  kDrawRect(qwWindowID, iX, iY + 36, iX + SYSTEMMONITOR_PROCESSOR_WIDTH, iY + SYSTEMMONITOR_PROCESSOR_HEIGHT, RGB(0, 0, 0), FALSE);
  
  iUsageBarHeight = (SYSTEMMONITOR_PROCESSOR_HEIGHT - 40) * qwProcessorLoad / 100;

  kDrawRect( qwWindowID, iX + 2,
    iY + ( SYSTEMMONITOR_PROCESSOR_HEIGHT - iUsageBarHeight ) - 2,
    iX + SYSTEMMONITOR_PROCESSOR_WIDTH - 2,
    iY + SYSTEMMONITOR_PROCESSOR_HEIGHT - 2, SYSTEMMONITOR_BAR_COLOR, TRUE );

  kDrawRect( qwWindowID, iX + 2, iY + 38, iX + SYSTEMMONITOR_PROCESSOR_WIDTH - 2,
    iY + ( SYSTEMMONITOR_PROCESSOR_HEIGHT - iUsageBarHeight ) - 1,
    WINDOW_COLOR_BACKGROUND, TRUE );

  kSPrintf(vcBuffer, "Usage: %d%%", qwProcessorLoad);
  iMiddleX = (SYSTEMMONITOR_PROCESSOR_WIDTH - (kStrLen(vcBuffer) * FONT_ENGLISHWIDTH)) /2;
  kDrawText(qwWindowID, iX + iMiddleX, iY + 80, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND, vcBuffer, kStrLen(vcBuffer));
  kSetRectangleData(iX, iY, iX + SYSTEMMONITOR_PROCESSOR_WIDTH, iY + SYSTEMMONITOR_PROCESSOR_HEIGHT, &stArea);
  kUpdateScreenByWindowArea(qwWindowID, &stArea);
}


static void kDrawMemoryInformation(QWORD qwWindowID, int iY, int iWindowWidth)
{
  char vcBuffer[100];
  QWORD qwTotalRAMKbyteSize;
  QWORD qwDynamicMemoryStartAddress;
  QWORD qwDynamicMemoryUsedSize;
  QWORD qwUsedPercent;
  QWORD qwTemp;
  int iUsageBarWidth;
  RECT stArea;
  int iMiddleX;

  qwTotalRAMKbyteSize = kGetTotalRAMSize() * 1024;

  kSPrintf(vcBuffer, "Total Size: %d KB     ", qwTotalRAMKbyteSize);
  kDrawText(qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 10, iY + 3, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND, vcBuffer, kStrLen(vcBuffer));
  kGetDynamicMemoryInformation(&qwDynamicMemoryStartAddress, &qwTemp, &qwTemp, &qwDynamicMemoryUsedSize);
  
  kSPrintf(vcBuffer, "Used Size: %d KB      ", (qwDynamicMemoryUsedSize + qwDynamicMemoryStartAddress) / 1024);
  kDrawText(qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 10, iY + 21, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND, vcBuffer, kStrLen(vcBuffer));

  kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN, iY + 40,
    iWindowWidth - SYSTEMMONITOR_PROCESSOR_MARGIN,
    iY + SYSTEMMONITOR_MEMORY_HEIGHT - 32, RGB( 0, 0, 0 ), FALSE );

  qwUsedPercent = (qwDynamicMemoryStartAddress + qwDynamicMemoryUsedSize) * 100 / 1024 / qwTotalRAMKbyteSize;

  if(qwUsedPercent > 100)
    qwUsedPercent = 100;
  
  iUsageBarWidth = (iWindowWidth - 2 * SYSTEMMONITOR_PROCESSOR_MARGIN) * qwUsedPercent / 100;

  kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 2, iY + 42,
    SYSTEMMONITOR_PROCESSOR_MARGIN + 2 + iUsageBarWidth,
    iY + SYSTEMMONITOR_MEMORY_HEIGHT - 34, SYSTEMMONITOR_BAR_COLOR, TRUE ); 
  
  kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 2 + iUsageBarWidth,
    iY + 42, iWindowWidth - SYSTEMMONITOR_PROCESSOR_MARGIN - 2,
    iY + SYSTEMMONITOR_MEMORY_HEIGHT - 34, WINDOW_COLOR_BACKGROUND, TRUE ); 
  

  kSPrintf(vcBuffer, "Usage: %d%%", qwUsedPercent);
  iMiddleX = (iWindowWidth - (kStrLen(vcBuffer) * FONT_ENGLISHWIDTH)) / 2;
  
  kDrawText( qwWindowID, iMiddleX, iY + 45, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND,
    vcBuffer, kStrLen( vcBuffer ) );

  kSetRectangleData(0, iY, iWindowWidth, iY + SYSTEMMONITOR_MEMORY_HEIGHT, &stArea);
  kUpdateScreenByWindowArea(qwWindowID, &stArea);

}



