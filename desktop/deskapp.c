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

#include "wccdesk.h"

#define MIN_WINT 4
#define MIN_HINT 2

/* Forward declaration of functions */
MLOCAL BYTE  get_defdrv(ULONG dr_exist, ULONG dr_hard);

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
ANODE *  app_alloc(WORD tohead)
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

VOID  app_free(ANODE *pa)
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

MLOCAL WORD  hex_dig(BYTE achar)
{
	if ( (achar >= '0') && (achar <= '9') ) return(achar - '0');	
	if ( (achar >= 'A') && (achar <= 'F') ) return(achar - 'A' + 10);
	return(0);
}
	

/*
*	Reverse of hex_dig().
*/

MLOCAL BYTE  uhex_dig(WORD wd)
{
	if ( (wd >= 0   ) && (wd <= 9   ) ) return(wd + '0');	
	if ( (wd >= 0x0a) && (wd <= 0x0f) ) return(wd + 'A' - 0x0a);
	return(' ');
}

/*
*	Scan off and convert the next two hex digits and return with
*	pcurr pointing one space past the end of the four hex digits
*/

MLOCAL BYTE *  scan_2(BYTE *pcurr, WORD *pwd)
{
	UWORD		temp;
	
	temp = 0x0;
	temp |= hex_dig(*pcurr++) << 4;
	temp |= hex_dig(*pcurr++);
	if (temp == 0x00ff) temp = NIL;
	*pwd = temp;
	return(	pcurr );
}	

/*
*	Reverse of scan_2().
*/

MLOCAL BYTE *  save_2(BYTE *pcurr, UWORD wd)
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

MLOCAL BYTE *  scan_memsz(BYTE *pcurr, UWORD *pwd)
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

MLOCAL BYTE *  save_memsz(BYTE *pcurr, UWORD wd)
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

BYTE *  scan_str(BYTE *pcurr, BYTE **ppstr)
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

MLOCAL BYTE *  save_str(BYTE *pcurr, BYTE *pstr)
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
VOID  app_detect()
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

MLOCAL BYTE *  app_parse(BYTE *pcurr, ANODE *pa)
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

VOID  app_tran(WORD bi_num)
{
	LPBIT		lpbi;
	BITBLK		lb;

	rsrc_gaddr(R_BITBLK, bi_num, (LPVOID *)&lpbi);

	LBCOPY(ADDR(&lb), (LPBYTE)lpbi, sizeof(BITBLK));
	gsx_trans(lb.bi_pdata, lb.bi_wb, lb.bi_pdata, lb.bi_wb, lb.bi_hl);
}


MLOCAL WORD  app_getfh(WORD openit, BYTE *pname, WORD attr)
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

/* [JCE] All ALCYON bits commented out, as they are no longer 
 *      going to compile or work. */

