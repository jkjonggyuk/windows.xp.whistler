/***
*mbscspn.c - Find first string char in charset (MBCS)
*
*	Copyright (c) 1985-2000, Microsoft Corporation.  All rights reserved.
*
*Purpose:
*	Find first string char in charset (MBCS)
*
*Revision History:
*	11-19-92  KRS	Ported from 16-bit sources.
*
*******************************************************************************/

#ifdef _MBCS
#include <cruntime.h>
#include <mbdata.h>
#include <mbctype.h>
#include <mbstring.h>
#include <stddef.h>


/*** 
#ifndef _RETURN_PTR
* _mbscspn - Find first string char in charset (MBCS)
#else
* _mbspbrk - Find first string char in charset, pointer return (MBCS)
#endif
*
*Purpose:
*       Returns maximum leading segment of string
*	which consists solely of characters NOT from charset.
*	Handles MBCS chars correctly.
*
*Entry:
*	char *string = string to search in
*	char *charset = set of characters to scan over
*
*Exit:
*
#ifndef _RETURN_PTR
*	Returns the index of the first char in string
*	that is in the set of characters specified by control.
*
*	Returns 0, if string begins with a character in charset.
#else
*	Returns pointer to first character in charset.
*
*	Returns NULL if string consists entirely of characters
*	not from charset.
#endif
*
*Exceptions:
*
*******************************************************************************/

#ifndef _RETURN_PTR

size_t _mbscspn( string, charset )
const unsigned char *string;
const unsigned char *charset;

#else

unsigned char * _CRTAPI1 _mbspbrk( string, charset )
const unsigned char *string;
const unsigned char  *charset;

#endif

{
        unsigned char *p, *q;

	/* loop through the string to be inspected */
	for (q = (char *)string; *q ; q++) {

		/* loop through the charset */
		for (p = (char *)charset; *p ; p++) {

			if (_ISLEADBYTE(*p)) {
				if (((*p == *q) && (p[1] == q[1])) || p[1] == '\0')
					break;
				p++;
			}

			else
				if (*p == *q)
					break;
                }

		if (*p != '\0') 	/* end of charset? */
			break;		/* no, match on this char */

		if (_ISLEADBYTE(*q))
                        if (*++q == '\0')
                                break;
        }

#ifndef _RETURN_PTR
	return((size_t) (q - string));		/* index */
#else
	return((*q) ? q : NULL);	/* pointer */
#endif

}
#endif	/* _MBCS */
