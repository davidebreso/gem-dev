/*****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include "portab.h"
#include "xfmlib.h"

EXTERN WORD x_xmul;              /* scaling factors for x transformation      */
EXTERN WORD x_xdiv;
EXTERN WORD x_xtrans;            /* translate factor for x transformation     */
EXTERN WORD x_ymul;              /* scaling factors for x transformation      */
EXTERN WORD x_ydiv;
EXTERN WORD x_ytrans;            /* translate factor for x transformation     */
EXTERN WORD x_wmicrons;
EXTERN WORD x_hmicrons;

/******************************************************************************
 *
 * transform an x-value from device space into user space.
 *
 ******************************************************************************/

WORD x_duy_xform(y)
WORD y;
{
   return( x_mul_div( y - x_ytrans, x_ydiv, x_ymul));
}

