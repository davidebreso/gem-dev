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
#include "oblib.h"


/******************************************************************************
 *
 * Set flags in ob_flags of a object
 *
 ******************************************************************************/

VOID ob_doflag(tree, index, flag)
OBJECT FAR *tree;
WORD index;
WORD flag;
{
   tree[index].ob_flags |= flag;
}

