;/*	DESKOSIF.A86	4/18/84 - 04/08/85	Lee Jay Lorenzen	*/
;
;
;/*
;*       Copyright 1999, Caldera Thin Clients, Inc.                      
;*       This software is licenced under the GNU Public License.         
;*       Please see LICENSE.TXT for further information.                 
;*                                                                       
;*                  Historical Copyright                                 
;*	-------------------------------------------------------------
;*	GEM Desktop					  Version 2.1
;*	Serial No.  XXXX-0000-654321		  All Rights Reserved
;*	Copyright (C) 1985, 1986		Digital Research Inc.
;*	-------------------------------------------------------------
;*/
;
KEYENTRY	equ	016h
VIDENTRY	equ	010h
DOSENTRY	equ	021h
;
;
	.psect	_TEXT,class=CODE
	.globl	_givekey
	.globl	_givedos
	.globl	_givevid
	.globl	_takekey
	.globl	_takedos
	.globl	_takevid

dossav:	dw	0,0	
keysav:	dw	0,0	
vidsav:	dw	0,0	
keycnt:	dw	0
f10cnt:	dw	0
keytab:	dw	01c0dh			; carriage return
	dw	0314eh			; N
	dw	01c0dh			; carriage return
;	dw	02e03h			;ctrl-c
	dw	0			;end of list is null
	dw	0			;end of list is null
	dw	0			;end of list is null
	dw	0			;end of list is null
	dw	0			;end of list is null

;
chrout:
	push	bp
	mov		bp,sp
	mov		dx,4[bp]
	mov		ax,#0200h
	int		#21h
	pop		bp
	ret

;
chrin:
	push	bp
	mov		bp,sp
chr2:		mov	ax,#0600h
	mov 	dl, #0FFH
	int		#21h
	jz 		chr2
	xor 	ah,ah	
	pop		bp
	ret
;
;
;
cxc33:
	push	bx
	push	dx
	cmp		ax,cx
	jnz		cxc1
	shr		bx,#1
	shr		dx,#1
	cmp		bx,dx
	jnz		cxc1
	pop		dx
	pop		bx
	cmp		bx,dx
	jmp		cxc2
cxc1:
	pop		dx
	pop		bx
cxc2:
	ret
;
;
cxd33:
	xchg	ax,bx
	mov		cx,dx
	jcxz	div0
	cmp		cx,#1
	jne		not_div1
	xchg	ax,bx
	xor		cx,cx
	mov		dx,cx
	ret
not_div1:
	xor	dx,dx
	xchg	ax,bx
	push	ax
	div		cx
	mov		di,ax
	mul		cx
	pop		dx
	sub		dx,ax
	mov		ax,bx
	div		cx
	sub		cx,cx
	mov		bx,ax
	mov		ax,di
	ret
div0:
	mov		ax,#-1
	mov		bx,ax
	ret
;
;
cxm33:
	push	dx
	mul		dx
	pop		dx
	push	ax
	mov		ax,bx
	push	dx
	mul		cx
	mov		cx,ax
	mov		ax,bx
	pop		dx
	mul		dx
	add		dx,cx
	pop		cx
	add		dx,cx
	mov		bx,ax
	mov		ax,dx
	ret
;
;
;
inttake:
	push	es
	mov		ah,#035h		; set fn # (int# passed in al)
	int 	#021h		; get current vector
	mov		cs:2[bp],es	; saving current values
	mov		cs:0[bp],bx
	pop		es
intset:
	push	ds
	mov		bx,cs			; new vector seg is current cseg
	mov		ds,bx	
	mov		ah,#025h		; new vector offset already in dx
	int		#021h
	pop		ds
	ret
;
;
;
intgive:
	push	ds
	mov		dx,cs:0[bp]	; set up old offset
	mov		ds,cs:2[bp]	; and segbase
	mov		ah,#025h			; int # already in al
	int		#021h
	pop		ds
	ret
;
;
;
;
;
;
keycod:
	cmp		ah,#00h				; if keyboard input
	jz		dinkey
	cmp		ah,#01h				; if con out
	jz		dinkey
	jmpf	[keysav]		; let other guy do iret
;
;
vidcod:
	iret
;
;
;
clrky:
	mov		ah,al
	cmp 	ah,#0
	jg		chkhi
	iret
chkhi:
	cmp		ah,#0ah
	jle		doscod
	iret	
;
;
doscod:
	cmp	ah,#01h				; if keyboard input
	jz	dinkey
	cmp	ah,#02h				; if con out
	jz	doutstr
	cmp	ah,#06h				; if direct i/o
	jz	ddirio
	cmp	ah,#07h				; if no echo
	jz	ddirio
	cmp	ah,#08h				; if keyboard input
	jz	dofunc8
	cmp	ah,#09h				; if console output
	jz	doutstr
	cmp	ah,#0ah				; if buffered input
	jz	dofunc10
	cmp	ah,#0bh				; if console status
	jz	csj
	cmp	ah,#0ch				; if clear keyboard
	jz	clrky
	jmpf	[dossav]		; let other guy do iret
;
csj:	jmp	csout
;
ddirio:
	cmp	dl,#0ffh
	jnz	doutstr				; ignore output
dinkey:
	push	bx
	mov	ax, keycnt
	mov	bx, #keytab
	add	bx, ax
	inc	ax
	inc	ax
	mov	keycnt, ax
	mov	ax, cs:[bx]
	pop	bx
	cmp	ax, #0
	jnz	doutstr
	mov	keycnt, ax
	jmp	dinkey
doutstr:
	sti
	retf	#2
;
;
dofunc8:
	inc		f10cnt,word
	cmp		f10cnt,#3,word
	jl		dof8
	mov		ax,#4c00h		; terminate the guy
	jmpf	[dossav]
dof8:
	jmp	dinkey
;
;
dofunc10:
	inc		f10cnt,word
	cmp		f10cnt,#3,word
	jl		dof10
	mov		ax,#4c00h		; terminate the guy
	jmpf	[dossav]

dof10:
	push	bx
	mov		bx,dx
	inc		bx				; skip buffer size
	mov		ds:[bx],#1,byte	; say one char was read
	inc		bx
	mov		ds:[bx],#04eh,byte	; stuff the one char
	inc		bx
	mov		ds:[bx],#00dh,byte	; add on carriage return
	pop	bx
	jmp	doutstr
;
;
csout:
	mov	al,0ffh
	iret
;
;
;
;
_takekey:
	mov		keycnt,#0,word
	mov		f10cnt,#0,word
	cli
	push	bp
	mov		al,KEYENTRY
	mov		bp,#keysav
	mov		dx,#keycod
	call	inttake
	pop		bp
	sti
	ret
;
;
;
_givekey:
	cli
	push	bp
	mov		al,KEYENTRY
	mov		bp,#keysav
	call	intgive
	pop		bp
	sti
	mov		ax,f10cnt
	ret
;
;
_takevid:
	cli
	push	bp
	mov		al,VIDENTRY
	mov		bp,#vidsav
	mov		dx,#vidcod
	call	inttake
	pop		bp
	sti
	ret
;
;
;
_givevid:
	cli
	push	bp
	mov		al,VIDENTRY
	mov		bp,#vidsav
	call	intgive
	pop		bp
	sti
	ret
;
;
;
_takedos:
	cli
	push	bp
	mov		al,DOSENTRY
	mov		bp,#dossav
	mov		dx,#doscod
	call	inttake
	pop	bp
	sti
	ret
;
;
;
_givedos:
	cli
	push	bp
	mov		al,DOSENTRY
	mov		bp,#dossav
	call	intgive
	pop		bp
	sti
	ret
;

