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
#include "aes.h"
#include "rclib.h"


/******************************************************************************
 *
 * Tests for two rectangles equal
 *
 ******************************************************************************/

WORD rc_equal(p1, p2)
GRECT FAR *p1;
GRECT FAR *p2;
{
   if ((p1->g_x != p2->g_x) || (p1->g_y != p2->g_y) ||
       (p1->g_w != p2->g_w) || (p1->g_h != p2->g_h))
      return(FALSE);

   return(TRUE);
}

