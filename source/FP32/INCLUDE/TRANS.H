/***
*trans.h - definitions for computing transcendentals
*
*	Copyright (c) 1996-2000, Microsoft Corporation.	All rights reserved.
*
*Purpose: Define constants and macros that are used for computing
*   transcendentals. Some of the definitions are machine dependent.
*   Double is assumed to conform to the IEEE 754 std format.
*
*Revision History:
*   08-14-91	GDP	written
*   10-29-91	GDP	removed unused prototypes, added _frnd
*   01-20-92	GDP	significant changes -- IEEE exc. support
*   03-27-92	GDP	put IEEE definitions in fpieee.h
*   03-31-92	GDP	add internal constants for _ctrlfp, _statfp
*   05-08-92    PLM     added M68K switch
*   05-18-92    XY      added exception macro under M68K switch
*   06-23-92    GDP     added macro for negative zero
*   09-06-92    GDP     include cruntime.h, calling convention macros
*   07-16-93    SRW     ALPHA Merge
*   01-13-94    RDL	Added #ifndef _LANGUAGE_ASSEMBLY for asm includes.
*
*******************************************************************************/
#ifndef _INC_TRANS

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LANGUAGE_ASSEMBLY

#include <cruntime.h>

/*
 * Conditional macro definition for function calling type and variable type
 * qualifiers.
 */
#if   ( (_MSC_VER >= 800) && (_M_IX86 >= 300) )

/*
 * Definitions for MS C8-32 (386/486) compiler
 */
#define _CRTAPI1 __cdecl
#define _CRTAPI2 __cdecl

#elif ( _MSC_VER == 600 )

/*
 * Definitions for old MS C6-386 compiler
 */
#define _CRTAPI1 _cdecl
#define _CRTAPI2 _cdecl
#define _CRTVAR1 _cdecl
#define _M_IX86  300

#else

/*
 * Other compilers (e.g., MIPS)
 */
#define _CRTAPI1
#define _CRTAPI2
#define _CRTVAR1

#endif

#include <fpieee.h>

#define D_BIASM1 0x3fe /* off by one to compensate for the implied bit */

#ifdef B_END
/* big endian */
#define D_EXP(x) ((unsigned short *)&(x))
#define D_HI(x) ((unsigned long *)&(x))
#define D_LO(x) ((unsigned long *)&(x)+1)
#else
#define D_EXP(x) ((unsigned short *)&(x)+3)
#define D_HI(x) ((unsigned long *)&(x)+1)
#define D_LO(x) ((unsigned long *)&(x))
#endif

/* return the int representation of the exponent
 * if x = .f * 2^n, 0.5<=f<1, return n (unbiased)
 * e.g. INTEXP(3.0) == 2
 */
#define INTEXP(x) ((signed short)((*D_EXP(x) & 0x7ff0) >> 4) - D_BIASM1)


/* check for infinity, NAN */
#define D_ISINF(x) ((*D_HI(x) & 0x7fffffff) == 0x7ff00000 && *D_LO(x) == 0)
#define IS_D_SPECIAL(x)	((*D_EXP(x) & 0x7ff0) == 0x7ff0)
#define IS_D_NAN(x) (IS_D_SPECIAL(x) && !D_ISINF(x))

#ifdef MIPS

#define IS_D_SNAN(x)	((*D_EXP(x) & 0x7ff8) == 0x7ff8)
#define IS_D_QNAN(x)	((*D_EXP(x) & 0x7ff8) == 0x7ff0 && \
			 (*D_HI(x) << 13 || *D_LO(x)))
#else

#define IS_D_QNAN(x)	((*D_EXP(x) & 0x7ff8) == 0x7ff8)
#define IS_D_SNAN(x)	((*D_EXP(x) & 0x7ff8) == 0x7ff0 && \
			 (*D_HI(x) << 13 || *D_LO(x)))
#endif

#define IS_D_DENORM(x)	((*D_EXP(x) & 0x7ff0) == 0  && \
			 (*D_HI(x) << 12 || *D_LO(x)))


