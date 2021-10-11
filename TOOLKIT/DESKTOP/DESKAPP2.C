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

/* [JCE] All ALCYON bits commented out, as they are no longer 
 *      going to compile or work. */

/************************************************************************/
/* a p p _ r d i c o n							*/
/************************************************************************/
	WORD
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
	  tmp = wmax( LLOWD(G.g_idlist[last_icon].ib_pmask),
		     LLOWD(G.g_idlist[last_icon].ib_pdata) );
	  num_icons = wmax(num_icons, tmp);
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


MLOCAL WORD app_start1()
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

MLOCAL WORD app_start2()
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
WORD app_start()
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

