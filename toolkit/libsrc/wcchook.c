/*
 * This contains a number of functions to help a program (normally a DA) to
 * intercept calls made to GEM.
 *
 */

#include "wccgem.h"
#include <dos.h>

#define GEM_VECTOR 0xEF


static WORD	_axpar = 0;
static LPGEMBLK _aespar = NULL;
static LPGSXPAR _vdipar = NULL;
static WORD     _funcpar = 0;
static WORD     _gemret = 0;
// void (__interrupt __far *_hookaddr)() = NULL;
void __interrupt __far _gem_call();
static BYTE 	_reentry = 0;
static WORD		_chain = 0;
	
static AESFUNC AES_C8  = NULL;
static AESFUNC AES_C9  = NULL;
static VDIFUNC VDI_473 = NULL;
static AESFUNC oldaes  = NULL;
static VDIFUNC oldvdi  = NULL;	

#define NEWSTACK_SIZ 1024
MLOCAL BYTE newstack[NEWSTACK_SIZ];
MLOCAL WORD save_ss, save_sp;

/* the pointer to the old GEM interrupt handler should be in the code segment,
 * so that it can be accessed easily by the new interrupt handler
 */
void  (__interrupt __far * __based( __segname( "_CODE" ) ) _old_ef)() = NULL;

__declspec ( naked ) static WORD _gem_old(void)
{
_asm 
{
	push	ds
	push	es
//	int		3
	mov		ax,_axpar
	push	ax
	mov		bx,word ptr _aespar
	mov		cx,_funcpar
	mov		dx,word ptr _vdipar
	
	mov		ax,word ptr _aespar + 2
	mov		es,ax
	mov		ax,word ptr _vdipar + 2
	mov		ds,ax
	pop		ax		;AX parameter
	pushf
	callf 	dword ptr CS:_old_ef;

	pop		es
	pop		ds
	ret
};
}

static WORD newvdi(LPGSXPAR gb)
{
	_vdipar  = gb;
	_funcpar = 0x473;
	return _gem_old();
}

static WORD newaes(LPGEMBLK gb)
{
	_aespar  = gb;
	_funcpar = 0xC8;
	return _gem_old();
}

	
void _gem_hook(void)
{	
	_reentry++;
	_chain  = 1;
	_gemret = 0;
	switch(_funcpar)
	{
		case 0xC8:  if (AES_C8)  _chain  = (*AES_C8)(_aespar);
		 		    else		 _chain  = 1;
				    break;
		case 0xC9:  if (AES_C8)  _chain  = (*AES_C9)(_aespar);
		 		    else		 _chain  = 1;
				    break;
		case 0x473: if (VDI_473) _chain  = (*VDI_473)(_vdipar);
		 		    else	     _chain  = 1;
				    break;
		default:	break;
	}
	_reentry--;
}	


VOID wcc_setresult(WORD r)
{
	_gemret = r;
}



VOID wcc_hookon(AESFUNC pAes, AESFUNC pAes2, VDIFUNC pVdi)
{
	AES_C8  = pAes;
	AES_C9  = pAes2;
	VDI_473 = pVdi;

	if (!_old_ef)
	{
		_old_ef = _dos_getvect(GEM_VECTOR);
		_dos_setvect(GEM_VECTOR, _gem_call);
	}
	oldaes = divert_aes(newaes);
	oldvdi = divert_vdi(newvdi);
}


VOID wcc_hookoff(VOID)
{
	if (_old_ef) _dos_setvect(GEM_VECTOR, _old_ef);
	_old_ef  = NULL;
	AES_C8  = NULL;
	AES_C9  = NULL;
	VDI_473 = NULL;
	divert_aes(oldaes);
	divert_vdi(oldvdi);
	oldaes = NULL;
	oldvdi = NULL;
}

	
/*
 *This is our handler for INT 0xEF.
 */
__declspec( naked ) void __interrupt __far _gem_call()
{
_asm{
	jmp		short gcall1
	db		'G','E','M','A','E','S','2','0',0
gcall1:
	/* Save registers */
	push	ds
	push	ax
	/* Set DS to our data, so that we can write to local variables like _reentry. */ 
	mov	ax,seg _reentry
	mov	ds,ax
	mov	al,_reentry
	or	al,al			/* Re-entrant call? */
	jnz	gopast	
	pop		ax			/* AX = caller AX	*/
	mov		_axpar,ax
	pop		ax			/* AX = caller DS	*/
	push	ax
	mov	_funcpar, cx		/* 0x473 for VDI, 0xC8 or 0xC9 for AES */
	mov	word ptr _vdipar,     dx
	mov	word ptr _vdipar + 2, ax	/* VDI parameter, far pointer in ds:dx */
	mov	word ptr _aespar,     bx
	mov	ax,es	
	mov	word ptr _aespar + 2, ax	/* AES parameter, far pointer in es:bx */

	push	bp
	/* save current SS:SP */
	mov save_ss, ss
	mov save_sp, sp
	/* set stack to newstack */
	mov ax, seg newstack
	mov ss, ax
	mov sp, offset newstack + NEWSTACK_SIZ - 2
#if defined( __SMALL__ ) || defined( __COMPACT__ )
	call _gem_hook
#else
	callf _gem_hook
	/* Restore DS since in the large model _gem_hook may change it */
	mov dx, seg newstack
	mov ds, dx
#endif
	/* Restore SS:SP */
	mov ss, save_ss
	mov sp, save_sp
	pop		bp
    /* If _chain = 0, restore registers and return, otherwise call old EF handler */	
	mov		ax,_chain
	or		ax,ax
	jnz		dochain
	mov		ax,_gemret
    /* Restore registers and return */
	pop	ds
	iret

dochain:
	/* Restore registers and call old EF int handler */
	mov		ax,_axpar
	push	ax
	mov		bx,word ptr _aespar
	mov		cx,_funcpar
	mov		dx,word ptr _vdipar	
	mov		ax,word ptr _aespar + 2
	mov		es,ax
	mov		ax,word ptr _vdipar + 2
	mov		ds,ax
gopast:
	pop		ax
	pop		ds
	jmpf	dword ptr CS:_old_ef
};
}

