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

LPVOID proc_malloc(LONG size, LONG *adrcsize)
{
	PR_IASIZE = (LPVOID)size;
        AOUT_LEN = 2;
        gem_if(PROC_MALLOC);
        *adrcsize = (LONG)(PR_OCSIZE);
        AOUT_LEN = 0;
        return(PR_OBEGADDR);
}
