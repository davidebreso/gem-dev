#include "wccrcs.h"

//	.globl	large_data
//	.globl	large_code
//	.psect	ltext,local,class=CODE,reloc=16,size=65535

/*************************************************************************** 
 *						
 * MUL_DIV (m1,m2,d1)
 *
 * ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2 
 * m1 = signed 16 bit integer 
 * m2 = unsigned 15 bit integer 
 * d1 = signed 16 bit integer
 *
 ****************************************************************************/


//	.globl	_mul_div
//	.signat	_mul_div,12346
__declspec( naked ) WORD mul_div (WORD m1, UWORD m2, WORD d1)
{
			// ax = m1
			// dx = m2
			// bx = d1
_asm{
 	xchg		ax,dx	// ax = m2, dx = m1
	shl		ax, 1	 // m2 * 2
	imul	dx		 // m2 * 2 * m1
	idiv	bx		 // m2 * 2 * m1 / d1
	and		ax,ax	 //set flags
	js		loc1
	inc		ax
	shr		ax, 1
	retf	
loc1:	add		ax,-1
	neg		ax
	shr		ax,1
	neg		ax
#ifdef __SMALL__
	ret	
#else
	retf
#endif	
};
}

/*************************************************************************** 
 * 
 * UMUL_DIV (m1,m2,d1) 
 * 
 * ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2 
 * m1 = usigned 16 bit integer 
 * m2 = unsigned 15 bit integer 
 * d1 = usigned 16 bit integer 
 *
 ****************************************************************************/


//	.globl	_umul_div
//	.signat	_umul_div,12346
__declspec( naked ) UWORD umul_div(UWORD m1, UWORD m2, WORD d1)
{
			// ax = m1
			// dx = m2
			// bx = d1
_asm{
 	xchg		ax,dx	// ax = m2, dx = m1
	shl		ax, 1	// m2 * 2
	mul		dx	// m2 * 2 * m1
	div		bx	// m2 * 2 * m1 / d1
	inc		ax	// ( ( m1 * 2 * m2 ) / d1 ) + 1
	shr		ax, 1	// ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2
#ifdef __SMALL__
	ret	
#else
	retf
#endif	
};
}
