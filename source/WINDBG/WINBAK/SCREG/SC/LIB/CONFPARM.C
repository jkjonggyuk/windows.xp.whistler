/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    ConfParm.c

Abstract:

    This file contains:
        ScCheckServiceConfigParms


Author:

    John Rogers (JohnRo) 14-Apr-2000

Environment:

    User Mode - Win32

Revision History:

    14-Apr-2000 JohnRo
        Created.
    20-May-2000 JohnRo
        Use CONST where possible.

--*/


//
// INCLUDES
//

#include <nt.h>
#include <ntrtl.h>      // Needed by <scdebug.h>

#include <wcstr.h>      // wcsicmp

#include <windef.h>

#include <scdebug.h>    // SC_ASSERT().
#include <sclib.h>      // My prototype.
#include <valid.h>      // ERROR_CONTROL_INVALID(), SERVICE_TYPE_INVALID(), etc.
#include <winerror.h>   // NO_ERROR and ERROR_ equates.
#include <winsvc.h>     // SERVICE_ equates.



DWORD
ScCheckServiceConfigParms(
    IN  BOOL            Change,
    IN  LPCWSTR         lpServiceName,
    IN  DWORD           dwActualServiceType,
    IN  DWORD           dwNewServiceType,
    IN  DWORD           dwStartType,
    IN  DWORD           dwErrorControl,
    IN  LPCWSTR         lpBinaryPathName OPTIONAL,
    IN  LPCWSTR         lpLoadOrderGroup OPTIONAL,
    IN  LPCWSTR         lpDependencies OPTIONAL
    )

/*++

Routine Description:

    This routine checks parameters for a CreateService() or a
    ChangeServiceConfig() API.

Arguments:

    Change - TRUE if this is the result of a ChangeServiceConfig API.
        (This flag is used to determine whether other parameters may be
        SERVICE_NO_CHANGE or NULL pointers.)

    dwActualServiceType - For ChangeServiceConfig, contains the current
        service type associated with this service.  Otherwise, this is
        the same value as dwNewServiceType.

    dwNewServiceType - SERVICE_NO_CHANGE or the service type given by app.

    (Other parameters same as CreateService and ChangeServiceConfig APIs.)

Return Value:

    NO_ERROR or error code.

--*/

{
    DWORD dwFinalServiceType;

#define PARM_MISSING( ws )   ( ((ws)==NULL) || ((*(ws)) == L'\0') )

    if ( !Change ) {

        //
        // Almost all fields must be present for creating a service.
        // (Exceptions are dependencies and password.)
        //
        if ( (dwNewServiceType  == SERVICE_NO_CHANGE) ||
             (dwStartType       == SERVICE_NO_CHANGE) ||
             (dwErrorControl    == SERVICE_NO_CHANGE) ||
             PARM_MISSING(lpBinaryPathName) ) {

            return (ERROR_INVALID_PARAMETER);

        }
    }

    //
    // Validate actual and desired service types.
    //
    if (dwNewServiceType != SERVICE_NO_CHANGE) {
        if ( SERVICE_TYPE_INVALID( dwNewServiceType ) ) {
            return (ERROR_INVALID_PARAMETER);
        }

        //
        // Not allowed to change the service type from Win32 to Driver
        // or Driver to Win32.
        //
        if ( ((dwNewServiceType & SERVICE_DRIVER) &&
              (dwActualServiceType & SERVICE_WIN32)) ||
             ((dwNewServiceType & SERVICE_WIN32) &&
              (dwActualServiceType & SERVICE_DRIVER)) ) {
            return (ERROR_INVALID_PARAMETER);
        }
    }
    if (dwActualServiceType == SERVICE_NO_CHANGE) {
        return (ERROR_INVALID_PARAMETER);
    }

    //
    // Validate start type (if that was given).
    //
    if (dwStartType != SERVICE_NO_CHANGE) {

        if ( START_TYPE_INVALID( dwStartType ) ) {
            return (ERROR_INVALID_PARAMETER);
        }

        //
        // A boot-start or system-start service must be a driver
        //
        if (dwStartType == SERVICE_BOOT_START ||
            dwStartType == SERVICE_SYSTEM_START) {

            if (dwNewServiceType == SERVICE_NO_CHANGE) {

                if (dwActualServiceType != SERVICE_KERNEL_DRIVER &&
                    dwActualServiceType != SERVICE_FILE_SYSTEM_DRIVER) {

                    return (ERROR_INVALID_PARAMETER);
                }
            }
            else {
                if (dwNewServiceType != SERVICE_KERNEL_DRIVER &&
                    dwNewServiceType != SERVICE_FILE_SYSTEM_DRIVER) {

                    return (ERROR_INVALID_PARAMETER);
                }
            }
        }
    }

    //
    // Validate error control...
    //
    if (dwErrorControl != SERVICE_NO_CHANGE) {
        if ( ERROR_CONTROL_INVALID( dwErrorControl ) ) {
            return (ERROR_INVALID_PARAMETER);
        }
    }

    //
    // Path type depends on final service type.
    //
    if (dwNewServiceType == SERVICE_NO_CHANGE) {
        dwFinalServiceType = dwActualServiceType;
    } else {
        dwFinalServiceType = dwNewServiceType;
    }
    SC_ASSERT( dwFinalServiceType != SERVICE_NO_CHANGE );

    //
    // Validate binary path name.
    //
    if (lpBinaryPathName != NULL) {

        // Check path name syntax and make sure it matches service type.
        // Path type depends on final service type.
        if ( !ScIsValidImagePath( lpBinaryPathName, dwFinalServiceType ) ) {
            return (ERROR_INVALID_PARAMETER);
        }
    }

    //
    // Check for trivial cases of circular dependencies:
    //    1) Service is dependent on itself
    //    2) Service is dependent on a group it is a member of
    //
    if ((lpDependencies != NULL) && (*lpDependencies != 0)) {

        LPWSTR DependPtr;


        DependPtr = (LPWSTR) lpDependencies;

        while (*DependPtr != 0) {

            if (*DependPtr == SC_GROUP_IDENTIFIERW) {

                if ((lpLoadOrderGroup != NULL) && (*lpLoadOrderGroup != 0)) {

                    if (wcsicmp(DependPtr + 1, lpLoadOrderGroup) == 0) {

                        //
                        // Service depends on the group it is in
                        //
                        return (ERROR_CIRCULAR_DEPENDENCY);
                    }
                }
            }
            else {

                if (wcsicmp(DependPtr, lpServiceName) == 0) {

                    //
                    // Service depends on itself
                    //
                    return (ERROR_CIRCULAR_DEPENDENCY);
                }
            }

            DependPtr += (wcslen(DependPtr) + 1);
        }
    }

    //
    // Start name is checked and canonicalized in ScValidateAndSaveAccount
    // if service type is Win32.  If service type is driver, it's up to
    // the I/O system to validate the driver name (it can be NULL and the
    // I/O system will use the default driver name).
    //

    return (NO_ERROR);
}
