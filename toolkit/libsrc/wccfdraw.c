/* JCE 7 February 1998
 *
 * The callback code is essentially the same for all drawing operations, so
 * it is stored in the library.
 *
 * The assembly language entry code is in _far_draw. When it is called it 
 * sends execution to _dr_code().
 *
 */

#include "wccgem.h"

#define NEWSTACK_SIZ 1024
MLOCAL BYTE newstack[NEWSTACK_SIZ];
MLOCAL WORD save_ss, save_sp;

MLOCAL WORD _dr_code(LPPARM drawpar);

/* 
 * This is a callback function to which GEM passes parameters in AX and BX.
 * Even though OpenWatcom support this directly, some assembly language 'glue'
 * is needed to to set up the segments and switch to a local stack.
 *
 *  far_draw()
 *      ax = segment of long pointer to PARMBLK
 *      bx = offset of long pointer to PARMBLK
 */

MLOCAL WORD __declspec( naked ) _far_draw(WORD seg, WORD ofs)
{
    _asm{
        /* save registers in the stack */
        push ds
        push dx
        /* set DS to the local data segment */
        mov dx, seg newstack
        mov ds, dx
        /* save current SS:SP */
        mov save_ss, ss
        mov save_sp, sp
        /* set stack to newstack */
        mov ss, dx
        mov sp, offset newstack + NEWSTACK_SIZ - 2
        /* The caller expects the parameter to be in DX:AX so make it so. */
        mov dx, ax
        mov ax, bx
        /* _dr_code calls the user function and returns value in AX */
#ifdef __SMALL__        
        call _dr_code
#else
        callf _dr_code
        mov dx, seg newstack
        mov ds, dx
#endif
        /* Restore SS:SP */
        mov ss, save_ss
        mov sp, save_sp
        /* Restore registers and return */
        pop dx
        pop ds
        retf
    };
}
#pragma aux far_draw parm [ax] [bx] value [ax];

MLOCAL WORD _dr_code(LPPARM drawpar)			
{	
	LPPBLK pBLK;
	
	/* The parameter passed back to us in drawpar is the address of 
	 * a WCCUBLK. */			
	pBLK = drawpar->pb_parm;
	/* The caller expects the parameter to be the ub_parm field in the
	 * WCCUBLK, so make it so. */
	drawpar->pb_parm = pBLK->ub_parm;
	/* And the caller's code is at pBLK->ub_code */
	return (pBLK->ub_code)(drawpar);
}


VOID wcc_userdef(LPTREE tree, WORD nobj, LPPBLK ub)
{
	tree[nobj].ob_type = ((tree[nobj].ob_type & 0xFF00) | G_USERDEF);

	ub->ub_reserved[0] = _far_draw; /* Draw handler function */
	ub->ub_reserved[1] = ub;		/* Parameter = address of userblk */

	tree[nobj].ob_spec = ub;
}

	


