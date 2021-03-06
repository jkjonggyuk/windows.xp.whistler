/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    svcxport.c

Abstract:

    This module contains routines for supporting the transport APIs in the
    server service, NetServerTransportAdd, NetServerTransportDel,
    and NetServerTransportEnum.

Author:

    David Treadwell (davidtr) 6-Mar-2000

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

//
// Forward declarations.
//

VOID
FillTransportInfoBuffer (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Block,
    IN OUT PVOID *FixedStructure,
    IN LPTSTR *EndOfVariableData
    );

BOOLEAN
FilterTransports (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Block
    );

ULONG
SizeTransports (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Block
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text( PAGE, SrvNetServerTransportAdd )
#pragma alloc_text( PAGE, SrvNetServerTransportDel )
#pragma alloc_text( PAGE, SrvNetServerTransportEnum )
#pragma alloc_text( PAGE, FillTransportInfoBuffer )
#pragma alloc_text( PAGE, FilterTransports )
#pragma alloc_text( PAGE, SizeTransports )
#endif

//
// Macros to determine the size a transport would take up in the output
// buffer.
//

#define TOTAL_SIZE_OF_TRANSPORT(endpoint)                                  \
    ( sizeof(SERVER_TRANSPORT_INFO_0) +                                    \
          SrvLengthOfStringInApiBuffer(&(endpoint)->TransportName) +       \
          (endpoint)->TransportAddress.Length + sizeof(TCHAR) +            \
          SrvLengthOfStringInApiBuffer(&(endpoint)->NetworkAddress) )

#define FIXED_SIZE_OF_TRANSPORT sizeof(SERVER_TRANSPORT_INFO_0)


NTSTATUS
SrvNetServerTransportAdd (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Buffer,
    IN ULONG BufferLength
    )

/*++

Routine Description:

    This routine processes the NetServerTransportAdd API in the server
    FSP.  Because it opens an object (the transpot device object) it
    must be done in the server FSP, not the FSD.

Arguments:

    Srp - a pointer to the server request packet that contains all
        the information necessary to satisfy the request.  This includes:

      INPUT:

        None.

      OUTPUT:

        None.

    Buffer - a pointer to a TRANSPORT_INFO_0 structure for the new
        transport.  All pointers should have been changed to offsets
        within the buffer.

    BufferLength - total length of this buffer.

Return Value:

    NTSTATUS - result of operation to return to the server service.

--*/

{
    NTSTATUS status;
    PSERVER_TRANSPORT_INFO_0 svti0;
    UNICODE_STRING transportName;
    ANSI_STRING transportAddress;
    UNICODE_STRING netName;

    PAGED_CODE( );

    Srp;

    //
    // Convert the offsets in the transport data structure to pointers.
    // Also make sure that all the pointers are within the specified
    // buffer.
    //

    svti0 = Buffer;

    OFFSET_TO_POINTER( svti0->svti0_transportname, svti0 );
    OFFSET_TO_POINTER( svti0->svti0_transportaddress, svti0 );

    if ( !POINTER_IS_VALID( svti0->svti0_transportname, svti0, BufferLength ) ||
         !POINTER_IS_VALID( svti0->svti0_transportaddress, svti0, BufferLength ) ) {
        return STATUS_ACCESS_VIOLATION;
    }

    //
    // Set up the transport name, server name, and net name.
    //

    RtlInitUnicodeString( &transportName, (PWCH)svti0->svti0_transportname );
    netName.Buffer = NULL;
    netName.Length = 0;
    netName.MaximumLength = 0;

    transportAddress.Buffer = svti0->svti0_transportaddress;
    transportAddress.Length = (USHORT)svti0->svti0_transportaddresslength;
    transportAddress.MaximumLength = (USHORT)svti0->svti0_transportaddresslength;

    //
    // Attempt to add the new transport to the server.
    //

    status = SrvAddServedNet( &netName, &transportName, &transportAddress );

    return status;

} // SrvNetServerTransportAdd


NTSTATUS
SrvNetServerTransportDel (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Buffer,
    IN ULONG BufferLength
    )

/*++

Routine Description:

    This routine processes the NetServerTransportEnum API in the server
    FSD.

Arguments:

    Srp - a pointer to the server request packet that contains all
        the information necessary to satisfy the request.  This includes:

      INPUT:

        None.

      OUTPUT:

        None.

    Buffer - a pointer to a TRANSPORT_INFO_0 structure for the new
        transport.  All pointers should have been changed to offsets
        within the buffer.

    BufferLength - total length of this buffer.

Return Value:

    NTSTATUS - result of operation to return to the server service.

--*/

{
    NTSTATUS status;
    PSERVER_TRANSPORT_INFO_0 svti0;
    UNICODE_STRING transportName;
    ANSI_STRING transportAddress;

    PAGED_CODE( );

    Srp;

    //
    // Convert the offsets in the transport data structure to pointers.
    // Also make sure that all the pointers are within the specified
    // buffer.
    //

    svti0 = Buffer;

    OFFSET_TO_POINTER( svti0->svti0_transportname, svti0 );
    OFFSET_TO_POINTER( svti0->svti0_transportaddress, svti0 );

    if ( !POINTER_IS_VALID( svti0->svti0_transportname, svti0, BufferLength ) ||
         !POINTER_IS_VALID( svti0->svti0_transportaddress, svti0, BufferLength ) ) {
        return STATUS_ACCESS_VIOLATION;
    }

    //
    // Set up the transport name, server name, and net name.
    //

    RtlInitUnicodeString( &transportName, (PWCH)svti0->svti0_transportname );

    transportAddress.Buffer = svti0->svti0_transportaddress;
    transportAddress.Length = (USHORT)svti0->svti0_transportaddresslength;
    transportAddress.MaximumLength = (USHORT)svti0->svti0_transportaddresslength;

    //
    // Attempt to delete the transport endpoint from the server.
    //

    status = SrvDeleteServedNet( &transportName, &transportAddress );

    return status;

} // SrvNetServerTransportDel


NTSTATUS
SrvNetServerTransportEnum (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Buffer,
    IN ULONG BufferLength
    )

/*++

Routine Description:

    This routine processes the NetServerTransportEnum API in the server
    FSD.

Arguments:

    Srp - a pointer to the server request packet that contains all
        the information necessary to satisfy the request.  This includes:

      INPUT:

        None.

      OUTPUT:

        Parameters.Get.EntriesRead - the number of entries that fit in
            the output buffer.

        Parameters.Get.TotalEntries - the total number of entries that
            would be returned with a large enough buffer.

        Parameters.Get.TotalBytesNeeded - the buffer size that would be
            required to hold all the entries.

    Buffer - a pointer to a TRANSPORT_INFO_0 structure for the new
        transport.  All pointers should have been changed to offsets
        within the buffer.

    BufferLength - total length of this buffer.

Return Value:

    NTSTATUS - result of operation to return to the server service.

--*/

{
    PAGED_CODE( );

    return SrvEnumApiHandler(
               Srp,
               Buffer,
               BufferLength,
               &SrvEndpointList,
               FilterTransports,
               SizeTransports,
               FillTransportInfoBuffer
               );

} // SrvNetServerTransportEnum


VOID
FillTransportInfoBuffer (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Block,
    IN OUT PVOID *FixedStructure,
    IN LPTSTR *EndOfVariableData
    )

/*++

Routine Description:

    This routine puts a single fixed transport structure and, if it fits,
    associated variable data, into a buffer.  Fixed data goes at the
    beginning of the buffer, variable data at the end.

Arguments:

    Endpoint - the endpoint from which to get information.

    FixedStructure - where the ine buffer to place the fixed structure.
        This pointer is updated to point to the next available
        position for a fixed structure.

    EndOfVariableData - the last position on the buffer that variable
        data for this structure can occupy.  The actual variable data
        is written before this position as long as it won't overwrite
        fixed structures.  It is would overwrite fixed structures, it
        is not written.

Return Value:

    None.

--*/

{
    PENDPOINT endpoint = Block;
    PSERVER_TRANSPORT_INFO_0 svti0 = *FixedStructure;

    PAGED_CODE( );

    Srp;

    //
    // Update FixedStructure to point to the next structure location.
    //

    *FixedStructure = (PCHAR)*FixedStructure + FIXED_SIZE_OF_TRANSPORT;
    ASSERT( (ULONG)*EndOfVariableData >= (ULONG)*FixedStructure );

    //
    // The number of VCs on the endpoint is equal to the total number
    // of connections on the endpoint less the free connections.
    //

    ACQUIRE_LOCK( &SrvEndpointLock );

    svti0->svti0_numberofvcs =
        endpoint->TotalConnectionCount - endpoint->FreeConnectionCount;

    RELEASE_LOCK( &SrvEndpointLock );

    //
    // Copy over the transport name.
    //

    SrvCopyUnicodeStringToBuffer(
        &endpoint->TransportName,
        *FixedStructure,
        EndOfVariableData,
        &svti0->svti0_transportname
        );

    //
    // Copy over the transport address.  We have to manually check here
    // whether it will fit in the output buffer.
    //

    *EndOfVariableData = (LPTSTR)( (PCHAR)*EndOfVariableData -
                                      endpoint->TransportAddress.Length );

    if ( (ULONG)*EndOfVariableData > (ULONG)*FixedStructure ) {

        //
        // The address will fit.  Copy it over to the output buffer.
        //

        RtlCopyMemory(
            *EndOfVariableData,
            endpoint->TransportAddress.Buffer,
            endpoint->TransportAddress.Length
            );

        svti0->svti0_transportaddress = (LPBYTE)*EndOfVariableData;
        svti0->svti0_transportaddresslength = endpoint->TransportAddress.Length;

    } else {

        svti0->svti0_transportaddress = NULL;
        svti0->svti0_transportaddresslength = 0;
    }

    //
    // Copy over the network name.
    //

    SrvCopyUnicodeStringToBuffer(
        &endpoint->NetworkAddress,
        *FixedStructure,
        EndOfVariableData,
        &svti0->svti0_networkaddress
        );

    return;

} // FillTransportInfoBuffer


BOOLEAN
FilterTransports (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Block
    )

/*++

Routine Description:

    This routine just returns TRUE since we always want to place
    information about all transports in the output buffer for a
    NetServerTransportEnum.

Arguments:

    Srp - not used.

    Block - not used.

Return Value:

    TRUE.

--*/

{
    PAGED_CODE( );

    Srp, Block;

    //
    // We always return information about all transports.
    //

    return TRUE;

} // FilterFiles


ULONG
SizeTransports (
    IN PSERVER_REQUEST_PACKET Srp,
    IN PVOID Block
    )

/*++

Routine Description:

    This routine returns the size the passed-in endpoint would take up
    in an API output buffer.

Arguments:

    Srp - not used.

    Block - a pointer to the endpoint to size.

Return Value:

    ULONG - The number of bytes the endpoint would take up in the
        output buffer.

--*/

{
    PENDPOINT endpoint = Block;

    PAGED_CODE( );

    Srp;

    return TOTAL_SIZE_OF_TRANSPORT( endpoint );

} // SizeTransports