#define IS_D_INF(x)  (*D_HI(x) == 0x7ff00000 && *D_LO(x) == 0)
#define IS_D_MINF(x) (*D_HI(x) == 0xfff00000 && *D_LO(x) == 0)

#ifdef MIPS
#define D_IND_HI 0x7ff7ffff
#define D_IND_LO 0xffffffff
#else
#define D_IND_HI 0xfff80000
#define D_IND_LO 0x0
#endif

typedef union	{
    long lng[2];
    double dbl;
    } _dbl;

extern _dbl _d_inf;
extern _dbl _d_ind;
extern _dbl _d_max;
extern _dbl _d_min;
extern _dbl _d_mzero;

#define D_INF  (_d_inf.dbl)
#define D_IND  (_d_ind.dbl)
#define D_MAX  (_d_max.dbl)
#define D_MIN  (_d_min.dbl)
#define D_MZERO	(_d_mzero.dbl)	     /* minus zero */

/* min and max exponents for normalized numbers in the
 * form: 0.xxxxx... * 2^exp (NOT 1.xxxx * 2^exp !)
 */
#define MAXEXP 1024
#define MINEXP -1021

#endif  /* _LANGUAGE_ASSEMBLY */

#ifdef i386

/* Control word for computation of transcendentals */
#define ICW	   0x133f

#define IMCW	   0xffff

#define IMCW_EM 	0x003f		/* interrupt Exception Masks */
#define IEM_INVALID	0x0001		/*   invalid */
#define IEM_DENORMAL	0x0002		/*   denormal */
#define IEM_ZERODIVIDE	0x0004		/*   zero divide */
#define IEM_OVERFLOW	0x0008		/*   overflow */
#define IEM_UNDERFLOW	0x0010		/*   underflow */
#define IEM_INEXACT	0x0020		/*   inexact (precision) */


#define IMCW_RC	0x0c00			/* Rounding Control */
#define IRC_CHOP	0x0c00		/*   chop */
#define IRC_UP		0x0800		/*   up */
#define IRC_DOWN	0x0400		/*   down */
#define IRC_NEAR	0x0000		/*   near */

#define ISW_INVALID	0x0001		/* invalid */
#define ISW_DENORMAL	0x0002		/* denormal */
#define ISW_ZERODIVIDE	0x0004		/* zero divide */
#define ISW_OVERFLOW	0x0008		/* overflow */
#define ISW_UNDERFLOW	0x0010		/* underflow */
#define ISW_INEXACT	0x0020		/* inexact (precision) */

#define IMCW_PC 	0x0300		/* Precision Control */
#define IPC_24		0x0000		/*    24 bits */
#define IPC_53		0x0200		/*    53 bits */
#define IPC_64		0x0300		/*    64 bits */

#define IMCW_IC 	0x1000		/* Infinity Control */
#define IIC_AFFINE	0x1000		/*   affine */
#define IIC_PROJECTIVE	0x0000		/*   projective */

#endif



#ifdef MIPS
#define ICW		0x00000f80		/* Internal CW for transcendentals */
#define IMCW		0xffffff83		/* Internal CW Mask */

#define IMCW_EM 	0x00000f80		/* interrupt Exception Masks */
#define IEM_INVALID	0x00000800		/*   invalid */
#define IEM_ZERODIVIDE	0x00000400		/*   zero divide */
#define IEM_OVERFLOW	0x00000200		/*   overflow */
#define IEM_UNDERFLOW	0x00000100		/*   underflow */
#define IEM_INEXACT	0x00000080		/*   inexact (precision) */


#define IMCW_RC 	0x00000003		/* Rounding Control */
#define IRC_CHOP	0x00000001		/*   chop */
#define IRC_UP		0x00000002		/*   up */
#define IRC_DOWN	0x00000003		/*   down */
#define IRC_NEAR	0x00000000		/*   near */


