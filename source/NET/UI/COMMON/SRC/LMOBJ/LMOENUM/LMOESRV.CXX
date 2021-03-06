/*****************************************************************/
/**		     Microsoft LAN Manager			**/
/**	       Copyright(c) Microsoft Corp., 2000		**/
/*****************************************************************/

/*
 *  HISTORY:
 *	RustanL     03-Jan-2000     Created
 *	RustanL     10-Jan-2000     Added SERVER1 subclass and iterator
 *	beng	    11-Feb-2000     Uses lmui.hxx
 *	KeithMo	    21-Oct-2000	    Remove INCL_WINDOWS, enhanced WIN32.
 *
 */

#include "pchlmobj.hxx"

/*****************************	SERVER_ENUM  ******************************/


/**********************************************************\

   NAME:       SERVER_ENUM::SERVER_ENUM

   SYNOPSIS:   server_enum constructor

   ENTRY:

   EXIT:

   NOTES:

   HISTORY:
   	RustanL     03-Jan-2000     Created

\**********************************************************/

SERVER_ENUM::SERVER_ENUM( const TCHAR * pszServer,
			  UINT	       uLevel,
			  const TCHAR * pszDomain,
			  ULONG        flServerType )
  : LOC_LM_ENUM( pszServer, uLevel ),
    _nlsDomain( pszDomain ),
    _flServerType( flServerType )
{
    UIASSERT( uLevel == 1 );	// only supports level 1 at this time

    if( QueryError() != NERR_Success )
    {
    	return;
    }

    if( !_nlsDomain )
    {
	ReportError( _nlsDomain.QueryError() );
    	return;
    }

}  // SERVER_ENUM::SERVER_ENUM



/**********************************************************\

   NAME:       SERVER_ENUM::CallAPI

   SYNOPSIS:   server enum call api

   ENTRY:

   EXIT:

   NOTES:

   HISTORY:
   	RustanL     03-Jan-2000     Created

\**********************************************************/

APIERR SERVER_ENUM::CallAPI( BYTE ** ppbBuffer,
			     UINT  * pcEntriesRead )
{
    UIASSERT( QueryInfoLevel() == 1 );	    // only level support at the time

    return ::MNetServerEnum( QueryServer(),
			     SERVER_INFO_LEVEL( QueryInfoLevel() ),
			     ppbBuffer,
			     pcEntriesRead,
			     _flServerType,
			     (TCHAR *)_nlsDomain.QueryPch() );

}  // SERVER_ENUM::CallAPI



/*****************************	SERVER1_ENUM  ******************************/


/**********************************************************\

   NAME:       SERVER1_ENUM::SERVER1_ENUM

   SYNOPSIS:   server enum 1 constructor

   ENTRY:

   EXIT:

   NOTES:

   HISTORY:
   	RustanL     03-Jan-2000     Created

\**********************************************************/

SERVER1_ENUM::SERVER1_ENUM( const TCHAR * pszServer,
			    const TCHAR * pszDomain,
			    ULONG	 flServerType )
  : SERVER_ENUM( pszServer, 1, pszDomain, flServerType )
{
    // do nothing else

}  // SERVER1_ENUM::SERVER1_ENUM



/*******************************************************************

    NAME:	SERVER1_ENUM_OBJ :: SetBufferPtr

    SYNOPSIS:	Saves the buffer pointer for this enumeration object.

    ENTRY:	pBuffer			- Pointer to the new buffer.

    EXIT:	The pointer has been saved.

    NOTES:	Will eventually handle OemToAnsi conversions.

    HISTORY:
	KeithMo	    09-Oct-2000	    Created.

********************************************************************/
#ifdef WIN32

VOID SERVER1_ENUM_OBJ :: SetBufferPtr( const SERVER_INFO_101 * pBuffer )

#else	// !WIN32

VOID SERVER1_ENUM_OBJ :: SetBufferPtr( const struct server_info_1 * pBuffer )

#endif	// WIN32
{
    ENUM_OBJ_BASE :: SetBufferPtr( (const BYTE *)pBuffer );

}   // SERVER1_ENUM_OBJ :: SetBufferPtr


#ifdef WIN32

DEFINE_LM_ENUM_ITER_OF( SERVER1, SERVER_INFO_101 );

#else	// !WIN32

DEFINE_LM_ENUM_ITER_OF( SERVER1, struct server_info_1 );

#endif	// WIN32

