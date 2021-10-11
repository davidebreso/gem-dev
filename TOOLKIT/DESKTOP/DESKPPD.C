/*	DESKTOP.C	13-11-2001 John Elliott		*/
/*
*       Copyright 2001 John Elliott                                     
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*/

#include "ppddesk.h"

/* Previous versions of DESKTOP tried to map sound() directly to v_sound().
 * This does not work; DESKTOP's sound() binding corresponds to both v_sound()
 * and vs_mute() */

WORD desk_sound( WORD isfreq, WORD frequency, WORD duration )
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



WORD wmax(WORD a, WORD b)
{
	return (a > b) ? a : b;
}

WORD wmin(WORD a, WORD b)
{
	return (a < b) ? a : b;
}

extern VOID dos_lpvoid(UWORD, LPVOID);


VOID dos_label(BYTE drive, BYTE *plabel)
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


WORD LSTCPY(LPBYTE d, LPBYTE s)
{
	while (*s) 
	{
		 *(d++) = *(s++);
	}
	*d = 0;
	return 1;
}


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



VOID r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph)
{
	*px = pxywh->g_x;
	*py = pxywh->g_y;
	*pw = pxywh->g_w;
	*ph = pxywh->g_h;
}



VOID r_set(GRECT *pxywh, WORD x, WORD y, WORD w, WORD h)
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


VOID rc_union(LPGRECT p1, LPGRECT p2)
	{
	WORD		tx, ty, tw, th;

	tw = wmax(p1->g_x + p1->g_w, p2->g_x + p2->g_w);
	th = wmax(p1->g_y + p1->g_h, p2->g_y + p2->g_h);
	tx = wmin(p1->g_x, p2->g_x);
	ty = wmin(p1->g_y, p2->g_y);
	p2->g_x = tx;
	p2->g_y = ty;
	p2->g_w = tw - tx;
	p2->g_h = th - ty;
	}


/* Get drive type. Based on BALJ's Desktop. */
WORD dos_dtype(WORD drive)
{
	union REGS rg;

	rg.x.ax = 0x0E00;	/* Select drive */
	rg.x.dx = drive;
	intdos(&rg, &rg);

	rg.x.ax = 0x1900;	
	intdos(&rg, &rg);
	if (rg.h.al != drive) 
	{
		return 0x0F;	/* Couldn't select it */
	}	
	if (drive + 'A' == 'B') 
	{
		rg.x.ax = 0x440E;
		rg.x.bx = 0;
		intdos(&rg, &rg);
		if (rg.h.al) 
		{
			return 0x0F;	/* Logical device on floppy 0 */
		}
	}
	rg.x.ax = 0x4408;
	rg.x.bx = drive + 1;
	intdos(&rg, &rg);
	if (rg.h.al == 0) 
	{
		return 0;	/* Floppy */
	}
	rg.x.ax = 0x4409;
	rg.x.bx = drive + 1;
	intdos(&rg, &rg);
	if (rg.x.cflag) 
	{
		return 0x0F;
	}
	if (rg.x.dx & 0x1000)  
	{
		return 2;			/* Network */
	}
	return 1;				/* HD */
}


#if DEBUG
static char buf[4096];

WORD form_valert(WORD button, BYTE *str, ...)
{
	va_list ap;
	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);
	return form_alert(button, ADDR(buf));
}

WORD dbg(BYTE *str, ...)
{
	FILE *fp = fopen("c:/gemapp.log", "a");
	
	va_list ap;
	va_start(ap, str);
	vfprintf(fp, str, ap);
	va_end(ap);

	fclose(fp);
}

extern WORD getcs(void);
extern WORD getip(void);

/* Dump system memory to disc, one byte at a time! 
 * Writes 1Mb of data, followed by four bytes far 
 * pointer to this function giving CS and IP.
 */
VOID crashdump(BYTE bt)
{
	char filename[20];
	long n;
	FILE *fp;
	LPBYTE ptr;
	WORD cs, ip;

	dbg("Creating crashdump %c\n", bt);
	sprintf(filename, "c:/gemapp_%c.mem", bt);
	fp = fopen(filename, "wb");
	for (n = 0; n < 0x100000L; n++)
	{
		ptr = MK_FP(n >> 4, n & 0x0F);
		fputc(ptr[0], fp);
	}
	cs = getcs();
	ip = getip();
	fwrite(&ip, 1, 2, fp);
	fwrite(&cs, 1, 2, fp);
	fclose(fp);

	dbg("Created crashdump %c as %s CS=%04x IP=%04x\n", bt, filename,
			cs, ip);
}

#endif



