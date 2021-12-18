/*	 DESKAPP.C	06/11/84 - 07/11/85		Lee Lorenzen	*/
/*	for 3.0		3/6/86   - 5/6/86		MDF		*/
/*	for 2.3		9/25/87				mdf		*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "ppddesk.h"

#define MIN_WINT 4
#define MIN_HINT 2

/*
#if DEBUG
void dump_anodes(char *s)
{
	ANODE *pa;
	
	dbg("dump_anodes(%s):\n", s);
	
	for (pa = G.g_ahead; pa; pa = pa->a_next)
	{
		dbg("    flags=%04x type=%04x obid=%04x aicon=%04x dicon=%04x %c %s %s\n",
				pa->a_flags, pa->a_type, pa->a_obid, pa->a_aicon,
				pa->a_dicon, pa->a_letter, pa->a_pappl, pa->a_pdata);
	}
	
}
#endif
*/

/* strcpy() with length bound */
/* 
VOID strlcpy(char *dest, char *src, int len)
{
	if (len < 1) return;
	if (strlen(src) < len)
	{
		strcpy(dest, src);
		return;
	}
	strncpy(dest, src, len-1);
	dest[len-1] = 0;	
}

VOID strlcat(char *dest, char *src, int len)
{
	strlcpy(dest + strlen(dest), src, len - strlen(dest));
}
 */

/*
*	Allocate an application object.
*/
ANODE *app_alloc(WORD tohead)
{
	ANODE		*pa, *ptmpa;

	pa = G.g_aavail;
	if (pa)
	{
	  G.g_aavail = pa->a_next;
	  if ( (tohead) ||
	       (!G.g_ahead) )
	  {
	    pa->a_next = G.g_ahead;
	    G.g_ahead = pa;
	  }
	  else
	  {
	    ptmpa = G.g_ahead;
	    while( ptmpa->a_next )
	      ptmpa = ptmpa->a_next;
	    ptmpa->a_next = pa;
	    pa->a_next = (ANODE *) NULL;
	  }
	}
	return(pa);
}


/*
*	Free an application object.
*/

VOID app_free(ANODE *pa)
{
	ANODE		*ptmpa;

	if (G.g_ahead == pa)
	  G.g_ahead = pa->a_next;
	else
	{
	  ptmpa = G.g_ahead;
	  while ( (ptmpa) &&
		  (ptmpa->a_next != pa) )
	    ptmpa = ptmpa->a_next;
	  if (ptmpa)
	    ptmpa->a_next = pa->a_next;
	}
	pa->a_next = G.g_aavail;
	G.g_aavail = pa;
}


/*
*	Convert a single hex ASCII digit to a number
*/

WORD hex_dig(BYTE achar)
{
	if ( (achar >= '0') && (achar <= '9') ) return(achar - '0');	
	if ( (achar >= 'A') && (achar <= 'F') ) return(achar - 'A' + 10);
	return(0);
}
	

/*
*	Reverse of hex_dig().
*/

BYTE uhex_dig(WORD wd)
{
	if ( (wd >= 0   ) && (wd <= 9   ) ) return(wd + '0');	
	if ( (wd >= 0x0a) && (wd <= 0x0f) ) return(wd + 'A' - 0x0a);
	return(' ');
}
	

/*
*	Reverse of scan_2().
*/

BYTE *save_2(BYTE *pcurr, UWORD wd)
{
	*pcurr++ = uhex_dig((wd >> 4) & 0x000f);
	*pcurr++ = uhex_dig(wd & 0x000f);
	return(	pcurr );
}

#if MULTIAPP


/*
*	Scan off and convert the next four hex digits and return with
*	pcurr pointing one space past the end of the four hex digits.
*	Start of field is marked with an 'R'.  If no field, set it to
*	default memory size -- DEFMEMREQ.
*/

BYTE *scan_memsz(BYTE *pcurr, UWORD *pwd)
{
	UWORD		temp1, temp2;
	
	temp1 = 0x0;
	while (*pcurr == ' ')
	  pcurr++;
	if (*pcurr == 'R')
	{
	  pcurr++;				
	  pcurr = scan_2(pcurr, (WORD *)&temp1);	/* hi byte	*/
	  pcurr = scan_2(pcurr, (WORD *)&temp2);	/* lo byte	*/
	  temp1 = ((temp1 << 8) & 0xff00) | temp2;
	}
	if (temp1 == 0)
	  temp1 = DEFMEMREQ;
	*pwd = temp1;
	return(	pcurr );
}

/*
*	Reverse of scan_memsz().
*/

BYTE *save_memsz(BYTE *pcurr, UWORD wd)
{
	*pcurr++ = 'R';
	pcurr = save_2(pcurr, LHIBT(wd));
	pcurr = save_2(pcurr, LLOBT(wd));
	return(	pcurr );
}

#endif