/************************************************************************/
/* a p p _ r d i c o n							*/
/************************************************************************/
MLOCAL 	WORD 
app_rdicon()
{
	LONG		temp;
	LPBYTE		stmp, dtmp;
	WORD		handle, length, i, iwb, ih;
	WORD		num_icons, num_masks, last_icon, num_wds, 
			num_bytes, msk_bytes, tmp;
/*#if ALCYON
	WORD		stlength, ret;
	BYTE		*fixup, *poffset, **namelist;
#endif*/
#if I8086
	WORD		fixup, poffset;
#endif
						/* open the file	*/
	handle = app_getfh(TRUE, 
		ini_str( (gl_height <= 300) ? STGEMLOI : STGEMHIC), 0);
	if (!handle)
	  return(FALSE);
						/* how much to read?	*/
	length = NUM_IBLKS * sizeof(ICONBLK);
/*#if ALCYON
	ret = dos_read(handle, 2, ADDR(&stlength));
#endif*/
	dos_read(handle, sizeof(WORD), ADDR(&gl_iconstart));
	dos_read(handle, sizeof(WORD), ADDR(&poffset));

	/* gl_iconstart = offset of icon names table relative to
	 * compile address of icon file 
	 *
	 * poffset = compile address of icon file
     *
     * Or, in shorthand:
     * gl_iconstart = filebase + offset(names)
     * poffset      = filebase
     */

	/* Add the offset of the icon bitmaps to poffset */
	poffset += (2 * sizeof(WORD)) + length;

	/* gl_iconstart = filebase + offset(names)
	   poffset      = filebase + offset(bitmaps)
	   */
	gl_iconstart -= poffset;
	/* gl_iconstart = offset(names) - offset(bitmaps)
	 *              = length of the bitmap & string tables. 
	 *                Brilliant! */
	
	/* Read the ICONBLK entries. */
	dos_read(handle, length, ADDR(&G.g_idlist[0]) );
	memcpy(&G.g_iblist[0], &G.g_idlist[0], length);
	
	/* find no. of icons	*/
	/*   actually used	*/
	num_icons = last_icon = 0;
	while ( (last_icon < NUM_IBLKS) &&
		((ULONG)(G.g_idlist[last_icon].ib_pmask) != -1L) )
	{
	  tmp = max( LLOWD(G.g_idlist[last_icon].ib_pmask),
		     LLOWD(G.g_idlist[last_icon].ib_pdata) );
	  num_icons = max(num_icons, tmp);
	  last_icon++;
	}
	num_icons++;
						/* how many words of 	*/
						/*   data to read?	*/
						/* assume all icons are	*/
						/*   same w,h as first	*/
	num_wds = (G.g_idlist[0].ib_wicon * G.g_idlist[0].ib_hicon) / 16;
	num_bytes = num_wds * 2;

						/* allocate some memory	*/
						/*   in bytes     	*/
				/* gl_iconstart = size of icon bit blocks	*/
				/*   and strings on Lattice C		*/
				/* NUM_NAMICS is for string pointers	*/
				/* stlength is strings on ALCYON C	*/
/*#if ALCYON
	length = gl_iconstart + ((NUM_NAMICS + 1) * 4) + stlength;
#else*/
	length = gl_iconstart + (NUM_NAMICS * sizeof(WORD));
/*#endif*/
	/* Load the icon bitblocks & strings */
	G.a_datastart = dos_alloc( LW(length) );
						/* read it		*/
	dos_read(handle, length, G.a_datastart);
	dos_close(handle);
						/* fix up str ptrs	*/
	gl_numics = 0;
/*
#if ALCYON
	namelist = (BYTE *) G.a_datastart + gl_pstart;
	for (i = 0; i < NUM_NAMICS; i++)
	{
	  fixup = *namelist - poffset;
	  *namelist++ = fixup;
#endif
*/

	for (i = 0; i < NUM_NAMICS; ++i)
	{
		LPWORD pv = (LPWORD)(G.a_datastart + gl_iconstart + i * 2);

		fixup = *pv - poffset;
		*pv   = fixup;

		if (G.a_datastart[fixup]) ++gl_numics;
	}
						/* figure out which are	*/
						/*   mask & which data	*/
	for (i=0; i<last_icon; i++)
	{
	  G.g_ismask[ (WORD) G.g_idlist[i].ib_pmask] = TRUE;
	  G.g_ismask[ (WORD) G.g_idlist[i].ib_pdata] = FALSE;
	}
						/* fix up mask ptrs	*/
	num_masks = 0;
	for (i=0; i<num_icons; i++)
	{
	  if (G.g_ismask[i])
	  {
	    G.g_ismask[i] = num_masks;
	    num_masks++;
	  }
	  else
	    G.g_ismask[i] = -1;
	}
						/* allocate memory for	*/
						/*   transformed mask	*/
						/*   forms		*/
	msk_bytes = num_masks * num_bytes;
	G.a_buffstart = dos_alloc( LW(msk_bytes) );
						/* fix up icon pointers	*/
	for (i=0; i<last_icon; i++)
	{
						/* first the mask	*/
	  temp = ( G.g_ismask[ (LONG)(G.g_idlist[i].ib_pmask) ] * 
	  					  ((LONG) num_bytes));
	  G.g_iblist[i].ib_pmask = G.a_buffstart + LW(temp);
	  temp = ( (LONG)(G.g_idlist[i].ib_pmask) * ((LONG) num_bytes));
	  G.g_idlist[i].ib_pmask = G.a_datastart + LW(temp);
						/* now the data		*/
	  temp = ( (LONG)(G.g_idlist[i].ib_pdata) * ((LONG) num_bytes));
	  G.g_iblist[i].ib_pdata = G.g_idlist[i].ib_pdata = 
		G.a_datastart + LW(temp);
						/* now the text ptrs	*/
	  G.g_idlist[i].ib_ytext = G.g_iblist[i].ib_ytext = 
			G.g_idlist[0].ib_hicon;
	  G.g_idlist[i].ib_wtext = G.g_iblist[i].ib_wtext = 12 * gl_wschar;
	  G.g_idlist[i].ib_htext = G.g_iblist[i].ib_htext = gl_hschar + 2;
	}
						/* transform forms	*/
	iwb = G.g_idlist[0].ib_wicon / 8;
	ih = G.g_idlist[0].ib_hicon;

	for (i=0; i<num_icons; i++)
	{
	  if (G.g_ismask[i] != -1)
	  {
						/* preserve standard	*/
						/*   form of masks	*/
	    stmp = G.a_datastart + (i * num_bytes);
	    dtmp = G.a_buffstart + (G.g_ismask[i] * num_bytes);
	    LWCOPY((LPWORD)dtmp, (LPWORD)stmp, num_wds);
	  }
	  else
	  {
						/* transform over std.	*/
						/*   form of datas	*/
	    dtmp = G.a_datastart + (i * num_bytes);
	  }
	  gsx_trans(dtmp, iwb, dtmp, iwb, ih);
	}
/* Move to before splash screen 	app_tran(0); */
/* DESKTOP v1.2 does not frob the icon masks like this
	for (i=0; i<last_icon; i++)
	{
	  if ( i == IG_FOLDER )
	    G.g_iblist[i].ib_pmask = G.g_iblist[IG_TRASH].ib_pmask;
	  if ( ( i == IG_FLOPPY ) ||
	       ( i == IG_HARD ) )
	    G.g_iblist[i].ib_pmask = G.g_iblist[IG_TRASH].ib_pdata;
	  if ( (i >= IA_GENERIC) &&
	       (i < ID_GENERIC) )
	    G.g_iblist[i].ib_pmask = G.g_iblist[IA_GENERIC].ib_pdata;
	  if ( (i >= ID_GENERIC) &&
	       (i < (NUM_ANODES - 1)) )
	    G.g_iblist[i].ib_pmask = G.g_iblist[ID_GENERIC].ib_pdata;
	}*/
	return(TRUE);
} /* app_rdicon */


