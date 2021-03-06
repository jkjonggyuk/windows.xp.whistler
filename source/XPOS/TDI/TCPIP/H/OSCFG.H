/********************************************************************/
/**                     Microsoft LAN Manager                      **/
/**               Copyright(c) Microsoft Corp., 1990-2000          **/
/********************************************************************/
/* :ts=4 */

#ifndef OSCFG_INCLUDED
#define OSCFG_INCLUDED


#define net_short(x) ((((x)&0xff) << 8) | (((x)&0xff00) >> 8))

//#define net_long(x) (((net_short((x)&0xffff)) << 16) | net_short((((x)&0xffff0000L)>>16)))
#define net_long(x) (((((ulong)(x))&0xffL)<<24) | \
                     ((((ulong)(x))&0xff00L)<<8) | \
                     ((((ulong)(x))&0xff0000L)>>8) | \
                     ((((ulong)(x))&0xff000000L)>>24))

#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#define MAX(a,b)    ((a) > (b) ? (a) : (b))


#ifdef  VXD
/////////////////////////////////////////////////////////////////////////////
//
// VXD definitions
//
////////////////////////////////////////////////////////////////////////////

#include <stddef.h>

#pragma code_seg("_LTEXT", "LCODE")
#pragma data_seg("_LDATA", "LCODE")

//* pragma bodies for bracketing of initialization code.

#define BEGIN_INIT  code_seg("_ITEXT", "ICODE")
#define BEGIN_INIT_DATA data_seg("_IDATA", "ICODE")
#define END_INIT    code_seg()
#define END_INIT_DATA data_seg()

#else // VXD
#ifdef NT

//////////////////////////////////////////////////////////////////////////////
//
// NT definitions
//
//////////////////////////////////////////////////////////////////////////////

#include <ntddk.h>

#define BEGIN_INIT
#define END_INIT

#else // NT

/////////////////////////////////////////////////////////////////////////////
//
// Definitions for additional environments go here
//
/////////////////////////////////////////////////////////////////////////////

#error Environment specific definitions missing

#endif // NT

#endif  // VXD


#endif // OSCFG_INCLUDED