/*
*	Scan off spaces until a string is encountered.  An @ denotes
*	a null string.  Copy the string into a string buffer until
*	a @ is encountered.  This denotes the end of the string.  Advance
*	pcurr past the last byte of the string.
*
* 	BIG WARNING: Note that the terminator is an @ sign, not a
* 	0 byte. Passing a null-terminated string as pcurr will invoke 
* 	undefined behaviour.
*
*/

BYTE *scan_str(BYTE *pcurr, BYTE **ppstr)
{
	/* Skip over spaces */
	while(*pcurr == ' ') pcurr++;

	/* ppstr points to the start of the buffer in gl_buffer */
	*ppstr = G.g_pbuff;
	while(*pcurr != '@') 
	{
		*G.g_pbuff++ = *pcurr++;
		/* [JCE 20-8-2005] Don't allow an overflow */
		if (G.g_pbuff > (gl_buffer + SIZE_BUFF - 1)) 
		{
			break;
		}
	}
	*G.g_pbuff++ = 0;
	pcurr++;
	/* Returns the next free space in the buffer */
	return(pcurr);
}


/*
*	Reverse of scan_str.
*/

BYTE *save_str(BYTE *pcurr, BYTE *pstr)
{
	while(*pstr)
	  *pcurr++ = *pstr++;
	*pcurr++ = '@';
	*pcurr++ = ' ';
	return(pcurr);
}

/*
*       Autodetect disc drives - Ben A. L. Jemmett, 19990828
*/
VOID app_detect()
{
        WORD            i, ii, ndisks, dtype, dup;
        ANODE           *pa;
        BYTE            label[15];
        BYTE            statinf[43];
	WORD		drv_x, drv_y;
	WORD            sichar;
	BYTE 		*pstr;

	if ( !G.g_detdrives ) return;

/* The notional 'previous' drive is off the top */
	drv_x = G.g_xdesk + (G.g_wdesk - G.g_icw);
	drv_y = G.g_ydesk;
/* If there are any drives already present, set drv_x and drv_y to refer
 * to them. We assume later drives come lower down. */
    for (pa = G.g_ahead; pa ; pa=pa->a_next)
    {
	if (pa->a_type == AT_ISDISK)
	{
		ins_posdisk(pa->a_xspot, pa->a_yspot, &drv_x, &drv_y);
	}
    }

    strlcpy(statinf, ini_str(DRVCLINE), 43);    /* Get check string        */

    sichar = -1;                                /* Look for drive letter ^ */
    for(i = 0; i < NUM_ANODES; i++)             /* in the string           */
      if (statinf[i]=='^')
        sichar = i;

    ndisks = dos_sdrv(gl_stdrv);		/* get # of logical drives */
    // dbg("deskapp1.c:276 Error %d in dos_sdrv(%d)\n", DOS_ERR, gl_stdrv);
/*
#ifdef LIMITDRV
    ndisks = LIMITDRV;
#endif
*/
    for( ii = 0; ii < ndisks; ii++ )            /* Loop all disks       */
    { 
        dup = FALSE;
        for(i = 0; i < NUM_ANODES; i++)       /* Check for duplicate  */
          if ( G.g_alist[i].a_type == AT_ISDISK )
            if ( G.g_alist[i].a_letter == 'A' + ii )
              dup = TRUE; 

        if (!dup)
        {
          if (sichar != -1)
            statinf[sichar]='A'+ii;               /* Merge real letter    */
    
//XXX     status( (BYTE FAR *)statinf );          /* Display status       */
          dtype = dos_dtype( ii );                /* Find drive type      */

          if ( dtype != 0x0F )                    /* if drive is present  */
          {
            pa = app_alloc(TRUE);                 /* allocate an ANODE    */
	    if (!pa)
	    {
		break;
	    }
            pa->a_type = AT_ISDISK;
            pa->a_flags = AF_ISCRYS | AF_ISGRAF | AF_ISDESK | AF_WASDET;
            switch (dtype)
            {
                case 0:
                  if ( ii < 2 )              /* A: or B: - floppy disks  */
                  {
                    pa->a_aicon = IG_FLOPPY;
                    strlcpy(label, ini_str(DRVTFLOP), sizeof(label));
                  }
                  else                      /* C: or above - removable  */
                  {
                    pa->a_aicon = IG_5QRTR;
                    strlcpy(label, ini_str(DRVTREM), sizeof(label));
                  }
                  scan_str(label, &pa->a_pappl);
                  break;
                case 1:
                  pa->a_aicon = IG_HARD;
                  strlcpy(label, ini_str(DRVTHARD), sizeof(label));
                  scan_str(label, &pa->a_pappl);
                  break;
                case 2:
		  pstr = ini_str(DRVTNET);
                  strlcpy(label, pstr, sizeof(label));
                  pa->a_aicon = IG_NET;
                  if ( G.g_probedrives )
		  {
                    if ( cd_isdrvcd( ii ) )
                    {
                      pa->a_aicon = IG_CD;
                      strlcpy(label, ini_str(DRVTCD), sizeof(label));
                    }
		  }
                  scan_str(label, &pa->a_pappl);
                  break;
                default:
                  pa->a_aicon = IG_5QRTR;
                  strlcpy(label, ini_str(DRVTUNKN), sizeof(label));
                  scan_str(label, &pa->a_pappl);
                  break;
            }
            label[0]='@';
            label[1]=0;
            scan_str(label, &pa->a_pdata);
            pa->a_dicon = 0xFF;
            pa->a_letter = 'A' + ii;
	    pa->a_xspot = drv_x;
	    pa->a_yspot = drv_y;
	    ins_posdisk(pa->a_xspot, pa->a_yspot, &drv_x, &drv_y);
            pa = NULL;
          } /* if exists */
        } /* if !dup */
    } /* for ndisks */
    ndisks = dos_sdrv(gl_stdrv);		/* reset to start drive */
    // dbg("deskapp1.c:360 Error %d in dos_sdrv(%d)\n", DOS_ERR, gl_stdrv);

}       /* app_detect */


