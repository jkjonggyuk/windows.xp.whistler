//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2000 - 2000.
//
//  File:       seekptr.cxx
//
//  Contents:   Seek pointer non-inline implementation
//
//  History:    11-Aug-92       PhilipLa        Created.
//
//--------------------------------------------------------------------------
#include <exphead.cxx>
#pragma hdrstop                 //  Remove for MAC build

#include <seekptr.hxx>

//+--------------------------------------------------------------
//
//  Member:     CSeekPointer::Release, public
//
//  Synopsis:   Decrements _cReferences and delete's on noref
//
//  History:    30-Apr-92       DrewB   Created
//
//---------------------------------------------------------------

#ifdef CODESEGMENTS
#pragma code_seg(SEG_CSeekPointer_vRelease)
#endif

void CSeekPointer::vRelease(void)
{
    LONG lRet;
    
    olDebugOut((DEB_ITRACE, "In  CSeekPointer::Release()\n"));
    olAssert(_cReferences > 0);
    lRet = AtomicDec(&_cReferences);
    if (lRet == 0)
        delete this;
    olDebugOut((DEB_ITRACE, "Out CSeekPointer::Release()\n"));
}