MLOCAL WORD  app_start1()
{
	WORD fh, i;
						/* remember start drive	*/
	gl_stdrv = dos_gdrv();

	G.g_pbuff = &gl_buffer[0];
	
	for(i=NUM_ANODES - 2; i >= 0; i--)
	  G.g_alist[i].a_next = &G.g_alist[i + 1];
	G.g_ahead = (ANODE *) NULL;
	G.g_aavail = &G.g_alist[0];
	G.g_alist[NUM_ANODES - 1].a_next = (ANODE *) NULL;

	shel_get(ADDR(&gl_afile[0]), SIZE_AFILE);
	if (gl_afile[0] != '#')
	{
						/* invalid signature	*/
						/*   so read from disk	*/
	  fh = app_getfh(TRUE, ini_str(STGEMAPP), 0x0);
	  if (!fh)
	    return(FALSE);
	  G.g_afsize = dos_read(fh, SIZE_AFILE, ADDR(&gl_afile[0]));
	  dos_close(fh);
	  gl_afile[G.g_afsize] = 0;
	}
	return TRUE;
}

MLOCAL WORD  app_start2()
{
	WORD xcnt, ycnt, xcent, x, y;
	ANODE *pa;

	G.g_wicon = (12 * gl_wschar) + (2 * G.g_idlist[0].ib_xtext);
	G.g_hicon = G.g_idlist[0].ib_hicon + gl_hschar + 2;

	G.g_icw = (gl_height <= 300) ? 0 : 8;
	G.g_icw += G.g_wicon;
	xcnt = (gl_width/G.g_icw);
	G.g_icw += (gl_width % G.g_icw) / xcnt;
	G.g_ich = G.g_hicon + MIN_HINT;
	ycnt = ((gl_height-gl_hbox) / G.g_ich);
	G.g_ich += ((gl_height-gl_hbox) % G.g_ich) / ycnt;

	/* DESKTOP v1.2: Scale drive coordinates */
	for (pa = G.g_ahead; pa; pa = pa->a_next)
	{
		x = pa->a_xspot * G.g_icw;
		y = pa->a_yspot * G.g_ich + G.g_ydesk;
		snap_disk(x, y, &pa->a_xspot, &pa->a_yspot);		
	}
	
	xcent = (G.g_wicon - G.g_idlist[0].ib_wicon) / 2;
	G.g_nmicon = 9;
	G.g_xyicon[0] = xcent;  G.g_xyicon[1] = 0;
	G.g_xyicon[2]=xcent; G.g_xyicon[3]=G.g_hicon-gl_hschar-2;
	G.g_xyicon[4] = 0;  G.g_xyicon[5] = G.g_hicon-gl_hschar-2;
	G.g_xyicon[6] = 0;  G.g_xyicon[7] = G.g_hicon;
	G.g_xyicon[8] = G.g_wicon;  G.g_xyicon[9] = G.g_hicon;
	G.g_xyicon[10]=G.g_wicon; G.g_xyicon[11] = G.g_hicon-gl_hschar-2;
	G.g_xyicon[12]=G.g_wicon - xcent; G.g_xyicon[13]=G.g_hicon-gl_hschar-2;
	G.g_xyicon[14] = G.g_wicon - xcent;  G.g_xyicon[15] = 0;
	G.g_xyicon[16] = xcent;  G.g_xyicon[17] = 0;
	G.g_nmtext = 5;
	G.g_xytext[0] = 0;  		G.g_xytext[1] = 0;
	G.g_xytext[2] = gl_wchar * 12; 	G.g_xytext[3] = 0;
	G.g_xytext[4] = gl_wchar * 12;   G.g_xytext[5] = gl_hchar;
	G.g_xytext[6] = 0;  		G.g_xytext[7] = gl_hchar;
	G.g_xytext[8] = 0; 		G.g_xytext[9] = 0;
	return TRUE;
}


