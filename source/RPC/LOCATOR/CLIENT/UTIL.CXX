/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    util.cxx

Abstract:

    This module contains utility functions used by the NSI client wrappers.

Author:

    Steven Zeck (stevez) 03/27/92

--*/


#include <nsi.h>

#include <memory.h>
#include <string.h>
#include <stdio.h>

#ifndef NTENV

extern "C" {

int atoi(char *);

void far pascal OutputDebugString(void far *);

}
#else
#include <stdlib.h>

#endif

RPC_STATUS NsiToRpcStatus[] =
{
    RPC_S_OK,                      // NSI_S_OK
    RPC_S_NO_MORE_BINDINGS,        // NSI_S_NO_MORE_BINDINGS
    RPC_S_INTERFACE_NOT_FOUND,     // NSI_S_INTERFACE_NOT_FOUND
    RPC_S_ENTRY_NOT_FOUND,         // NSI_S_ENTRY_NOT_FOUND
    RPC_S_NAME_SERVICE_UNAVAILABLE,// NSI_S_NAME_SERVICE_UNAVAILABLE
    RPC_S_ACCESS_DENIED,           // NSI_S_NO_NS_PRIVILEGE
    RPC_S_UNSUPPORTED_NAME_SYNTAX, // NSI_S_UNSUPPORTED_NAME_SYNTAX
    RPC_S_NOT_ALL_OBJS_UNEXPORTED, // NSI_S_NOTHING_TO_UNEXPORT
    RPC_S_INVALID_NAME_SYNTAX,     // NSI_S_INVALID_NAME_SYNTAX
    RPC_S_NO_CONTEXT_AVAILABLE,    // NSI_S_INVALID_NS_HANDLE
    RPC_S_OUT_OF_RESOURCES,        // NSI_S_INVALID_OBJECT
    RPC_S_NOT_ALL_OBJS_UNEXPORTED, // NSI_S_NOT_ALL_OBJS_UNEXPORTED
    RPC_S_INVALID_STRING_BINDING,  // NSI_S_INVALID_STRING_BINDING
    RPC_S_INTERNAL_ERROR,          // NSI_S_SOME_OTHER_ERROR
    RPC_S_NOTHING_TO_EXPORT,       // NSI_S_NOTHING_TO_EXPORT
    RPC_S_CANNOT_SUPPORT,          // NSI_S_UNIMPLEMENTED_API
    RPC_S_NOTHING_TO_EXPORT,       // NSI_S_NO_INTERFACES_EXPORTED
    RPC_S_INCOMPLETE_NAME,         // NSI_S_INCOMPLETE_NAME
    RPC_S_INVALID_VERS_OPTION,     // NSI_S_INVALID_VERS_OPTION
    RPC_S_NO_MORE_MEMBERS,         // NSI_S_NO_MORE_MEMBERS
    RPC_S_ENTRY_ALREADY_EXISTS,    // NSI_S_ENTRY_ALREADY_EXISTS
    RPC_S_OUT_OF_MEMORY,           // NSI_S_OUT_OF_MEMORY
    RPC_S_GROUP_MEMBER_NOT_FOUND,  // NSI_S_GROUP_MEMBER_NOT_FOUND
    RPC_S_SERVER_UNAVAILABLE       // NSI_S_NO_MASTER_LOCATOR
};



UNSIGNED16
MapException(
    IN RPC_STATUS Exception
    )
/*++

Routine Description:

    Map a RPC exception to a NSI status code.

Arguments:

    Exception - Stub generated exception code.

Returns:

    NSI status code
--*/
{

#ifdef NTENV

    // If an NT access fault was raised, re-raise it.

    if (Exception == 0xc0000005)
        RpcRaiseException(Exception);
#endif

    switch(Exception)
    {
      case RPC_X_SS_CONTEXT_MISMATCH:
      case RPC_X_SS_IN_NULL_CONTEXT:
        return(NSI_S_INVALID_NS_HANDLE);

      case RPC_S_OUT_OF_MEMORY:
        return(NSI_S_OUT_OF_MEMORY);

      case RPC_X_ENUM_VALUE_OUT_OF_RANGE:
        return (NSI_S_UNIMPLEMENTED_API);
    }

    return(NSI_S_NAME_SERVICE_UNAVAILABLE);
}


WIDE_STRING::WIDE_STRING(
    IN unsigned char * AsciiString OPTIONAL
    )

/*++

Routine Description:

    Make a UNICODE string from an ASCII string.

Arguments:

    AsciiString - 8 bit string to widen to 16 bits.

Returns:

    String is set to new value.  If there was an allocation error, then
    the value is set to AllocError.

--*/
{
    if (!AsciiString)
        {
        String = 0;
        AllocMode = AllocReference;
        return;
        }

    int Size = (strlen((CONST_CHAR *) AsciiString) + 1) * sizeof(unsigned short);

    if (! (String = (unsigned short *)I_RpcAllocate(Size)))
        {
        AllocMode = AllocError;
        return;
        }

    AllocMode = AllocMemory;

#ifdef NTENV
    AsciiToUnicodeNT(String, AsciiString);
#else
    for (unsigned short *pT = String; *pT++ = *AsciiString++;) ;
#endif

}

