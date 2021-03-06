/* Copyright (c) 2000, Microsoft Corporation, all rights reserved
** @@ ROADMAP :: RasAdmin Comm Ports and Port Status dialog routines
**
** ports.cxx
** Remote Access Server Admin program
** Comm Ports and Port Status dialog routines
** Listed alphabetically by base class methods, subclass methods
**
** 02/05/91 Steve Cobb
** 08/07/92 Chris Caputo - NT Port
**
** CODEWORK:
**
**   * Making the Port Status dialog dynamic text fields a single MLT rather
**     than multiple SLTs might reduce flicker during refresh...then again it
**     might not.
*/

#if 0
#define INCL_WINDOWS
#define INCL_WINDOWS_GDI
#define INCL_DOSERRORS
#define INCL_NETACCESS
#define INCL_NETERRORS
#include <lmui.hxx>

#define INCL_BLT_DIALOG
#define INCL_BLT_CONTROL
#define INCL_BLT_MSGPOPUP
#include <blt.hxx>

#include <string.hxx>
#include <strnumer.hxx>

extern "C"
{
    #include <dialcons.h>
    #include <admapi.h>
    #include <serial.h>
}

#include "rasadmin.rch"
#include "ports.rch"
#include "x25stats.rch"

#include "rasadmin.hxx"
#include "errormsg.hxx"
#include "qtimer.hxx"
#include "refresh.hxx"
#include "util.hxx"
//#include "disconn.hxx"
//#include "sendmsg.hxx"
#include "ports.hxx"
#endif

#include "precomp.hxx"

PORTSTATUS_X25_DIALOG::PORTSTATUS_X25_DIALOG(
    HWND hwndOwner,
    const TCHAR *pszServer,
    const TCHAR *pszDevice )

    /* Constructs a Port Status dialog.
    **
    ** 'hwndOwner' is the handle of the parent window.  'pszServer' is the name
    ** of the Dial-In server whose status is being reported, e.g. "\\SERVER".
    ** 'pszDevice' is the ID of a port on the server, e.g. 1 for "COM1".
    */

    : SERVER_BASE( IDD_X25_PORTSTATUS, hwndOwner, pszServer,
                   IDC_PS_ST_X25_SERVER_VALUE ),
      _sltPort( this, IDC_PS_ST_X25_PORT_VALUE ),
      _pszDevice( pszDevice ),
      _sltLineCondition( this, IDC_PS_ST_X25_CONDITION_VALUE ),
      _sltHardwareCondition( this, IDC_PS_ST_X25_HARDWARE_VALUE ),
      _sltBaud( this, IDC_PS_ST_X25_BAUD_VALUE ),
      _sltBytesTransmitted( this, IDC_PS_ST_X25_BYTES_XMITTED_VALUE ),
      _sltCompressionOut( this, IDC_PS_ST_X25_COMPRESSOUT_VALUE ),
      _sltBytesReceived( this, IDC_PS_ST_X25_BYTES_RECVED_VALUE ),
      _sltCompressionIn( this, IDC_PS_ST_X25_COMPRESSIN_VALUE ),
      _sltFramesTransmitted( this, IDC_PS_ST_X25_FRAMES_XMITTED_VALUE ),
      _sltFramesReceived( this, IDC_PS_ST_X25_FRAMES_RECVED_VALUE ),
      _sltOverrunErrors( this, IDC_PS_ST_X25_OVERRUNS_VALUE ),
      _sltTimeoutErrors( this, IDC_PS_ST_X25_TIMEOUTS_VALUE ),
      _sltFramingErrors( this, IDC_PS_ST_X25_FRAMINGS_VALUE ),
      _sltCrcErrors( this, IDC_PS_ST_X25_CRCS_VALUE ),
      _sltAlignmentErrors( this, IDC_PS_ST_X25_ALIGNS_VALUE ),
      _sltBufferOverrunErrors( this, IDC_PS_ST_X25_BUFFERS_VALUE ),
      _qtimerRefresh( QueryHwnd(), PS_REFRESHRATEMS )
{
    if (QueryError() != NERR_Success)
    {
        return;
    }

    /* Fill and display the port name and statistics fields.
    */
    RefreshStats();

    /* Start timer to trigger statistics updates.
    */
    _qtimerRefresh.Start();
}


