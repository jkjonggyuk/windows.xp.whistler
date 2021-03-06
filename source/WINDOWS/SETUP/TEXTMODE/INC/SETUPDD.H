/*++

Copyright (c) 1993 Microsoft Corporation

Module Name:

    setupdd.h

Abstract:

    Public header file for setup device driver.

Author:

    Ted Miller (tedm) 11-August-1993

Revision History:

--*/


#ifndef _SETUPDD_
#define _SETUPDD_


#define DD_SETUP_DEVICE_NAME_U  L"\\Device\\Setup"


#define IOCTL_SETUP_START       CTL_CODE(FILE_DEVICE_UNKNOWN,0,METHOD_BUFFERED,FILE_ANY_ACCESS)


typedef struct _SETUP_COMMUNICATION {

    union {
        ULONG    RequestNumber;
        NTSTATUS Status;
    } u;

    UCHAR Buffer[2048];

} SETUP_COMMUNICATION, *PSETUP_COMMUNICATION;


//
// Input structure for IOCTL_SETUP_START.
//

typedef struct _SETUP_START_INFO {

    //
    // Handles of events used for communication between
    // device driver and user-mode parts of text setup.
    //
    HANDLE RequestReadyEvent;
    HANDLE RequestServicedEvent;

    //
    // Base address of the user-mode process.
    // This is used by the device driver to load massages
    // from the user-mode process' resource tables.
    //
    PVOID UserModeImageBase;

    //
    // System information structure.
    //
    SYSTEM_BASIC_INFORMATION SystemBasicInfo;

    //
    // Address of a buffer in the user process' address space,
    // to be used for same communication.
    //
    PSETUP_COMMUNICATION Communication;

} SETUP_START_INFO, *PSETUP_START_INFO;


//
// Input structure for IOCTL_SETUP_DISPLAY.
//


typedef enum {
    SetupServiceDone,
    SetupServiceExecute,
    SetupServiceLoadKey,
    SetupServiceUnloadKey,
    SetupServiceQueryDirectoryObject,
    SetupServiceFlushVirtualMemory,
    SetupServiceShutdownSystem,
    SetupServiceDeleteKey,
    SetupServiceSaveKey,
    SetupServiceLoadKbdLayoutDll,
    SetupServiceLockVolume,
    SetupServiceUnlockVolume,
    SetupServiceDismountVolume,
    SetupServiceSetDefaultFileSecurity,
    SetupServiceVerifyFileAccess,
    SetupServiceMax
};


typedef struct _SERVICE_EXECUTE {

    PWSTR FullImagePath;
    PWSTR CommandLine;
    ULONG ReturnStatus;

    //
    // The two nul-terminated strings follow in the buffer.
    //
    WCHAR Buffer[1];

} SERVICE_EXECUTE, *PSERVICE_EXECUTE;


typedef struct _SERVICE_LOAD_KEY {

    HANDLE TargetKeyRootDirectory;
    HANDLE SourceFileRootDirectory;

    PWSTR  TargetKey;
    PWSTR  FileName;        // only used for SetupServiceLoadKey

    //
    // The two nul-terminated strings follow in the buffer.
    //
    WCHAR Buffer[1];

} SERVICE_LOAD_KEY, *PSERVICE_LOAD_KEY;

typedef struct _SERVICE_DELETE_KEY {

    HANDLE KeyRootDirectory;
    PWSTR  Key;

    //
    // The nul-terminated string follows in the buffer.
    //
    WCHAR Buffer[1];

} SERVICE_DELETE_KEY, *PSERVICE_DELETE_KEY;

typedef struct _SERVICE_SAVE_KEY {

    HANDLE KeyHandle;
    HANDLE FileHandle;

} SERVICE_SAVE_KEY, *PSERVICE_SAVE_KEY;


typedef struct _SERVICE_QUERY_DIRECTORY_OBJECT {

    HANDLE  DirectoryHandle;
    ULONG   Context;
    BOOLEAN RestartScan;

    //
    // Make sure this fits within the Buffer field of SETUP_COMMUNICATION.
    // It's an arroy of ULONGs to force alignment.
    //
    ULONG  Buffer[256];

} SERVICE_QUERY_DIRECTORY_OBJECT, *PSERVICE_QUERY_DIRECTORY_OBJECT;


typedef struct _SERVICE_FLUSH_VIRTUAL_MEMORY {

    IN PVOID BaseAddress;
    IN ULONG RangeLength;

} SERVICE_FLUSH_VIRTUAL_MEMORY, *PSERVICE_FLUSH_VIRTUAL_MEMORY;


typedef struct _SERVICE_LOAD_KBD_LAYOUT_DLL {

    PVOID TableAddress;
    WCHAR DllName[1];

} SERVICE_LOAD_KBD_LAYOUT_DLL, *PSERVICE_LOAD_KBD_LAYOUT_DLL;

typedef struct _SERVICE_LOCK_UNLOCK_VOLUME {

    HANDLE Handle;

} SERVICE_LOCK_UNLOCK_VOLUME, *PSERVICE_LOCK_UNLOCK_VOLUME;

typedef struct _SERVICE_DISMOUNT_VOLUME {

    HANDLE Handle;

} SERVICE_LOCK_DISMOUNT_VOLUME, *PSERVICE_DISMOUNT_VOLUME;

typedef struct _SERVICE_VERIFY_FILE_ACESS {

    ACCESS_MASK DesiredAccess;
    WCHAR       FileName[1];

} SERVICE_VERIFY_FILE_ACCESS, *PSERVICE_VERIFY_FILE_ACCESS;

typedef struct _SERVICE_DEFAULT_FILE_SECURITY {

    WCHAR FileName[1];

} SERVICE_DEFAULT_FILE_SECURITY, *PSERVICE_DEFAULT_FILE_SECURITY;

#endif // ndef _SETUPDD_