#ifndef NTENV

int
UnicodeToAscii(
    unsigned short *UnicodeString
    )
/*++

Routine Description:

    Make a  ASCII string from an UNICODE string.  This is done in
    place so the string becomes ASCII.

Arguments:

    UnicodeString - unicode string to convert

Returns:

    1 if conversion was OK, 0 if there was an error.

--*/
{
    unsigned char * AsciiString;

    for (AsciiString = (unsigned char *) UnicodeString;
         *UnicodeString <= 0xff && *UnicodeString; )

         *AsciiString++ = *UnicodeString++;

    *AsciiString = 0;
    return((*UnicodeString == 0)? RPC_S_OK: RPC_S_OK);
}

#endif


void *  
#if defined(NTENV)
_CRTAPI1
#endif 
operator new(
        size_t size
        )
{

    return(I_RpcAllocate(size));
}

void  
#if defined(NTENV)
_CRTAPI1
#endif
operator delete(
           void * p
           )
{

    I_RpcFree(p);
}

extern "C" {


#if defined(DOS) && !defined(WIN)

typedef void __RPC_FAR * (__RPC_FAR * __RPC_API MIDL_USER_ALLOCATE)(size_t);
typedef void (__RPC_FAR * __RPC_API MIDL_USER_FREE)(void __RPC_FAR *);

extern __RPC_API __loadds
I_NsGetMemoryAllocator(void __RPC_FAR *, void __RPC_FAR *);

static MIDL_USER_ALLOCATE pMidlUserAlloc;
static MIDL_USER_FREE pMidlUserFree;

int _NsAllocatorInitialized = 0;

void __RPC_API
NsAllocatorSetup()
/*++

Routine Description:

    BUGBUG    

--*/
{
    I_NsGetMemoryAllocator((void __RPC_FAR *)&pMidlUserAlloc,
                           (void __RPC_FAR *)&pMidlUserFree);

    _NsAllocatorInitialized = 1;

    return;
}
#endif


void __RPC_FAR * __RPC_API
MIDL_user_allocate(
    size_t cb
    )
/*++

Routine Description:

    Call the C memory allocation for MIDL.

--*/
{
    void *p;

#if defined(DOS) && !defined(WIN)
    ASSERT(_NsAllocatorInitialized);

    p = (*pMidlUserAlloc)(cb);
#else
    p = I_RpcAllocate(cb);
#endif

    return(p);
}

void __RPC_API
MIDL_user_free(
    void __RPC_FAR * p
    )
{
#if defined(DOS) && !defined(WIN)
    ASSERT(_NsAllocatorInitialized);

    (*pMidlUserFree)(p);
#else
    I_RpcFree(p);
#endif
}

void CallExportInit() {}


unsigned char *
CopyString(
    IN char * String
    )

/*++

Routine Description:

    Copy a string.

Arguments:

    String - to copy

Returns:

    pointer to a copy, Nil if out of memory.

--*/
{
    unsigned char * pReturn;

    if (!String || !( pReturn = new unsigned char [strlen(String)+1]))
        return(0);

    return ((unsigned char *) strcpy((char *) pReturn, (CONST_CHAR *) String));
}

} // extern "C"


void
GetDefaultEntrys(
    IN void * Key
    )

/*++

Routine Description:

    Get the default Entry name and syntax type form the registry.

Arguments:

    Key - open registry handle

--*/
{
    // While we have the registry open, get the default syntax entrys.

    if (! fSyntaxDefaultsLoaded)
        {
        unsigned char *SyntaxValue;

        if (SyntaxValue = RegGetString(Key, "DefaultSyntax"))
            {
            DefaultSyntax = atoi((char *)SyntaxValue);
            delete SyntaxValue;
            }

        if (SyntaxValue = RegGetString(Key, "DefaultEntry"))
            {
            DefaultName = new WIDE_STRING(SyntaxValue);
            delete SyntaxValue;
            }

        fSyntaxDefaultsLoaded = 1;
        }
}


#if !defined(NTENV) && DBG


void
RtlAssert(
    void * FailedAssertion,
    void * FileName,
    unsigned long LineNumber,
    char * Message
    )
{
    UNUSED(Message);

#if defined(OS212)

    printf("Assert failed %s in %s line %ld\n",
        FailedAssertion, FileName, LineNumber);

#elif defined(WIN)


    char Buffer[300];
    sprintf(Buffer, "Assert failed %s in %s line %ld\n",
        FailedAssertion, FileName, LineNumber);

    OutputDebugString(Buffer);

#endif

}

#endif

