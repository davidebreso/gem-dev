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

VOID v_pat_rotate(handle, angle)
WORD handle, angle;
{
   contrl[0] = 134;
   contrl[1] = 0;
   contrl[3] = 1;
   contrl[6] = handle;

   intin[0] = angle;

   vdi();
}


