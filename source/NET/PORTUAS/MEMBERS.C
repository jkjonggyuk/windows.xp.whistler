/*++

Copyright (c) 2000-1993  Microsoft Corporation

Revision History:

    28-Feb-2000 JohnRo
        Fixed MIPS build errors.
    18-Mar-2000 JohnRo
        Use iterator to allow hash-table collision handling.
    24-Apr-2000 JohnRo
        Made immune to UNICODE being defined.
    27-May-2000 JohnRo
        RAID 9829: winsvc.h and related file cleanup.
    29-Sep-2000 JohnRo
        RAID 8001: PORTUAS.EXE not in build (work with stdcall).
    26-Jan-1993 JohnRo
        RAID 8683: PortUAS should set primary group from Mac parms.

--*/


#include <nt.h>         // Needed by <portuasp.h>
#include <ntrtl.h>      // (Needed with nt.h and windows.h)
#include <nturtl.h>     // (Needed with ntrtl.h and windows.h)
#include <windows.h>
#include <lmcons.h>

#include <lmaccess.h>   // LPGROUP_INFO_0, etc.
#include <lmapibuf.h>   // NetApiBufferFree(), etc.
#include <lmerr.h>      // NERR_ equates.
#include "portuasp.h"
#include <stdio.h>
#include <stdlib.h>     // EXIT_FAILURE, EXIT_SUCCESS, _CRTAPI1.
#include <tstring.h>

int _CRTAPI1
main(
    IN int argc,
    IN char *argv[]
    )
{

    NET_API_STATUS rc;
    LPGROUP_INFO_0 groups;
    LPDWORD gids;
    BYTE msg[120];
    BYTE name[GNLEN+1];
    DWORD count;
    DWORD j;
    USER_ITERATOR userIterator;

    if ( argc < 2 ) {

        sprintf( msg, "Usage : members {uas database name}\n" );
        MessageBoxA( NULL, msg, NULL, MB_OK );
        return (EXIT_FAILURE);

    }

    if (( rc = PortUasOpen( argv[1] )) != NERR_Success ) {

        sprintf( msg, "Problem opening database - %ld\n", rc );
        MessageBoxA( NULL, msg, NULL, MB_OK );
        return (EXIT_FAILURE);

    }

    PortUasInitUserIterator( userIterator );
    while (TRUE) {

        rc = PortUasGetUserGroups(
                &userIterator,
                (LPBYTE *) (LPVOID) &groups,
                (LPBYTE *) (LPVOID) &gids,
                &count );
        if ( rc == NERR_UserNotFound ) {
            break;
        } else if ( rc != NERR_Success ) {

            sprintf( msg, "Problem reading user - %ld\n", rc );
            MessageBoxA( NULL, msg, NULL, MB_OK );
            PortUasClose();
            return (EXIT_FAILURE);

        }

        if ( rc == NERR_Success ) {

            printf( "User index - %ld\n", userIterator.Index );

            if ( count == 0 ) {

                printf( "No groups\n" );
            }

            for ( j = 0; j < count; j++ ) {

                NetpCopyWStrToStr( name, groups[j].grpi0_name );
                printf( "Group - %ld - %s\n", gids[j], name );
            }

            printf( "\n" );

            NetApiBufferFree( (LPBYTE)gids );
            NetApiBufferFree( (LPBYTE)groups );
        }
    }

    PortUasClose();
    return (EXIT_SUCCESS);
}