/*
*	Initialize the application list by reading in the DESKTOP.INF
*	file, either from memory or from the disk if the shel_get
*	indicates no message is there.
*/
WORD  app_start()
{
	WORD		i;
	ANODE		*pa;
	WSAVE		*pws;
	BYTE		*pcurr, *ptmp, prevdisk;
	WORD		envr, wincnt;
#if MULTIAPP
	WORD		numaccs;
	BYTE		*savbuff;
	
	numaccs = 0;
#endif		
	if (!app_start1()) return FALSE;	

	wincnt = 0;
	pcurr = &gl_afile[0];
	prevdisk = ' ';
	while (*pcurr)
	{
	  if (*pcurr != '#')
	    pcurr++;
	  else
	  {
	    pcurr++;
	    switch(*pcurr)
	    {
		  case 'T':				/* DESKTOP v1.2: Trash */
		  	pa = app_alloc(TRUE);
		  	pcurr = app_parse(pcurr, pa);

		    for (i = 0; i < 6; i++)
	        {
		      	pa = app_alloc(TRUE);
      		    app_parse(ini_str(ST1STD+i)+1, pa);
		    } /* for */
		  	break;
	      case 'M':				/* Media (Hard/Floppy)	*/
	      case 'G':				/* GEM Application	*/
	      case 'F':				/* File	(DOS w/o parms)	*/
	      case 'f':				/*   use full memory	*/
	      case 'P':				/* Parm	(DOS w/ parms)	*/
	      case 'p':				/*   use full memory	*/
	      case 'D':				/* Directory		*/
			if ( *pcurr == 'M') prevdisk = 'M';
			/*else	// DESKTOP v1.2: This is done by the 'Trash' line
			{ 
			
			  if (prevdisk == 'M') 
			  {
			    for (i = 0; i < 6; i++)
		        {
			      	pa = app_alloc(TRUE);
	      		    app_parse(ini_str(ST1STD+i)+1, pa);
			    }
			  } 
			  prevdisk = ' ';
			}*/
			pa = app_alloc(TRUE);
	    	pcurr = app_parse(pcurr, pa);
			break;
#if MULTIAPP			
	      case 'A':				/* Desk Accessory	*/
		    pcurr++;
			pcurr = scan_2(pcurr, &(gl_caccs[numaccs].acc_swap));
			savbuff = G.g_pbuff;
			G.g_pbuff = &(gl_caccs[numaccs].acc_name[0]);
			pcurr = scan_str(pcurr, &ptmp);
			G.g_pbuff = savbuff;
			numaccs++;
			break;
#endif
	      case 'W':				/* Window		*/
			pcurr++;
			if ( wincnt < NUM_WNODES )
			{
			  pws = &G.g_cnxsave.win_save[wincnt];
			  pcurr = scan_2(pcurr, &pws->hsl_save);
			  pcurr = scan_2(pcurr, &pws->vsl_save);
/* DESKTOP v1.2 puts these in pws->*_save, not gl_savewin */
			  pcurr = scan_2(pcurr, &pws->x_save);
			  pws->x_save *= gl_wchar;
			  pcurr = scan_2(pcurr, &pws->y_save);
			  pws->y_save *= gl_hchar;
			  pcurr = scan_2(pcurr, &pws->w_save);
			  pws->w_save *= gl_wchar;
			  pcurr = scan_2(pcurr, &pws->h_save);
			  pws->h_save *= gl_hchar;
/* */
			  pcurr = scan_2(pcurr, &pws->obid_save);
			  ptmp = &pws->pth_save[0];
			  pcurr++;
			  while ( *pcurr != '@' )
			    *ptmp++ = *pcurr++;
			  *ptmp = 0;
			  wincnt++;
/* DESKTOP v1.2 does these a bit before.
			  gl_savewin[wincnt].g_x = x * gl_wchar;
			  gl_savewin[wincnt].g_y = y * gl_hchar;
			  gl_savewin[wincnt].g_w = w * gl_wchar;
			  gl_savewin[wincnt++].g_h = h * gl_hchar;
*/
			}
			break;
	      case 'E':
			pcurr++;
			pcurr = scan_2(pcurr, &envr);
			G.g_cnxsave.vitem_save = ( (envr & 0x80) != 0);
			G.g_cnxsave.sitem_save = ( (envr & 0x60) >> 5);
			G.g_cnxsave.cdele_save = ( (envr & 0x10) != 0);
			G.g_cnxsave.ccopy_save = ( (envr & 0x08) != 0);
			G.g_cnxsave.cdclk_save = envr & 0x07;
			pcurr = scan_2(pcurr, &envr);
/* Bring these 4 back which weren't in 1.2 */
			G.g_cnxsave.covwr_save = ( (envr & 0x10) == 0);
		 	G.g_cnxsave.cmclk_save = ( (envr & 0x08) != 0);
			G.g_cnxsave.cdtfm_save = ( (envr & 0x04) == 0);
			G.g_cnxsave.ctmfm_save = ( (envr & 0x02) == 0);
			desk_sound(FALSE, !(envr & 0x01), 0);
/* From BALJ's Desktop */
			pcurr = scan_2(pcurr,&envr); 
			G.g_cnxsave.cdetn_save = ( (envr & 0x08) == 0);
			G.g_cnxsave.cdetd_save = ( (envr & 0x04) == 0);
			G.g_detdrives = G.g_cnxsave.cdetd_save;
			G.g_probedrives = G.g_cnxsave.cdetn_save;
			break;
	    }
	  }
	}
	if (!app_rdicon())
	  return(FALSE);	

	return app_start2();
}