VOID PORTSTATUS_X25_DIALOG::ClearStats() const

    /* Clears Dial-In port statistical data.  It's the counters on the server
    ** that are cleared, not the displayed values.  An error popup is
    ** generated if indicated.
    */
{
    AUTO_CURSOR cursorHourglass;

    APIERR err = RasadminPortClearInfo( QueryServer(), QueryDevice() );

    if (err != NERR_Success)
    {
        ErrorMsgPopup( (OWNER_WINDOW* )this, IDS_OP_PORTCLEARINFO_S, err,
                       SkipUnc( QueryServer() ) );
    }
}


VOID PORTSTATUS_X25_DIALOG::RefreshStats()

    /* Refresh port statistics fields with current data from port.  Error
    ** popups are generated if indicated.
    */
{
    AUTO_CURSOR cursorHourglass;
  
    PRAS_PORT_1 prasport1;
    PDWORD stats;
    RAS_PARAMS *params;

    APIERR err = RasadminPortGetInfo( QueryServer(), QueryDevice(), &prasport1,
            &stats, &params);
  
    if (err != NERR_Success)
    {
        STACK_NLS_STR( nlsPlaceHolder, MAX_RES_STR_LEN + 1 );
  
        ErrorMsgPopup(this, IDS_OP_PORTGETINFO_S, err, SkipUnc(QueryServer()));
  
        nlsPlaceHolder.Load( IDS_IDLEPORT );
  
        _sltPort.SetText( nlsPlaceHolder );
        _sltBaud.SetText( nlsPlaceHolder );
        _sltLineCondition.SetText( nlsPlaceHolder );
        _sltHardwareCondition.SetText( nlsPlaceHolder );
        _sltBytesTransmitted.SetText( nlsPlaceHolder );
        _sltCompressionOut.SetText( nlsPlaceHolder );
        _sltBytesReceived.SetText( nlsPlaceHolder );
        _sltCompressionIn.SetText( nlsPlaceHolder );
        _sltFramesTransmitted.SetText( nlsPlaceHolder );
        _sltFramesReceived.SetText( nlsPlaceHolder );
        _sltOverrunErrors.SetText( nlsPlaceHolder );
        _sltTimeoutErrors.SetText( nlsPlaceHolder );
        _sltFramingErrors.SetText( nlsPlaceHolder );
        _sltCrcErrors.SetText( nlsPlaceHolder );
        _sltAlignmentErrors.SetText( nlsPlaceHolder );
        _sltBufferOverrunErrors.SetText( nlsPlaceHolder );
  
        return;
    }
  
  
    /* Got the data...now display it.
    */

    UIASSERT(prasport1->NumStatistics == NUM_RAS_SERIAL_STATS);

    _sltPort.SetText( QueryDevice() );
  
    STACK_NLS_STR( nlsCondition, MAX_RES_STR_LEN + 1 );
    UINT idsCondition;
  
    switch (prasport1->LineCondition)
    {
        case RAS_PORT_DISCONNECTED:
            idsCondition = IDS_LINE_DISCONNECTED;
            break;
  
        case RAS_PORT_CALLING_BACK:
            idsCondition = IDS_LINE_CALLING_BACK;
            break;
  
        case RAS_PORT_LISTENING:
            idsCondition = IDS_LINE_WAITING_FOR_CALL;
            break;
  
        case RAS_PORT_AUTHENTICATING:
            idsCondition = IDS_LINE_AUTHENTICATING;
            break;
  
        case RAS_PORT_AUTHENTICATED:
            idsCondition = IDS_LINE_AUTHENTICATED;
            break;
  
        case RAS_PORT_INITIALIZING:
            idsCondition = IDS_LINE_INITIALIZING;
            break;
  
        case RAS_PORT_NON_OPERATIONAL:
        default:
            idsCondition = IDS_LINE_NON_OPERATIONAL;
            break;
    }
  
    (VOID )nlsCondition.Load( idsCondition );
    _sltLineCondition.SetText( nlsCondition );
  
    switch (prasport1->HardwareCondition)
    {
        case RAS_MODEM_OPERATIONAL:
            idsCondition = IDS_MODEM_OPERATIONAL;
            break;
  
        case RAS_MODEM_INCORRECT_RESPONSE:
            idsCondition = IDS_MODEM_INCORRECT_RESPONSE;
            break;
  
        case RAS_MODEM_HARDWARE_FAILURE:
            idsCondition = IDS_MODEM_HARDWARE_FAILURE;
            break;
  
        case RAS_MODEM_NOT_RESPONDING:
            idsCondition = IDS_MODEM_NOT_RESPONDING;
            break;
  
        case RAS_MODEM_UNKNOWN:
            idsCondition = IDS_MODEM_UNKNOWN;
            break;
    }
  

    (VOID )nlsCondition.Load( idsCondition );
    _sltHardwareCondition.SetText( nlsCondition );
  
    DEC_STR decnlsBaud( prasport1->LineSpeed );
    _sltBaud.SetText( decnlsBaud );
  
    NUM_NLS_STR numnls( 0 );
  

    numnls = stats[BYTES_XMITED];
    _sltBytesTransmitted.SetText( numnls );

    numnls = stats[BYTES_RCVED];
    _sltBytesReceived.SetText( numnls );
  
    if (stats[BYTES_XMITED_COMP])
    {
        ULONG ulUncomp = stats[BYTES_XMITED_UNCOMP];
        ULONG ulComp   = stats[BYTES_XMITED_COMP];

        numnls = ((ulUncomp <= 0 || ulUncomp < ulComp)
                 ? 0 : ((ulUncomp - ulComp) * 100 ) / ulUncomp );
    }
    else
    {
        numnls = 0;
    }
    numnls.strcat(SZ("%"));
    _sltCompressionOut.SetText( numnls );
  
    if (stats[BYTES_RCVED_COMP])
    {
        ULONG ulUncomp = stats[BYTES_RCVED_UNCOMP];
        ULONG ulComp   = stats[BYTES_RCVED_COMP];

        numnls = ((ulUncomp <= 0 || ulUncomp < ulComp)
                 ? 0 : ((ulUncomp - ulComp) * 100 ) / ulUncomp );
    }
    else
    {
        numnls = 0;
    }

    numnls.strcat(SZ("%"));
    _sltCompressionIn.SetText( numnls );
  
    numnls.strcat(SZ("%"));
    _sltCompressionIn.SetText( numnls );
  
    numnls = stats[FRAMES_XMITED];
    _sltFramesTransmitted.SetText( numnls );
  
    numnls = stats[FRAMES_RCVED];
    _sltFramesReceived.SetText( numnls );
  
    numnls = stats[SERIAL_OVERRUN_ERR];
    _sltOverrunErrors.SetText( numnls );
  
    numnls = stats[TIMEOUT_ERR];
    _sltTimeoutErrors.SetText( numnls );
  
    numnls = stats[FRAMING_ERR];
    _sltFramingErrors.SetText( numnls );
  
    numnls = stats[CRC_ERR];
    _sltCrcErrors.SetText( numnls );

    numnls = stats[ALIGNMENT_ERR];
    _sltAlignmentErrors.SetText( numnls );

    numnls = stats[BUFFER_OVERRUN_ERR];
    _sltBufferOverrunErrors.SetText( numnls );


    RasadminFreeBuffer(prasport1);
    RasadminFreeBuffer(stats);
    RasadminFreeBuffer(params);
}


