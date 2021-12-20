/* DESKCDS.C - Desktop CD-ROM support code              */

/* This file provide the Desktop with some support for  */
/* CD/DVD drives - basically, anything that interfaces  */
/* with DOS through the MSCDEX requester/redirector (or */
/* its counterpart in other DOSes, e.g. Caldera NWCDEX) */

/* (C)opyright Ben A L Jemmett 1999.  This software is  */
/* released under the GNU Public License.  See the file */
/* LICENSE.TXT (included) for more information.         */

/* Changes:                                             */
/* 19990831 - BALJ - File created, cd_isdrvcd() added.  */
/* 19991230 - BALJ - Finally got it working!            */

#include "ppddesk.h"

/********************************************************
 * External function definitions                        * 
 ********************************************************/

/* [JCE Port Ben's cdexinst() to C rather than asm.] */
/* [DB 2021, rewrite in asm to reduce code size ] */
MLOCAL __declspec( naked ) WORD cdexinst()
{
/*	union REGS rg;

	rg.x.ax = 0x1500;
	rg.x.bx = 0x0000;
	int86(0x2f, &rg, &rg);
	rg.h.ch = rg.h.bl;
	return rg.x.cx;		 Returns (BL << 8) | CL */
	_asm{
		mov ax, 0x1500
		mov bx, 0x0000
		int 0x2f
		mov ch, bl	/* Returns (BL << 8) | CL */
		ret
	}
}
#pragma aux cdexinst value [cx] modify exact [ax bx cx] nomemory;

/********************************************************
 * cd_isdrvcd ( drvno );                                *
 * -- Returns TRUE if drive drvno (0=A) is a CD/DVD.    *
 *    BALJ, 1999831                                     *
 ********************************************************/
BOOLEAN __near cd_isdrvcd( BYTE drvno )
{
        WORD    cdinfo;
        BYTE    numcds, firstcd;
        BOOLEAN ret;

        cdinfo = cdexinst();

        numcds = ( cdinfo & 0xFF00 ) / 0x100;
        firstcd = cdinfo & 0xFF;

        ret = FALSE;

        if ( numcds )
          if ( (drvno >= firstcd) && (drvno < ( firstcd + numcds ) ) )
            ret = TRUE;

        return(ret);
}       /* cd_isdrvcd() */