/*
*	Reverse list when we write so that we can read it in naturally
*/
MLOCAL VOID  app_revit()
{
	ANODE		*pa;
	ANODE		*pnxtpa;
						/* reverse list		*/
	pa = G.g_ahead;
	G.g_ahead = (ANODE *) NULL;
	while(pa)
	{
	  pnxtpa = pa->a_next;
	  pa->a_next = G.g_ahead;
	  G.g_ahead = pa;
	  pa = pnxtpa;
	}
}


/*
*	Save the current state of all the icons to a file called 
*	DESKTOP.INF
*/
WORD  app_save(WORD todisk)
{
	WORD		i, fh, ret;
	UWORD		envr;
	BYTE		*pcurr, *ptmp;
	ANODE		*pa;
	WSAVE		*pws;

	memset(gl_afile, 0, SIZE_AFILE);
	pcurr = &gl_afile[0];
						/* save evironment	*/
	*pcurr++ = '#';
	*pcurr++ = 'E';
	envr = 0x0;
	envr |= (G.g_cnxsave.vitem_save) ? 0x80 : 0x00;
	envr |= ((G.g_cnxsave.sitem_save) << 5) & 0x60;
	envr |= (G.g_cnxsave.cdele_save) ? 0x10 : 0x00;
	envr |= (G.g_cnxsave.ccopy_save) ? 0x08 : 0x00;
	envr |= G.g_cnxsave.cdclk_save;
	pcurr = save_2(pcurr, envr);
/* Not in DESKTOP v1.2; bring them back */
	envr = (G.g_cnxsave.covwr_save)  ? 0x00 : 0x10;
	envr |= (G.g_cnxsave.cmclk_save) ? 0x08 : 0x00;
	envr |= (G.g_cnxsave.cdtfm_save) ? 0x00 : 0x04;
	envr |= (G.g_cnxsave.ctmfm_save) ? 0x00 : 0x02;
	envr |= desk_sound(FALSE, -1, 0) ? 0x00 : 0x01;
	pcurr = save_2(pcurr, envr );
	envr  = 0;
	envr |= (G.g_cnxsave.cdetd_save) ? 0x00 : 0x04;
	envr |= (G.g_cnxsave.cdetn_save) ? 0x00 : 0x08;
	pcurr = save_2(pcurr, envr );
	*pcurr++ = 0x0d;
	*pcurr++ = 0x0a;
						/* save windows		*/
	for(i=0; i<NUM_WNODES; i++)
	{
	  *pcurr++ = '#';
	  *pcurr++ = 'W';
	  pws = &G.g_cnxsave.win_save[i];
	  pcurr = save_2(pcurr, pws->hsl_save);
	  pcurr = save_2(pcurr, pws->vsl_save);
	  pcurr = save_2(pcurr, pws->x_save / gl_wchar);
	  pcurr = save_2(pcurr, pws->y_save / gl_hchar);
	  pcurr = save_2(pcurr, pws->w_save / gl_wchar);
	  pcurr = save_2(pcurr, pws->h_save / gl_hchar);
	  pcurr = save_2(pcurr, pws->obid_save);
	  ptmp = &pws->pth_save[0];
	  *pcurr++ = ' ';
	  if (*ptmp != '@')
	  {
	    while (*ptmp)
	      *pcurr++ = *ptmp++;
	  }
	  *pcurr++ = '@';
	  *pcurr++ = 0x0d;
	  *pcurr++ = 0x0a;
	}		
#if MULTIAPP
	for (i=0; i<3; i++)
	  if (gl_caccs[i].acc_name[0])
	  {
	    *pcurr++ = '#';
	    *pcurr++ = 'A';
	    pcurr = save_2(pcurr, gl_caccs[i].acc_swap);
	    *pcurr++ = ' ';
	    pcurr = save_str(pcurr, &(gl_caccs[i].acc_name[0]));
	    pcurr--;
	    *pcurr++ = 0x0d;
	    *pcurr++ = 0x0a;
	  }
#endif	
						/* reverse ANODE list	*/
	app_revit();
						/* save ANODE list	*/
	for(pa=G.g_ahead; pa; pa=pa->a_next)
	{
	  /* From BALJ's desktop: Don't save autodetected drives 
	   * (why not, though?)
          if ( (pa->a_flags >= AF_WASDET) && todisk )
		              continue; */
	  *pcurr++ = '#';
	  switch(pa->a_type)
	  {
	    case AT_ISDISK:
		*pcurr++ = 'M';
		break;
	    case AT_ISFILE:
		if ( (pa->a_flags & AF_ISCRYS) &&
		     (pa->a_flags & AF_ISGRAF) )
		  *pcurr++ = 'G';
		else
		{  
		  *pcurr = (pa->a_flags & AF_ISPARM) ? 'P' : 'F';
		  if (pa->a_flags & AF_ISFMEM)
		    *pcurr += 'a' - 'A';
		  pcurr++;
		}
		break;
	    case AT_ISFOLD:
		*pcurr++ = 'D';
		break;
		
		case AT_ISTRSH:	// DESKTOP v1.2
		*pcurr++ = 'T';
		break;
	  }
	  
	  if (pa->a_flags & AF_ISDESK)
	  {
	    pcurr = save_2(pcurr, pa->a_xspot / G.g_icw);
	    pcurr = save_2(pcurr, (pa->a_yspot - G.g_ydesk) / G.g_ich);
	  }
	  pcurr = save_2(pcurr, pa->a_aicon);
	  pcurr = save_2(pcurr, pa->a_dicon);
	  *pcurr++ = ' ';
	  if (pa->a_flags & AF_ISDESK)
	  {
	    *pcurr++ = pa->a_letter ? pa->a_letter : ' ';
	    *pcurr++ = ' ';
	  }
	  pcurr = save_str(pcurr, pa->a_pappl);
	  pcurr = save_str(pcurr, pa->a_pdata);
	  pcurr--;
#if MULTIAPP
	  if (!(pa->a_flags & AF_ISDESK))	/* only for apps	*/
	  {
	    pcurr++;				/* leave blank		*/
	    pcurr = save_memsz(pcurr, pa->a_memreq);
	  }
#endif
	  *pcurr++ = 0x0d;
	  *pcurr++ = 0x0a;
						/* skip standards	*/
//	  if ( (pa->a_type == AT_ISDISK) && 
//	       (pa->a_next->a_type != AT_ISDISK) )
	  if ( (pa->a_type == AT_ISTRSH))	/* DESKTOP v1.2 */
	  {
	    for(i=0; i<6; i++)
	      pa = pa->a_next;
	  }
	}
	*pcurr++ = 0x1a;
	*pcurr++ = 0x0;
						/* reverse list back	*/
	app_revit();
						/* calculate size 	*/
	G.g_afsize = pcurr - &gl_afile[0];
						/* save in memory	*/
	shel_put(ADDR(&gl_afile[0]), G.g_afsize);
						/* save to disk		*/
	if (todisk)
	{
	  G.g_afsize--;
	  fh = 0;
	  while (!fh)
	  {
	    fh = app_getfh(FALSE, ini_str(STGEMAPP), 0x0);
	    if (!fh)
	    {
	      ret = fun_alert(1, STNOINF);
	      if (ret == 2)
	        return(FALSE);
	    }
	  }
	  G.g_afsize = dos_write(fh, G.g_afsize, ADDR(&gl_afile[0]));
	  dos_close(fh);
	}
	return TRUE;
}


