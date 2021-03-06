/*****************************************************************************
*
* COPYRIGHT 1993 - COLORADO MEMORY SYSTEMS, INC.
* ALL RIGHTS RESERVED.
*
******************************************************************************
*
* FILE: \SE\DRIVER\Q117CD\SRC\0X11055.C
*
* FUNCTION: cqd_CheckMediaCompatibility
*
* PURPOSE: Determine the compatibility of the media, drive combination.
*
* HISTORY:
*		$Log:   J:\se.vcs\driver\q117cd\src\0x11055.c  $
*	
*	   Rev 1.2   03 Jun 1994 15:30:20   BOBLEHMA
*	Changed to use the drive type defines from the frb_api.h header.
*	Defines from cqd_defs.h contain different values and can't be interchanged.
*	
*	   Rev 1.1   17 Feb 1994 15:39:42   KEVINKES
*	Fixed a semicolon idiocy.
*
*	   Rev 1.0   17 Feb 1994 15:21:26   KEVINKES
*	Initial Revision.
*
*****************************************************************************/
#define FCT_ID 0x11055
#include "include\neptune\adi_api.h"
#include "include\neptune\frb_api.h"
#include "include\neptune\kdi_pub.h"
#include "include\neptune\cqd_pub.h"
#include "q117cd\include\cqd_defs.h"
#include "q117cd\include\cqd_strc.h"
#include "q117cd\include\cqd_hdr.h"
/*endinclude*/

dStatus cqd_CheckMediaCompatibility
(
/* INPUT PARAMETERS:  */

   CqdContextPtr cqd_context

/* UPDATE PARAMETERS: */

/* OUTPUT PARAMETERS: */

)
/* COMMENTS: *****************************************************************
 *
 * DEFINITIONS: *************************************************************/
{

/* DATA: ********************************************************************/

	dStatus status=ERR_NO_ERR;	/* Status or error condition.*/

/* CODE: ********************************************************************/

	switch (cqd_context->device_descriptor.drive_class) {

	case QIC40_DRIVE:

		switch (cqd_context->tape_cfg.tape_class) {

		case QIC40_FMT:

			cqd_context->tape_cfg.formattable_media = dTRUE;
			cqd_context->tape_cfg.read_only_media = dFALSE;
			break;

		default:

   	   status =  kdi_Error(ERR_UNSUPPORTED_FORMAT, FCT_ID, ERR_SEQ_1);
		}

		break;

	case QIC80_DRIVE:

		switch (cqd_context->tape_cfg.tape_class) {

		case QIC40_FMT:

			cqd_context->tape_cfg.formattable_media = dTRUE;
			cqd_context->tape_cfg.read_only_media = dTRUE;
			break;

		case QIC80_FMT:

			cqd_context->tape_cfg.formattable_media = dTRUE;
			cqd_context->tape_cfg.read_only_media = dFALSE;
			break;

		default:

   	   status =  kdi_Error(ERR_UNSUPPORTED_FORMAT, FCT_ID, ERR_SEQ_2);
		}

		break;

	case QIC3010_DRIVE:

		switch (cqd_context->tape_cfg.tape_class) {

		case QIC40_FMT:
		case QIC80_FMT:

			cqd_context->tape_cfg.formattable_media = dFALSE;
			cqd_context->tape_cfg.read_only_media = dTRUE;
			break;

		case QIC3010_FMT:

			cqd_context->tape_cfg.formattable_media = dTRUE;
			cqd_context->tape_cfg.read_only_media = dFALSE;
			break;

		default:

   	   status =  kdi_Error(ERR_UNSUPPORTED_FORMAT, FCT_ID, ERR_SEQ_3);
		}

		break;

	case QIC3020_DRIVE:

		switch (cqd_context->tape_cfg.tape_class) {

		case QIC40_FMT:
		case QIC80_FMT:

			cqd_context->tape_cfg.formattable_media = dFALSE;
			cqd_context->tape_cfg.read_only_media = dTRUE;
			break;

		case QIC3010_FMT:
		case QIC3020_FMT:

			cqd_context->tape_cfg.formattable_media = dTRUE;
			cqd_context->tape_cfg.read_only_media = dFALSE;
			break;

		default:

   	   status =  kdi_Error(ERR_UNSUPPORTED_FORMAT, FCT_ID, ERR_SEQ_4);
		}

		break;

	default:

   	status =  kdi_Error(ERR_UNSUPPORTED_FORMAT, FCT_ID, ERR_SEQ_5);

	}

	return status;
}
