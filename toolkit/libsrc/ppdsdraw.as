
;  FARDRAW .ASM   3/22/85     Tim Oren
;  SFARDRAW.AS    7 Jan 1998  John Elliott
;
;  This is a callback function to which GEM passes parameters in AX and BX.
; Since Pacific C does not support this directly, assembly language 'glue'
; code is used.
;
;
;	far_draw()
;		ax = hi part of long pointer to PARMBLK
;		bx = lo part of long pointer to PARMBLK
;
;	need to save the regs, get on a local stack, and call
;	the dr_code() routine with the parameters pointed at
;	in PARMBLK
;
;   [JCE] In Pacific C, we can use built-in compiler support to set up
;   the segments and switch to a local stack; dr_code has to be declared
;   as 'interrupt'. So all the asm code has to do is handle parameters.
;
;   In Pacific C the best way of doing this is to store the
;   parameters in a global variable (drawparam). To get a
;   result out, we use the global variable drawret in the same
;   way. 

	_DATA SEGMENT WORD PUBLIC 'DATA'
	_DATA ENDS

	_TEXT SEGMENT BYTE PUBLIC 'CODE'
	ASSUME   cs:_TEXT, ds:_DATA

	PUBLIC	__far_draw
	EXTRN   _dr_code_:	far

__far_draw	PROC FAR
	push	ds
	push	ax				;Set DS to our data, so that we can write to 
	mov	ax,seg __drawpar	;variables like drawpar.
	mov		ds,ax
	pop		ax
	mov		word ptr __drawpar, bx		;low word
	mov		word ptr __drawpar + 2, ax	;high word

	;pushf			 ;dr_code will return with an iret, so simulate an 
	callf	_dr_code_ ;interrupt call.
	
	mov		ax,__drawret
	pop		ds
	retf
__far_draw	ENDP	

_TEXT		ENDS

_DATA SEGMENT WORD PUBLIC 'DATA'

	PUBLIC	__drawaddr
	PUBLIC	__drawpar
	PUBLIC	__drawret

__drawaddr:
	dw	__far_draw,seg (__far_draw)
__drawpar:
	dw	0,0
__drawret:
	dw	0

_DATA		ENDS

END