/*
*	Build the desktop list of objects based on this current 
*	application list.
*/

BYTE  app_blddesk()
{
	WORD		obid;
	ULONG		bvdisk, bvhard, bvect;
	ANODE		*pa;
	OBJECT		*pob;
	ICONBLK		*pic;
#if ALCYON
	LONG		*ptr;
#endif						/* free all this windows*/
						/*   kids and set size	*/
	obj_wfree(DROOT, 0, 0, gl_width, gl_height);
#if ALCYON
	ptr = &global[3];
	G.g_screen[DROOT].ob_spec = LLGET(ptr);
#else
	G.g_screen[DROOT].ob_spec = (LPVOID)(LW(global[3]) + HW(global[4]));
#endif
	// FreeGEM only...
	G.g_screen[DROOT].ob_flags = USECOLORCAT;
	
	bvdisk = bvhard = 0x0;

	for(pa = G.g_ahead; pa; pa = pa->a_next)
	{
	  if (pa->a_flags & AF_ISDESK)
	  {
	    obid = obj_ialloc(DROOT, pa->a_xspot, pa->a_yspot,
					G.g_wicon, G.g_hicon);
	    if (!obid)
	    {
	    /* error case, no more obs */
	    }
						/* set up disk vector	*/
	    if (pa->a_type == AT_ISDISK)
	    {
		  bvect = 1L << 31 + 'A' - pa->a_letter;
		  //dbg("%c -> %lx\n", pa->a_letter, bvect);
	      bvdisk |= bvect;
	      if (pa->a_aicon == IG_HARD) bvhard |= bvect;
	    }
						/* remember it		*/
	    pa->a_obid = obid;
						/* build object		*/
	    pob = &G.g_screen[obid];
	    pob->ob_state = NORMAL;
	    pob->ob_flags = NONE;
	    pob->ob_type = G_ICON;
	    G.g_index[obid] = pa->a_aicon;
	    pob->ob_spec = ADDR( pic = &gl_icons[obid] );
	    memcpy(pic, &G.g_iblist[pa->a_aicon], sizeof(ICONBLK));
	    pic->ib_xicon = ((G.g_wicon - pic->ib_wicon) / 2);
	    pic->ib_ptext = ADDR(pa->a_pappl);
	    pic->ib_char |= (0x00ff & pa->a_letter);
	  } /* if */
	} /* for */

	if ((gl_xbuf.abilities & ABLE_XBVSET) || global[0] >= 0x400)
	{
		appl_xbvset(bvdisk, bvhard);
	}
	else appl_bvset(bvdisk >> 16, bvhard >> 16);
	
	return( get_defdrv(bvdisk, bvhard) );
} /* app_blddesk */


