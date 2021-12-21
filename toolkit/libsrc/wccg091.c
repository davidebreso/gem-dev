/*****************************************************************************
 *
 * Copyright 2006, John Elliott <jce@seasip.demon.co.uk>
 * Copyright 1999, Caldera Thin Clients, Inc.
 *
 *       This software is licenced under the GNU Public License.         
 *       Please see LICENSE.TXT for further information.                 
 *
 *****************************************************************************/

#include "wccgem.h"
#include "wccg0.h"

WORD proc_create(LPVOID ibegaddr, LONG isize, WORD isswap, WORD isgem,
		                WORD *onum)
{
        WORD    ret;

        PR_IBEGADDR = ibegaddr;
        PR_ISIZE = (LPVOID)isize;
        PR_ISSWAP = isswap;
        PR_ISGEM = isgem;
        ret = gem_if(PROC_CREATE); 
        *onum = PR_ONUM;
	return(ret);
}
