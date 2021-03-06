/*
** Copyright 2000,2000, Silicon Graphics, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Revision: 1.8 $
** $Date: 1993/08/31 16:23:06 $
*/
#include "context.h"
#include "global.h"
#include "imfuncs.h"

void __glim_Bitmap(GLsizei w, GLsizei h, GLfloat xOrig, GLfloat yOrig,
		   GLfloat xMove, GLfloat yMove, const GLubyte *bitmap)
{
    __GL_SETUP();
    GLuint beginMode;

    beginMode = gc->beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    gc->beginMode = __GL_NOT_IN_BEGIN;
	    (*gc->dispatchState->dispatch->Bitmap)(w,h,xOrig,yOrig,
		    xMove,yMove,bitmap);
	    return;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    if ((w < 0) || (h < 0)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    (*gc->procs.bitmap)(gc, w, h, xOrig, yOrig, xMove, yMove, bitmap);
}
