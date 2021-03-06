/***************************************************************************\
* Module Name: debug.h
*
* Commonly used debugging macros.
*
* Copyright (c) 2000 Microsoft Corporation
\***************************************************************************/

extern
VOID
DebugPrint(
    ULONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    );

// if we are in a debug environment, macros should

#if DBG
#define DISPDBG(arg) DebugPrint arg
#define RIP(x) { DebugPrint(0, x); DebugBreak();}
#define ASSERTQV(x,y)  if(!(x)) RIP(y)

// if we are not in a debug environment, we want all of the debug
// information to be stripped out.

#else
#define DISPDBG(arg)
#define RIP(x)
#define ASSERTQV(x,y)

#endif
