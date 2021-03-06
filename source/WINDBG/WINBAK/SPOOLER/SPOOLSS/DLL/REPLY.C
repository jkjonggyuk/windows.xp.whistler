/*++

Copyright (c) 1990-1994  Microsoft Corporation
All rights reserved

Module Name:

    Reply.c

Abstract:

    Handles all communication setup for RPC from the Server back
    to the Client.

    This implementation allows multiple reply handles for one print
    handle, but relies on serialized access to context handles on this
    machine.

Author:

    Albert Ting (AlbertT) 04-June-94

Environment:

    User Mode -Win32

Revision History:

--*/

#include <windows.h>
#include <winspool.h>
#include <winsplp.h>
#include "router.h"
#include "winspl.h"
#include "change.h"
#include "reply.h"


PPRINTHANDLE pPrintHandleReplyList;


DWORD
OpenReplyRemote(
    LPWSTR pszMachine,
    PHANDLE phNotifyRemote,
    HANDLE hPrinterRemote,
    DWORD dwType,
    DWORD cbBuffer,
    LPBYTE pBuffer)

/*++

Routine Description:

    Establishes a context handle from the server back to the client.
    There is no security in this implementation since delegation
    is not supported.

Arguments:

    pszLocalMachine - Machine to talk to.

    phNotifyRemote - Remote context handle to set up

    hPrinterRemote - remote printer handle we are talking to.

Return Value:

--*/

{
    DWORD dwReturn;

    //
    // If create a context handle to reply.
    //
    RpcTryExcept {

        dwReturn = RpcReplyOpenPrinter(
                       pszMachine,
                       phNotifyRemote,
                       (DWORD)hPrinterRemote,
                       dwType,
                       cbBuffer,
                       pBuffer);


        DBGMSG(DBG_NOTIFY, ("OpenReplyRemote dwReturn = %d, hNotify = 0x%x\n",
                            dwReturn, *phNotifyRemote));

    } RpcExcept(1) {

        dwReturn = RpcExceptionCode();

    } RpcEndExcept

#if DBG
    if (dwReturn) {
        DBGMSG(DBG_ERROR, ("OpenReplyRemote ERROR dwReturn = %d, hNotify = 0x%x\n",
                            dwReturn, *phNotifyRemote));
    }
#endif

    return dwReturn;
}

VOID
CloseReplyRemote(
    HANDLE hNotifyRemote)
{
    DWORD dwError;

    DBGMSG(DBG_NOTIFY, ("CloseReplyRemote requested: 0x%x\n",
                        hNotifyRemote));

    if (!hNotifyRemote)
        return;

    RpcTryExcept {

        dwError = RpcReplyClosePrinter(
                      &hNotifyRemote);

    } RpcExcept(1) {

        dwError = RpcExceptionCode();

    } RpcEndExcept

    if (dwError) {

        DBGMSG(DBG_ERROR, ("FCPCN:ReplyClose error %d, DestroyClientContext: 0x%x\n",
                            dwError,
                            hNotifyRemote));

        //
        // Error trying to close down the notification,
        // clear up our context.
        //
        RpcSmDestroyClientContext(&hNotifyRemote);
    }
}


BOOL
RouterReplyPrinter(
    HANDLE hNotify,
    DWORD fdwFlags,
    DWORD cbBuffer,
    LPBYTE pBuffer)

/*++

Routine Description:

    Handle the notification coming in from a remote router (as
    opposed to a print providor).

Arguments:

    hNotify -- printer that changed, notification context handle

    fdwFlags -- flags that changed

    cbBuffer -- size of pBuffer

    pBuffer -- return buffer (currently empty)

Return Value:

    BOOL  TRUE  = success
          FALSE = fail

--*/

