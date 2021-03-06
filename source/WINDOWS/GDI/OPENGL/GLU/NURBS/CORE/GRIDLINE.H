/**************************************************************************
 *									  *
 * 		 Copyright (C) 2000, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/*
 * gridline.h - $Revision: 1.1 $
 */

#ifndef __GRIDLINE_H__
#define __GRIDLINE_H__

struct Gridline {
    long 		v;
    REAL		vval;
    long		vindex;
    long 		ustart;
    long 		uend;
 };
#endif /* __GRIDLINE_H__ */
