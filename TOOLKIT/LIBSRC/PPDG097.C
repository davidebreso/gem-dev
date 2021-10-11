/*****************************************************************************
 *
 * Copyright 2006, John Elliott <jce@seasip.demon.co.uk>
 * Copyright 1999, Caldera Thin Clients, Inc.
 *
 *       This software is licenced under the GNU Public License.         
 *       Please see LICENSE.TXT for further information.                 
 *
 *****************************************************************************/

#include "ppdgem.h"
#include "ppdg0.h"

WORD proc_shrink(WORD pid)
{
        PR_NUM = pid;
        return( gem_if(PROC_SHRINK) );
}