{
    PNOTIFY pNotify = (PNOTIFY)hNotify;
    BOOL bReturn = FALSE;

    EnterCriticalSection(&RouterNotifySection);

    if (!pNotify ||
        pNotify->signature != NOTIFYHANDLE_SIGNATURE ||
        !pNotify->pPrintHandle) {

        SetLastError(ERROR_INVALID_HANDLE);
        goto Done;
    }

    DBGMSG(DBG_NOTIFY, ("RRP: Remote notification received: pNotify 0x%x, pPrintHandle 0x%x\n",
                        pNotify, pNotify->pPrintHandle));

    switch (pNotify->dwType) {
    case REPLY_TYPE_NOTIFICATION:

        bReturn = ReplyPrinterChangeNotification(pNotify->pPrintHandle,
                                                 fdwFlags,
                                                 cbBuffer,
                                                 pBuffer);
        break;

    default:

        DBGMSG(DBG_ERROR, ("RRPCN: Bogus notify 0x%x type: %d\n",
                           pNotify, pNotify->dwType));

        bReturn = FALSE;
        SetLastError(ERROR_INVALID_PARAMETER);
        break;
    }

Done:
    LeaveCriticalSection(&RouterNotifySection);

    return bReturn;
}



/*------------------------------------------------------------------------

    Routines from here down occur on the client machine.

------------------------------------------------------------------------*/

VOID
FreePrinterHandleNotifys(
    PPRINTHANDLE pPrintHandle)
{
    PNOTIFY pNotify;

#if 0
    DBGMSG(DBG_NOTIFY, ("FreePrinterHandleNotifys on 0x%x\n",
                        pPrintHandle));
#endif
    for(pNotify = pPrintHandle->pNotify;
        pNotify;
        pNotify = pNotify->pNext) {

        pNotify->pPrintHandle = NULL;
    }

    //
    // For safety, remove all replys.
    //
    RemoveReplyClient(pPrintHandle,
                      (DWORD)~0);
}

VOID
BeginReplyClient(
    PPRINTHANDLE pPrintHandle,
    DWORD fdwType)
{
    DBGMSG(DBG_NOTIFY, ("BeginReplyClient called 0x%x type %x (sig=0x%x).\n",
                        pPrintHandle, fdwType, pPrintHandle->signature));

    if (!pPrintHandle->fdwReplyTypes) {

        pPrintHandle->pNext = pPrintHandleReplyList;
        pPrintHandleReplyList = pPrintHandle;
    }

    pPrintHandle->fdwReplyTypes |= fdwType;
}

VOID
EndReplyClient(
    PPRINTHANDLE pPrintHandle,
    DWORD fdwType)
{
    DBGMSG(DBG_NOTIFY, ("EndReplyClient called 0x%x type %x.\n",
                        pPrintHandle, fdwType));
}

VOID
RemoveReplyClient(
    PPRINTHANDLE pPrintHandle,
    DWORD fdwType)
{
    PPRINTHANDLE p;

    DBGMSG(DBG_NOTIFY, ("RemoveReplyClient called 0x%x typed %x (sig=0x%x).\n",
                        pPrintHandle, fdwType, pPrintHandle->signature));

    //
    // Remove this reply type from the print handle.
    //
    pPrintHandle->fdwReplyTypes &= ~fdwType;

    //
    // If no replys remain, remove from linked list.
    //
    if (!pPrintHandle->fdwReplyTypes) {

        //
        // Remove from linked list.
        //
        if (pPrintHandleReplyList == pPrintHandle) {

            pPrintHandleReplyList = pPrintHandle->pNext;

        } else {

            for (p = pPrintHandleReplyList; p; p=p->pNext) {

                if (p->pNext == pPrintHandle) {

                    p->pNext = pPrintHandle->pNext;
                    return;
                }
            }
        }
    }
}


BOOL
ReplyOpenPrinter(
    HANDLE hPrinter,
    PHANDLE phNotify,
    DWORD dwType,
    DWORD cbBuffer,
    LPBYTE pBuffer)

/*++

Routine Description:

    When sending a notification back from the print server to the
    client, we open up a notification context handle back on the client.
    This way, every time we send back a notification, we just use this
    context handle.

Arguments:

    hPrinter - printer handle valid here (on the client).  The spoolss.exe
               switches this around for us.

    phNotify - context handle to return to the remote print server.

    dwType - Type of notification

    cbBuffer - reserved for extra information passed

    pBuffer - reserved for extra information passed

Return Value:

    BOOL TRUE = success
         FALSE

--*/

