/*	DESKTOP.C	13-11-2001 John Elliott		*/
/*
*       Copyright 2001 John Elliott                                     
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*/

#include "wccdesk.h"

/* Previous versions of DESKTOP tried to map sound() directly to v_sound().
 * This does not work; DESKTOP's sound() binding corresponds to both v_sound()
 * and vs_mute() */

WORD  desk_sound( WORD isfreq, WORD frequency, WORD duration )
{
    if (isfreq) return v_sound(gl_handle, frequency, duration);
    else        return vs_mute(gl_handle, frequency);
}


char *scasb(char *s, char b)
{
	char *t = strchr(s, b);

	if (t) return t;
	return s + strlen(s);
}



/**** replaced with WCC library functions ***
WORD  wmax(WORD a, WORD b)
{
	return (a > b) ? a : b;
}

MLOCAL WORD  wmin(WORD a, WORD b)
{
	return (a < b) ? a : b;
}
 */

extern VOID dos_lpvoid(UWORD, LPVOID);


VOID  dos_label(BYTE drive, BYTE *plabel)
{
	BYTE		label_buf[128];
	BYTE		ex_fcb[40];

	dos_sdta(ADDR(&label_buf[0]));
	ex_fcb[0] = -1;
	memset(ex_fcb + 1, 0, 5);
	ex_fcb[6] = 0x08;		/* volume label	*/
	ex_fcb[7] = drive;
	memset(ex_fcb + 8, '?', 11);
	memset(ex_fcb +19, 0,   21);

	dos_lpvoid(0x1100, ADDR(&ex_fcb[0]));

	if ( (DOS_AX & 0x00ff) == 0xff )
	  *plabel = 0;
	else
	{
	  label_buf[19] = 0x0;
	  strlcpy(plabel, label_buf + 8, 12);
	}
}


/**** replaced with WCC library function ***
WORD LSTCPY(LPBYTE d, LPBYTE s)
{
	while (*s) 
	{
		 *(d++) = *(s++);
	}
	*d = 0;
	return 1;
}
 */


VOID lstlcpy(LPBYTE d, LPBYTE s, WORD maxlen)
{
	while (*s) 
	{
		--maxlen;
		if (!maxlen) break;
		 *(d++) = *(s++);
	}
	*d = 0;
}




VOID  r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph)
{
	*px = pxywh->g_x;
	*py = pxywh->g_y;
	*pw = pxywh->g_w;
	*ph = pxywh->g_h;
}



VOID  r_set(GRECT *pxywh, WORD x, WORD y, WORD w, WORD h)
{
	pxywh->g_x = x;
	pxywh->g_y = y;
	pxywh->g_w = w;
	pxywh->g_h = h;
}

	UWORD  
inside(x, y, pt)		/* determine if x,y is in rectangle	*/
	WORD		x, y;
	GRECT		*pt;
	{
	if ( (x >= pt->g_x) && (y >= pt->g_y) &&
	    (x < pt->g_x + pt->g_w) && (y < pt->g_y + pt->g_h) )
		return(TRUE);
	else
		return(FALSE);
	} /* inside */


VOID  rc_union(LPGRECT p1, LPGRECT p2)
	{
	WORD		tx, ty, tw, th;

	tw = max(p1->g_x + p1->g_w, p2->g_x + p2->g_w);
	th = max(p1->g_y + p1->g_h, p2->g_y + p2->g_h);
	tx = min(p1->g_x, p2->g_x);
	ty = min(p1->g_y, p2->g_y);
	p2->g_x = tx;
	p2->g_y = ty;
	p2->g_w = tw - tx;
	p2->g_h = th - ty;
	}


/* Get drive type. Based on BALJ's Desktop. */
/* DB: rewrite in asm to reduce code size. */
__declspec( naked ) WORD  dos_dtype(WORD drive)
{
	_asm{
		mov dx, ax	/* param drive is now in DX */
		mov ax, 0x0E00	/* Select drive */
		int 0x21
		mov ax, 0x1900 	/* Get current disk */
		int 0x21 	/* current disk is now in AL */
		cmp al, dl	/* If different from drive, return error */
		jne ret_error
		cmp dx, 1	/* check if drive is 'B' */
		jne check_floppy
		mov ax, 0x440E	/* Get Logical Drive Map */
		mov bx,	0x0	/* BX = 0 is default drive */
		int 0x21	/* Logical dive map is now in AX */
		test ax, ax	
		jnz ret_error	/* if AX != 0, Logical device on floppy 0 */
	check_floppy:
		mov ax, 0x4408	/* Block Device Changeable */
		mov bx, dx	/* BX is drive */
		inc bx		/* BX is drive + 1 */
		int 0x21	/* Fixed Code is now in AX */
		test ax, ax	/* Fixed Code 00h = Removable */
		jne check_network
		mov ax, 0	/* Floppy, return 0 */
		ret
	check_network:
		mov ax, 0x4409	/* Block Device Local */
				/* BX is already drive + 1 */
		int 0x21	/* Attribute Word is now in DX */
		jc ret_error	/* if CF set, return error */
		and dx, 0x1000	/* check if bit 12 of the attribute word is set */
		jz is_hd	/* bit is not set, drive is HD */
		mov ax, 2	/* Network, return 2 */
		ret
	is_hd:	mov ax, 1	/* HD, return 1 */
		ret
	ret_error:
		mov ax, 0x0F
		ret
	};
}
#pragma aux dos_dtype parm [ax] value [ax] modify exact [ax bx cx dx] nomemory;






