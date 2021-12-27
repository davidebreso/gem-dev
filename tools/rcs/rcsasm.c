
#include "wccrcs.h"

/************************************************
 *	b_move(&from, count, &to);					*
 *		&from:		DX:SI				*
 *		count:		CX				*
 *		&to:		ES:DI				*
 ************************************************/
__declspec( naked ) void B_MOVE(LPVOID from, WORD count, LPVOID to)
{
_asm{
	// Save registers
	push	ds
	push	cx
	push	di
	push	si
	mov	ds, dx	// ds is now segment of source
	rep 	movsb	// move  cx bytes from ds:si to es:di
	// Restore and exit.
	pop	si
	pop	di
	pop	cx
	pop	ds
#ifdef __SMALL__
	ret
#else
	retf
#endif
};
}
#pragma aux B_MOVE parm [dx si] [cx] [es di] modify exact [ ] nomemory;


/***********************************************************
 * MAGNIFY(src_mp, dst_mp, npix)
 * expand bits to bytes from source to dest
 *		src_mp:		DX:SI				*
 *		dst_mp:		ES:DI				*
 *		npix:		CX				*
 ***********************************************************/
__declspec( naked ) void MAGNIFY(LPVOID src_mp, LPVOID dst_mp, WORD npix)
{
_asm{
	// save registers
	push	ds
	push	si
	push	di
	push	dx
	push	cx
	push	bx
	push	ax

	mov	bx,0x8000
	shr	cx, 1		// npix / 2
	mov	ds, dx		// source is now in ds:si
				// and destination is in es:di
	mov	dx,[si]		// load first word of source
	inc	si
	inc	si
magloop:
	xor	ax,ax
	test	dx,bx
	jz	hi0
	not	ah
//	dec	ah	//this is for white lines around fat pels
hi0:	
	ror	bx, 1
	test	dx,bx
	jz	low0
	not	al
//	dec	al
low0:
	stosw
	ror	bx, 1
	jc	next_source
	loop	magloop
	jmp	magdone

next_source:
	mov	dx,[si]
	inc	si
	inc	si
	loop	magloop

magdone:
	// Restore and exit.
	pop	ax
	pop	bx
	pop	cx
	pop	dx
	pop	di
	pop	si
	pop	ds
#ifdef __SMALL__
	ret
#else
	retf
#endif
};
}
#pragma aux MAGNIFY parm [dx si] [es di] [cx] modify exact [ ] nomemory;



/*

;************************************************
;	b_move(&from, count, &to);					*
;		&from:		ADDR	4[bp]				*
;		count:		WORD	8[bp]				*
;		&to:		ADDR	10[bp]				*
;************************************************
B_MOVE:
	push	bp
	mov	bp, sp
	mov	bx, ds

; Move from the source to the destination.
	mov	cx, 8 + 2[bp]		; large code +2
	les	di, 10 + 2[bp]		; es:di = address of destination
	lds	si, 4 + 2[bp]		; ds:si = address of source
	rep	movsb			; move bytes

; Restore and exit.
	mov	ds, bx
	pop	bp
	ret
;
;***********************************************************
;MAGNIFY(src_mp, dst_mp, npix)
;expand bits to bytes from source to dest
;***********************************************************
MAGNIFY:
	push	bp
	mov	bp,sp
	mov	bx,8000h
	mov	cx,12 + 2[bp]
	shr	cx,1
	push	ds
	push	es
	lds	si,4 + 2[bp]
	les	di,8 + 2[bp]
	mov	dx,[si]
	inc	si
	inc	si
magloop:
	xor	ax,ax
	test	dx,bx
	jz	hi0
	not	ah
;	dec	ah		;this is for white lines around fat pels
hi0:	ror	bx,1
	test	dx,bx
	jz	low0
	not	al
;	dec	al
low0:	stosw
	ror	bx,1
	jc	next_source
	loop	magloop
	jmp	magdone
;
next_source:
	mov	dx,[si]
	inc	si
	inc	si
	loop	magloop
;
magdone:
	pop	es
	pop	ds
	pop	bp
	ret
;

CODE ENDS
END
*/

