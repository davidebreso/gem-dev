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
void (__interrupt __far *_old_ef)() = NULL;
static BYTE 	_reentry = 0;
static WORD		_chain = 0;
	
static AESFUNC AES_C8  = NULL;
static AESFUNC AES_C9  = NULL;
static VDIFUNC VDI_473 = NULL;
static AESFUNC oldaes  = NULL;
static VDIFUNC oldvdi  = NULL;	

static WORD _gem_old(void)
{
_asm 
{
	push	ds
	push	es

;	int		#3
	mov		ax,_axpar
	push	ax
	mov		bx,offset _aespar
	mov		cx,_funcpar
	mov		dx,offset _vdipar
	
	mov		ax,offset _aespar + 2
	mov		es,ax
	mov		ax,offset _vdipar + 2
	mov		ds,ax
	pop		ax		;AX parameter
	pushf
	callf	_old_ef;

	pop		es
	pop		ds
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

	
void interrupt _gem_hook(void)
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

/********

asm {

#ifdef LARGE_MODEL
	.globl	large_data
	.globl	large_code
	.psect	ltext,local,class=CODE,reloc=16,size=65535
#else
	.globl	small_data
	.globl	small_code
	.psect	_TEXT,class=CODE
#endif

	.globl	__gem_call
	.globl  __gem_hook
	.globl	__gem_old
*****/
	
/*
 *This is our handler for INT 0xEF.
 */
void __interrupt __far _gem_call()
{
_asm
{
	jmp		gcall1
	byte	'G','E','M','A','E','S','2','0',0

gcall1:
	push	ds				;Set DS to our data, so that we can write to
	push	ax				;local variables like _aespar. 
	mov	ax,#seg _funcpar
	mov		ds,ax
;	int		#3
	mov		al,_reentry
	or		al,al			;Re-entrant call?
	jnz		gopast
	
	pop		ax				;AX = caller AX
	mov		_axpar,ax
	pop		ax				;AX = caller DS
	push	ax

	mov	_funcpar,    cx	;0x473 for VDI, 0xC8 or 0xC9 for AES
	mov	word ptr _vdipar,     dx
	mov	word ptr _vdipar + 2, ax	;VDI parameter, far pointer in ds:dx
	mov	word ptr _aespar,     bx
	mov	ax,es	
	mov	word ptr _aespar + 2, ax	;AES parameter, far pointer in es:bx

	push	bp
	pushf			 		;gem_hook will return with an iret, so simulate 
	callf	_gem_hook 		;an interrupt call.
	pop		bp
	
	mov		ax,_chain
	or		ax,ax
	jnz		dochain
	
	mov		ax,_gemret
	pop	ds
	iret
;
dochain:
	mov		ax,_axpar
	push	ax
	mov		bx,offset _aespar
	mov		cx,_funcpar
	mov		dx,offset _vdipar
	
	mov		ax,offset _aespar + 2
	mov		es,ax
	mov		ax,offset _vdipar + 2
	mov		ds,ax

gopast:
	pop		ax
	pop		ds
	jmpf	_old_ef
};
}
