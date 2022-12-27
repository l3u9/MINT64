#ifndef __WINDOWMANAGERTASK_H__
#define __WINDOWMANAGERTASK_H__

#include "Types.h"

#define WINDOWMANAGER_DATAACCUMULATECOUNT       20

void kStartWindowManager(void);
BOOL kProcessMouseData(void);
BOOL kProcessKeyData(void);
BOOL kProcessEventQueueData(void);


#endif