#define ISW_INVALID	(1<<6)	/* invalid */
#define ISW_ZERODIVIDE	(1<<5)	/* zero divide */
#define ISW_OVERFLOW	(1<<4)	/* overflow */
#define ISW_UNDERFLOW	(1<<3)	/* underflow */
#define ISW_INEXACT	(1<<2)	/* inexact (precision) */

#endif

#ifdef _ALPHA_

//
// ICW is the Internal Control Word for transcendentals: all five exceptions
// are masked and round to nearest mode is set. IMCW is the mask: all bits
// are set, except for the ISW bits.
//

#define ICW (IEM_INEXACT | IEM_UNDERFLOW | IEM_OVERFLOW | IEM_ZERODIVIDE | IEM_INVALID | IRC_NEAR)
#define ISW (ISW_INEXACT | ISW_UNDERFLOW | ISW_OVERFLOW | ISW_ZERODIVIDE | ISW_INVALID)
#define IMCW (0xffffffff ^ ISW)

//
// The defines for the internal control word match the format of the Alpha
// AXP software FPCR except for the rounding mode which is obtained from the
// Alpha AXP hardware FPCR and shifted right 32 bits.
//

//
// Internal Exception Mask bits.
// Each bit _disables_ an exception (they are not _enable_ bits).
//

#define IMCW_EM 	0x0000003e	/* interrupt Exception Masks */

#define IEM_INEXACT	0x00000020	/*   inexact (precision) */
#define IEM_UNDERFLOW	0x00000010	/*   underflow */
#define IEM_OVERFLOW	0x00000008	/*   overflow */
#define IEM_ZERODIVIDE	0x00000004	/*   zero divide */
#define IEM_INVALID	0x00000002	/*   invalid */

//
// Internal Rounding Control values.
//

#define IMCW_RC 	(0x3 << 26)	/* Rounding Control */

#define IRC_CHOP	(0x0 << 26)	/*   chop */
#define IRC_DOWN	(0x1 << 26)	/*   down */
#define IRC_NEAR	(0x2 << 26)	/*   near */
#define IRC_UP		(0x3 << 26)	/*   up */

//
// Internal Status Word bits.
//

#define ISW_INEXACT	0x00200000	/* inexact (precision) */
#define ISW_UNDERFLOW	0x00100000	/* underflow */
#define ISW_OVERFLOW	0x00080000	/* overflow */
#define ISW_ZERODIVIDE	0x00040000	/* zero divide */
#define ISW_INVALID	0x00020000	/* invalid */

#endif

#ifdef M68K

#include "trans.a"


/* LATER -- we don't handle exception until Mac OS has better support on it */

#define _except1(flags, op, arg1, res, cw) _errcode(flags), \
					    _set_statfp(cw),(res)

#define _except2(flags, op, arg1, arg2, res, cw) _errcode(flags), \
					          _set_statfp(cw),(res)

#define _handle_qnan1(opcode, x, savedcw) _set_errno(_DOMAIN), _rstorfp(savedcw), (x);
#define _handle_qnan2(opcode, x, y, savedcw) _set_errno(_DOMAIN), _rstorfp(savedcw), (x+y);

#endif /* M68K*/

#ifndef _LANGUAGE_ASSEMBLY

#define RETURN(fpcw,result) return _rstorfp(fpcw),(result)

#define RETURN_INEXACT1(op,arg1,res,cw) 		\
	if (cw & IEM_INEXACT) {				\
	    _rstorfp(cw);				\
	    return res; 				\
	}						\
	else {						\
	    return _except1(FP_P, op, arg1, res, cw);	\
	}


#define RETURN_INEXACT2(op,arg1,arg2,res,cw)		\
	if (cw & IEM_INEXACT) {				\
	    _rstorfp(cw);				\
	    return res; 				\
	}						\
	else {						\
	    return _except2(FP_P, op, arg1, arg2, res, cw);	\
	}


#ifdef _ALPHA_

//
// Since fp32 is not compiled in IEEE exception mode perform Alpha NaN
// propagation in software to avoid hardware/kernel trap involvement.
//

extern double _nan2qnan(double);

