/****************************** Module Header ******************************\
* Module Name: pipe.h
*
* Copyright (c) 2000, Microsoft Corporation
*
* Defines functions exported by pipe.c
*
* History:
* 06-29-92 Davidc       Created.
\***************************************************************************/


//
// Function prototypes
//

BOOL
MyCreatePipe(
    LPHANDLE ReadHandle,
    LPHANDLE WriteHandle,
    LPSECURITY_ATTRIBUTES SecurityAttributes,
    DWORD Size,
    DWORD Timeout,
    DWORD ReadHandleFlags,
    DWORD WriteHandleFlags
    );

DWORD
ReadPipe(
    HANDLE PipeHandle,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    DWORD Timeout
    );