/*
*	Parse a single line from the DESKTOP.INF file.
*/

BYTE *app_parse(BYTE *pcurr, ANODE *pa)
{
	switch(*pcurr)
	{
	  case 'T':			    /* DESKTOP v1.2: Trash */
	    pa->a_type  = AT_ISTRSH;
	    pa->a_flags = AF_ISCRYS | AF_ISGRAF | AF_ISDESK;
	    break;
	  case 'M':				/* Storage Media	*/
		pa->a_type = AT_ISDISK;
		pa->a_flags = AF_ISCRYS | AF_ISGRAF | AF_ISDESK;
		break;
	  case 'G':				/* GEM App File		*/
		pa->a_type = AT_ISFILE;
		pa->a_flags = AF_ISCRYS | AF_ISGRAF;
		break;
	  case 'F':				/* DOS File no parms	*/
	  case 'f':				/*   needs full memory	*/
		pa->a_type = AT_ISFILE;
		pa->a_flags = (*pcurr == 'F') ? NONE : AF_ISFMEM;
		break;
	  case 'P':				/* DOS App needs parms	*/
	  case 'p':				/*   needs full memory	*/
		pa->a_type = AT_ISFILE;
		pa->a_flags = (*pcurr == 'P') ? 
				AF_ISPARM : AF_ISPARM | AF_ISFMEM;
		break;
	  case 'D':				/* Directory (Folder)	*/
		pa->a_type = AT_ISFOLD;
		break;
	}
	pcurr++;
	if (pa->a_flags & AF_ISDESK)
	{
	  pcurr = scan_2(pcurr, &pa->a_xspot);
	  pcurr = scan_2(pcurr, &pa->a_yspot);
	}
	pcurr = scan_2(pcurr, &pa->a_aicon);
	pcurr = scan_2(pcurr, &pa->a_dicon);
	pcurr++;
	if (pa->a_flags & AF_ISDESK)
	{
	  pa->a_letter = (*pcurr == ' ') ? 0 : *pcurr;
	  pcurr += 2;
	}
	pcurr = scan_str(pcurr, &pa->a_pappl);
	pcurr = scan_str(pcurr, &pa->a_pdata);
#if MULTIAPP
	if (!(pa->a_flags & AF_ISDESK))			/* only for apps */
	  pcurr = scan_memsz(pcurr, (UWORD *)&pa->a_memreq);
#endif
	return(pcurr);
}

VOID app_tran(WORD bi_num)
{
	LPBIT		lpbi;
	BITBLK		lb;

	rsrc_gaddr(R_BITBLK, bi_num, (LPVOID *)&lpbi);

	LBCOPY(ADDR(&lb), (LPBYTE)lpbi, sizeof(BITBLK));
	gsx_trans(lb.bi_pdata, lb.bi_wb, lb.bi_pdata, lb.bi_wb, lb.bi_hl);
}


WORD app_getfh(WORD openit, BYTE *pname, WORD attr)
{
	WORD		handle, tmpdrv;
	LPBYTE		lp;

	handle = 0;
	strcpy(G.g_srcpth, pname);
	lp = ADDR(&G.g_srcpth[0]);
	tmpdrv = dos_gdrv();
	if (tmpdrv != gl_stdrv)
	{
	  dos_sdrv(gl_stdrv);
	  // dbg("deskapp1.c:447 Error %d in dos_sdrv(%d)\n", DOS_ERR, gl_stdrv);
	}
	if ( shel_find(lp) )
	{
	  if (openit)
	    handle = dos_open(lp, attr);
	  else
	    handle = dos_create(lp, attr);
	  if ( DOS_ERR )
	  {
	    handle = 0;
	  }
	}
	if (tmpdrv != gl_stdrv)
	{
	  dos_sdrv(tmpdrv);
	  // dbg("deskapp1.c:463 Error %d in dos_sdrv(%d)\n", DOS_ERR, gl_stdrv);
	}
	return(handle);
}