#define _d_snan2(x,y)	_nan2qnan(y)
#define _s2qnan(x)	_nan2qnan(x)

#else
//handle NaN propagation
#define _d_snan2(x,y)	((x)+(y))
#define _s2qnan(x)	((x)+1.0)
#endif


#define _maskfp() _ctrlfp(ICW, IMCW)
#ifdef _ALPHA_
#define _rstorfp(cw) 0
#else
#define _rstorfp(cw) _ctrlfp(cw, IMCW)
#endif


#define ABS(x) ((x)<0 ? -(x) : (x) )


int _d_inttype(double);

#endif  /* _LANGUAGE_ASSEMBLY */

#define _D_NOINT 0
#define _D_ODD 1
#define _D_EVEN 2


// IEEE exceptions
#define FP_O	     0x01
#define FP_U	     0x02
#define FP_Z	     0x04
#define FP_I	     0x08
#define FP_P	     0x10

// An extra flag for matherr support
// Set together with FP_I from trig functions when the argument is too large
#define FP_TLOSS     0x20


#ifndef _LANGUAGE_ASSEMBLY
#ifdef B_END
#define SET_DBL(msw, lsw)     msw, lsw
#else
#define SET_DBL(msw, lsw)     lsw, msw
#endif
#endif  /* _LANGUAGE_ASSEMBLY */


// special types
#define T_PINF	1
#define T_NINF	2
#define T_QNAN	3
#define T_SNAN	4


// exponent adjustment for IEEE overflow/underflow exceptions
// used before passing the result to the trap handler

#define IEEE_ADJUST 1536

// QNAN values

#define INT_NAN 	(~0)

#define QNAN_SQRT	D_IND
#define QNAN_LOG	D_IND
#define QNAN_LOG10	D_IND
#define QNAN_POW	D_IND
#define QNAN_SINH	D_IND
#define QNAN_COSH	D_IND
#define QNAN_TANH	D_IND
#define QNAN_SIN1	D_IND
#define QNAN_SIN2	D_IND
#define QNAN_COS1	D_IND
#define QNAN_COS2	D_IND
#define QNAN_TAN1	D_IND
#define QNAN_TAN2	D_IND
#define QNAN_ACOS	D_IND
#define QNAN_ASIN	D_IND
#define QNAN_ATAN2	D_IND
#define QNAN_CEIL	D_IND
#define QNAN_FLOOR	D_IND
#define QNAN_MODF	D_IND
#define QNAN_LDEXP	D_IND
#define QNAN_FMOD	D_IND
#define QNAN_FREXP	D_IND


/*
 * Function prototypes
 */

#ifndef _LANGUAGE_ASSEMBLY

double _set_exp(double x, int exp);
double _set_bexp(double x, int exp);
double _add_exp(double x, int exp);
double _frnd(double);
double _fsqrt(double);
#ifndef M68K
double _except1(int flags, int opcode, double arg, double res, unsigned int cw);
double _except2(int flags, int opcode, double arg1, double arg2, double res, unsigned int cw);
#endif
int _sptype(double);
int _get_exp(double);
double _decomp(double, int *);
int _powhlp(double x, double y, double * result);
extern unsigned int _fpstatus;
double _frnd(double);
double _exphlp(double, int *);
#ifndef M68K
double _handle_qnan1(unsigned int op, double arg, unsigned int cw);
double _handle_qnan2(unsigned int op,double arg1,double arg2,unsigned int cw);
#endif
unsigned int _clhwfp(void);
unsigned int _setfpcw(unsigned int);
int _errcode(unsigned int flags);
void _set_errno(int matherrtype);
int _handle_exc(unsigned int flags, double * presult, unsigned int cw);
unsigned int _clrfp(void);
unsigned int _ctrlfp(unsigned int,unsigned int);
unsigned int _statfp(void);
#ifdef M68K
void _set_statfp(unsigned int sw);
#endif

#endif  /* _LANGUAGE_ASSEMBLY */

#ifdef __cplusplus
}
#endif

#define _INC_TRANS
#endif	/* _INC_TRANS */