VOID PORTSTATUS_X25_DIALOG::OnClear()

    /* Action taken when Clear button is pressed.  The statistics on the
    ** Dial-In server are cleared and the statistics fields are redisplayed to
    ** reflect same.  Error popups are generated if indicated.
    */
{
    ClearStats();
    RefreshStats();
}


BOOL PORTSTATUS_X25_DIALOG::OnCommand(
    const CONTROL_EVENT & event )

    /*
    ** Returns true if the command is processed, false otherwise.
    */
{
    switch (event.QueryCid())
    {
        case IDC_PS_PB_X25_RESET:
            OnClear();
            return TRUE;
    }

    /* Not one of our commands, so pass to base class for default handling.
    */
    return SERVER_BASE::OnCommand( event );
}


BOOL PORTSTATUS_X25_DIALOG::OnTimer(
    const TIMER_EVENT & event )

    /*
    ** Returns true if processes the command, false otherwise.
    */
{
    UNREFERENCED(event);

    /* Refresh timeout, update the statistics fields.
    */
    _qtimerRefresh.Stop();
    RefreshStats();
    _qtimerRefresh.Start();
    return TRUE;
}


ULONG PORTSTATUS_X25_DIALOG::QueryHelpContext()
{
    return HC_X25_PORTSTATUS;
}

