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

WORD proc_info(WORD num, WORD *oisswap, WORD *oisgem, 
                LPVOID *obegaddr, LONG *ocsize, LPVOID *oendmem, 
                LONG *ossize, LPVOID *ointtbl)
{
	WORD ret;

	PR_NUM = num;
	AOUT_LEN = 5;
	ret = gem_if(PROC_INFO);
	AOUT_LEN = 0;
	*oisswap = PR_OISSWAP;
	*oisgem = PR_OISGEM;
	*obegaddr = PR_OBEGADDR;
	*ocsize = (LONG)(PR_OCSIZE);
	*oendmem = PR_OENDMEM;
	*ossize = (LONG)(PR_OSSIZE);
	*ointtbl = PR_OITBL;
	return ret;
}
