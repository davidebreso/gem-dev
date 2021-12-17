/* JCE 7 February 1998
 *
 * The callback code is essentially the same for all drawing operations, so
 * it is stored in the library.
 *
 * The assembly language entry code is in PPDLDRAW.AS or PPDSDRAW.AS 
 * depending on memory model. When it is called it sends execution to 
 * _dr_code().
 *
 */

#include "ppdgem.h"
#include <i86.h>

MLOCAL LPPARM drawpar;
MLOCAL WORD drawret;

void interrupt _dr_code(union INTPACK r)			
{	
	LPPBLK pBLK;
	
	_asm{ int 3 };
	/* This code does the following: */
	
	/* ax:bx points at the drawpar */
	drawpar = MK_FP(r.w.ax, r.w.bx);
	/* The parameter passed back to us in drawpar is the address of 
	 * a PPDUBLK. */			
	pBLK = drawpar->pb_parm;
	/* The caller expects the parameter to be the ub_parm field in the
	 * PPDUBLK, so make it so. */
	drawpar->pb_parm = (LPVOID)(pBLK->ub_parm);
	/* And the caller's code is at pBLK->ub_code */
	drawret = (pBLK->ub_code)(drawpar);
}


VOID ppd_userdef(LPTREE tree, WORD nobj, LPPBLK ub)
{
	tree[nobj].ob_type = ((tree[nobj].ob_type & 0xFF00) | G_USERDEF);

	ub->ub_reserved[0] = _dr_code;	/* Draw handler function */
	ub->ub_reserved[1] = ub;		/* Parameter = address of userblk */

	tree[nobj].ob_spec = ub;
}

	


