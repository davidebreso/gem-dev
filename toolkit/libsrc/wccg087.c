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
#include "wccg0.h"

WORD prop_gui_get(WORD property)
{
  PROP_NUM = property;
 
  return gem_if( PROP_GUI_GET );
}