/*
*	Find the ANODE that is appropriate for this object.
*/
ANODE *  app_afind(WORD isdesk, WORD atype, WORD obid, BYTE *pname, WORD *pisapp)
{
	ANODE		*pa;

	for(pa = G.g_ahead; pa; pa = pa->a_next)
	{
	  if (isdesk)
	  {
	    if (pa->a_obid == obid)
	      return(pa);
	  }
	  else
	  {
	    if ( (pa->a_type == atype) && !(pa->a_flags & AF_ISDESK) )
	    {
	      if ( wildcmp(pa->a_pdata, pname) )
	      {
		*pisapp = FALSE;
		return(pa);
	      }
	      if ( wildcmp(pa->a_pappl, pname) )
	      {
		*pisapp = TRUE;
		return(pa);
	      } /* if */
	    } /* if */
	  } /* else */
	} /* for */
	return(0);
} /* app_afind */

/************************************************************************/
/* g e t _ d e f d r v							*/
/************************************************************************/
MLOCAL BYTE  get_defdrv(ULONG dr_exist, ULONG dr_hard)
{
/* this routine returns the drive letter of the lowest drive: lowest	*/
/* lettered hard disk if possible, otherwise lowest lettered floppy	*/
/* (which is usually A)							*/
/* in dr_exist, MSbit = A						*/
	ULONG		mask, hd_disk;
	WORD		ii;
	BYTE		drvletr;

	mask = 0x80000000L;
	hd_disk = dr_exist & dr_hard;
	if (hd_disk)
	{		/* there's a hard disk out there somewhere	*/
	  for (ii = 0; ii <= 31; ii++)
	  {
	    if (mask & hd_disk)
	    {
	      drvletr = ii + 'A';
	      break;
	    } /* if */
	    mask >>= 1;
	  } /* for */
	} /* if hd_disk */
	else
	  drvletr = 'A';			/* assume A is always	*/
	  					/* lowest floppy	*/
	return(drvletr);
} /* get_defdrv */
