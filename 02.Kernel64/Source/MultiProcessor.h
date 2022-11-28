#ifndef __MULTIPROCESSOR_H__
#define __MULTIPROCESSOR_H__

#include "Types.h"
#include "MPConfigurationTable.h"
#include "IOAPIC.h"

#define BOOTSTRAPPROCESSOR_FLAGADDRESS      0x7C09
#define MAXPROCESSORCOUNT                   16

BOOL kStartUpApplicationProcessor(void);
BYTE kGetAPICID(void);
static BOOL kWakeUpApplicationProcessor(void);

#endif