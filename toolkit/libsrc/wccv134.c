/*****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include "wccgem.h"
#include "wccv0.h"

WORD v_bez_off(handle)
WORD handle;
{
   contrl[0] = 11;
   contrl[1] = 0;
   contrl[3] = 0;
   contrl[5] = 13;
   contrl[6] = handle;

   vdi();

   return( intout[0] );
}

