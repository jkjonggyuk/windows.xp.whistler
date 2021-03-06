/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    beep.h

Abstract:

    These are the structures and defines that are used in the beep driver.

Author:

    Lee A. Smith (lees) 02-Aug-2000.

Revision History:

--*/

#ifndef _BEEP_
#define _BEEP_

#include <ntddbeep.h>

//
// Define the device extension.
//

typedef struct _DEVICE_EXTENSION {

    KTIMER Timer;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#endif // _BEEP_