{
    PPRINTHANDLE pPrintHandle;
    PNOTIFY pNotify;
    BOOL bReturnValue = FALSE;
    PPRINTHANDLE p;

    pPrintHandle = (PPRINTHANDLE)hPrinter;

    EnterCriticalSection(&RouterNotifySection);

    //
    // Validate that we are waiting on this print handle.
    // We traverse the linked list to ensure that random bogus
    // hPrinters (which may point to garbage that looks valid)
    // are rejected.
    //
    for (p = pPrintHandleReplyList; p; p=p->pNext) {

        if (p == pPrintHandle)
            break;
    }

    if (!p || !(pPrintHandle->fdwReplyTypes & dwType)) {

        DBGMSG(DBG_ERROR, ("ROPCN: Invalid printer handle 0x%x\n",
                           pPrintHandle));
        SetLastError(ERROR_INVALID_HANDLE);
        goto Done;
    }

    pNotify = AllocSplMem(sizeof(NOTIFY));

    if (!pNotify) {

        goto Done;
    }

    pNotify->signature = NOTIFYHANDLE_SIGNATURE;
    pNotify->pPrintHandle = pPrintHandle;
    pNotify->dwType = dwType;

    //
    // Add us to the list of Notifys.
    //
    pNotify->pNext = pPrintHandle->pNotify;
    pPrintHandle->pNotify = pNotify;

    DBGMSG(DBG_NOTIFY, ("ROPCN: Notification 0x%x (pPrintHandle 0x%x) set up\n",
                        pNotify,
                        pPrintHandle));

    *phNotify = (HANDLE)pNotify;
    bReturnValue = TRUE;

Done:
    LeaveCriticalSection(&RouterNotifySection);

    return bReturnValue;
}


BOOL
ReplyClosePrinter(
    HANDLE hNotify)
{
    PNOTIFY pNotify = (PNOTIFY)hNotify;
    PNOTIFY pNotifyTemp;

    BOOL bReturnValue = FALSE;

    EnterCriticalSection(&RouterNotifySection);

    if (!pNotify || pNotify->signature != NOTIFYHANDLE_SIGNATURE) {

        SetLastError(ERROR_INVALID_HANDLE);
        goto Done;
    }

    if (pNotify->pPrintHandle) {

        //
        // Remove from notification list
        //
        if (pNotify->pPrintHandle->pNotify == pNotify) {

            pNotify->pPrintHandle->pNotify = pNotify->pNext;

        } else {

            for (pNotifyTemp = pNotify->pPrintHandle->pNotify;
                pNotifyTemp;
                pNotifyTemp = pNotifyTemp->pNext) {

                if (pNotifyTemp->pNext == pNotify) {
                    pNotifyTemp->pNext = pNotify->pNext;
                    break;
                }
            }
        }
    }

    DBGMSG(DBG_NOTIFY, ("RCPCN: Freeing notify: 0x%x (pPrintHandle 0x%x)\n",
                         pNotify,
                         pNotify->pPrintHandle));

    FreeSplMem(pNotify, sizeof(NOTIFY));
    bReturnValue = TRUE;

Done:
    LeaveCriticalSection(&RouterNotifySection);

    return bReturnValue;
}


VOID
RundownPrinterNotify(
    HANDLE hNotify)

/*++

Routine Description:

    This is the rundown routine for notifications (the context handle
    for the print server -> client communication).  When the print server
    goes down, the context handle gets rundown on the client (now acting
    as an RPC server).  We should signal the user that something has
    changed.

Arguments:

    hNotify - Handle that has gone invalid

Return Value:

--*/

{
    PNOTIFY pNotify = (PNOTIFY)hNotify;
    BYTE abyDummy[1];

    DBGMSG(DBG_NOTIFY, ("Rundown called: 0x%x type %d\n",
                        pNotify,
                        pNotify->dwType));

    //
    // Notify the client that the printer has changed--it went away.
    // This should _always_ be a local event.
    //
    switch (pNotify->dwType) {

    case REPLY_TYPE_NOTIFICATION:

        ReplyPrinterChangeNotification((HANDLE)pNotify->pPrintHandle,
                                       PRINTER_CHANGE_FAILED_CONNECTION_PRINTER,
                                       1,
                                       abyDummy);

        ReplyClosePrinter(hNotify);
        break;

    default:

        //
        // This can legally occur on a pNotify that was reopened
        // (due to network error) and hasn't been used yet.
        // dwType should be reinitialized every time the pNotify
        // is used.
        //
        DBGMSG(DBG_ERROR, ("Rundown: unknown notify type %d\n",
                           pNotify->dwType));
    }
}


