/*	DESKINF.C	09/03/84 - 05/29/85	Gregg Morris		*/
/*	for 3.0	& 2.1	5/5/86			MDF			*/
/*	merge source	5/27/87  - 5/28/87	mdf			*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985 - 1987		Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "ppddesk.h"

typedef struct sfcb
{
	BYTE		sfcb_junk;
	unsigned char		sfcb_attr;
	WORD		sfcb_time;
	WORD		sfcb_date;
	LONG		sfcb_size;
	BYTE		sfcb_name[13];
} SFCB;

typedef SFCB far *LPSFCB;

/************************************************************************/
/* m y _ i t o a							*/
/************************************************************************/
VOID my_itoa(UWORD number, BYTE *pnumstr)
{
	WORD		ii;

	for (ii = 0; ii < 2; pnumstr[ii++] = '0');
	pnumstr[2] = 0;
	if (number > 9) merge_str(pnumstr,   "%W", number);
	else	        merge_str(pnumstr+1, "%W", number);
} /* my_itoa */

/*
*	Routine to format DOS style time.
*
*	15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
*	<     hh     > <    mm    > <   xx  >
*	hh = binary 0-23
*	mm = binary 0-59
*	xx = binary seconds \ 2 
*
*	put into this form 12:45 pm
*                   or 12:45 in 24-hr mode
*/
VOID fmt_time(UWORD time, BYTE *ptime)
{
	WORD		pm, val;

	val = ((time & 0xf800) >> 11) & 0x001f;
	if (G.g_ctimeform)
	{
		if (val >= 12)
		{
		    if (val > 12)
		      val -= 12;
		    pm = TRUE;
		}
		else
		{
		    if (val == 0)
		      val = 12;
	   	 pm = FALSE;
		}
	}
	my_itoa( val, &ptime[0]);
	my_itoa( ((time & 0x07e0) >> 5) & 0x003f, &ptime[2]);
	// DESKTOP v1.2 doesn't localise these strings, & doesn't do
	// 24-hour
//	strlcpy(ptime + 4, (pm? "pm" : "am"), 3);

	if (G.g_ctimeform) strlcpy(ptime + 4, (pm? gl_pmstr : gl_amstr), 3);
	else               strlcpy(ptime + 4, "  ", 3);

} /* fmt_time */

/*
*	Routine to format DOS style date.
*	
*	15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
*	<     yy          > < mm  > <  dd   >
*	yy = 0 - 119 (1980 - 2099)
*	mm = 1 - 12
*	dd = 1 - 31
*/
VOID fmt_date(UWORD date, BYTE *pdate)
{
	WORD year;

/* Not supported in DESKTOP v1.2; but bring it back for 3.2 */
	if (G.g_cdateform)
	{
	  my_itoa( (date & 0x01e0) >> 5, &pdate[0]);
	  my_itoa(date & 0x001f, &pdate[2]);
	}
	else
	{
	  my_itoa(date & 0x001f, &pdate[0]);
	  my_itoa( (date & 0x01e0) >> 5, &pdate[2]);
	}
	/* [JCE 25-11-2001] year 2000 bugfix. Without this, the
         * my_itoa() call overruns the buffer in ob_sfcb() by 
	 * putting 3 bytes where there was only room for 2. The
	 * last byte hits the saved BP value, with hilarious 
	 * consequences. */

	year = 1980 + (((date & 0xfe00) >> 9) & 0x007f);
	/* v3.2: Display full 4-figure year numbers */
	my_itoa(year / 100, &pdate[4]);	
	my_itoa(year % 100, &pdate[6]);	
	//my_itoa(year % 100, &pdate[4]);
} /* fmt_date */

WORD ob_sfcb(LPSFCB psfcb, BYTE *pfmt)
{
	SFCB		sf;
	BYTE		*pdst, *psrc;
	BYTE		pdate_str[11], ptime_str[11], psize_str[20];
	WORD		cnt;

	LBCOPY(ADDR(&sf.sfcb_junk), (LPBYTE)psfcb, sizeof(SFCB));
	pdst = pfmt;
	psrc = &sf.sfcb_name[0];
	*pdst++ = ' ';
	*pdst++ = (sf.sfcb_attr & F_SUBDIR) ? 0x07 : ' ';
	*pdst++ = ' ';
	if (sf.sfcb_attr & F_DESKTOP)
	{
	  *pdst++ = sf.sfcb_junk;
	  *pdst++ = ':';
	  *pdst++ = ' ';
	}
	else
	{
	  while( (*psrc) &&
	         (*psrc != '.') )
	    *pdst++ = *psrc++;
	  while( (pdst - pfmt) < 12 )
	    *pdst++ = ' ';
	  if (*psrc)
	    psrc++;
	  while (*psrc)
	    *pdst++ = *psrc++;
	}
	while( (pdst - pfmt) < 16 )
	  *pdst++ = ' ';
	psrc = &psize_str[0];
	if (sf.sfcb_attr & F_SUBDIR)
	  *psrc = 0;
	else
	{
	  // [JCE] wow, big buffer overflow...
	  LONG kay = sf.sfcb_size;
	  static char *fix[3] = { "K", "M", "G" };
	  char *s = fix[0];  
	  merge_str(&psize_str[0], "%L", sf.sfcb_size);
  	  while (strlen(psrc) > 8)
	  {
		kay = (kay + 1023) / 1024;
		merge_str(&psize_str[0], "%L", kay);
		strcat(psrc, s);
		++s;
	  }
	}
	for(cnt = 8 - strlen(psrc); cnt--; *pdst++ = ' ');
	while (*psrc)
	  *pdst++ = *psrc++;
	*pdst++ = ' ';
	*pdst++ = ' ';
	fmt_date(sf.sfcb_date, &pdate_str[0]);
	psrc = &pdate_str[0];
	for(cnt = 3; cnt--; )
	{
	  *pdst++ = *psrc++;
	  *pdst++ = *psrc++;
	  if (cnt)
	    *pdst++ = '/';
	}
	*pdst++ = *psrc++;
	*pdst++ = *psrc++;	/* Last 2 digits of year */
	*pdst++ = ' ';
	fmt_time(sf.sfcb_time, &ptime_str[0]);
	psrc = &ptime_str[0];
	for(cnt = 2; cnt--; )
	{
	  *pdst++ = *psrc++;
	  *pdst++ = *psrc++;
	  if (cnt)
	    *pdst++ = ':';
	}
	*pdst++ = ' ';
	strlcpy(pdst, ptime_str + 4, 3);
	pdst += 2;
	return(pdst - pfmt);
}	

WORD dr_fnode(UWORD last_state, UWORD curr_state, 
			WORD x, WORD y, WORD w, WORD h, LPSFCB psfcb)
{
	WORD		len;
	char		path[82];
	
	if ((last_state ^ curr_state) & SELECTED)
	  bb_fill(MD_XOR, FIS_SOLID, IP_SOLID, x, y, w, h);
	else
	{
 	  len = ob_sfcb(psfcb, &G.g_tmppth[0]);
	  gsx_attr(TRUE, MD_REPLACE, BLACK);
	  lstlcpy(ADDR(path), ADDR(&G.g_tmppth[0]), sizeof(path));
	  gsx_tblt(IBM, x, y, len, path);
	  gsx_attr(FALSE, MD_XOR, BLACK);
	}
	return(curr_state);
}

WORD dr_code(LPPARM pparms)
{
	PARMBLK		pb;
	GRECT		oc;
	WORD		state;

	LBCOPY(ADDR(&pb), (LPBYTE)pparms, sizeof(PARMBLK));

	gsx_gclip(&oc);
	gsx_sclip((GRECT *)&pb.pb_xc);
	state = dr_fnode(pb.pb_prevstate, pb.pb_currstate,
			 pb.pb_x, pb.pb_y, pb.pb_w, pb.pb_h, pb.pb_parm);
	gsx_sclip(&oc);
	return(state);
}

/*
*	Put up dialog box & call form_do.
*/
WORD inf_show(LPTREE tree, WORD start)
{
	WORD		xd, yd, wd, hd;

	form_center(tree, &xd, &yd, &wd, &hd);
	form_dial(FMD_START, 0, 0, 0, 0, xd, yd, wd, hd);
/* ViewMAX here includes code to hide the help button on helpscreens */
	objc_draw(tree, ROOT, MAX_DEPTH, xd, yd, wd, hd);
	form_do(tree, start);
	form_dial(FMD_FINISH, 0, 0, 0, 0, xd, yd, wd, hd);
/* ViewMAX here waits for the next two messages. Not sure why. */
	return(TRUE);
}

/*
*	Routine for finishing off a simple ok-only dialog box
*/
VOID inf_finish(LPTREE tree, WORD dl_ok)
{
	inf_show(tree, 0);
	tree[dl_ok].ob_state &= ~SELECTED;
}

/*
*	Routine to get number of files and folders and stuff them in
*	a dialog box.
*/
WORD inf_fifo(LPTREE tree, WORD dl_fi, WORD dl_fo, BYTE *ppath, WORD pathlen)
{
	WORD		junk, more;
	BYTE		nf_str[20], nd_str[20];

	G.g_nfiles = 0x0L;
	G.g_ndirs = 0x0L;
	G.g_size = 0x0L;

	more = d_doop(OP_COUNT, NULL, 0, ppath, ppath, pathlen, pathlen,
		       	&junk, &junk, 0);
	if (!more)
	  return(FALSE);
	G.g_ndirs--;

	merge_str(&nf_str[0], "%L", G.g_nfiles);
	inf_sset(tree, dl_fi, &nf_str[0]);

	merge_str(&nd_str[0], "%L", G.g_ndirs);
	inf_sset(tree, dl_fo, &nd_str[0]);
	return(TRUE);
}

VOID inf_dttmsz(LPTREE tree, FNODE *pf, WORD dl_dt, WORD dl_tm, WORD dl_sz, 
			    LONG *psize)
{
	/* [JCE 26-11-2001] Appease the dread spirits of buffer 
	 *                 overflow... */
	BYTE	psize_str[20], ptime_str[15], pdate_str[15];
	BYTE	ftime_str[20];	/* [JCE 6-5-2002] Fix the formatting of  */
	BYTE	*pt, *ft;	/* the time. I think I must have         */
				/* disassembled a buggy Desktop, because */
				/* I think the original misformatted the */
				/* time as well... */

	fmt_date(pf->f_date, &pdate_str[0]);
	inf_sset(tree, dl_dt, &pdate_str[0]);

	fmt_time(pf->f_time, &ptime_str[0]);
	ft = ftime_str;
	pt = ptime_str;
	*ft++ = *pt++;
	*ft++ = *pt++;
	*ft++ = ':';
	*ft++ = *pt++;
	*ft++ = *pt++;
	*ft++ = ' ';
	/* 6 bytes have been written to ft, so there are 14 left. */
	strlcpy(ft, pt, 14);
	inf_sset(tree, dl_tm, &ftime_str[0]);

	merge_str(&psize_str[0], "%L", *psize);
	inf_sset(tree, dl_sz, &psize_str[0]);
}

/************************************************************************/
/* i n f _ f i l e							*/
/************************************************************************/
WORD inf_file(BYTE *ppath, FNODE *pfnode)
{
	LPTREE		tree;
	WORD		attr, more, nmidx;
	BYTE		poname[13], pnname[13];

	tree = G.a_trees[ADFILEIN];

	strlcpy(G.g_srcpth, ppath, 82);
	strlcpy(G.g_dstpth, ppath, 82);
	nmidx = 0;
	while (G.g_srcpth[nmidx] != '*')
	  nmidx++;

	fmt_str(&pfnode->f_name[0], &poname[0]);

	inf_sset(tree, FINAME, &poname[0]);

	inf_dttmsz(tree, pfnode, FIDATE, FITIME, FISIZE, &pfnode->f_size);

	inf_fldset(tree, FIRO,  pfnode->f_attr, F_RDONLY,  SELECTED, NORMAL);
//	inf_fldset(tree, FIHID, pfnode->f_attr, F_HIDDEN,  SELECTED, NORMAL);
//	inf_fldset(tree, FISYS, pfnode->f_attr, F_SYSTEM,  SELECTED, NORMAL);
	inf_fldset(tree, FIARC, pfnode->f_attr, F_ARCHIVE, SELECTED, NORMAL);

	inf_show(tree, 0);
					/* now find out what happened	*/
						/* was it OK or CANCEL?	*/
	if ( inf_what(tree, FIOK, FICNCL) )
	{
	  graf_mouse(HGLASS, 0x0L);

	  more = TRUE;
	  inf_sget(tree, FINAME, &pnname[0], sizeof(pnname));
	  				/* unformat the strings		*/
	  unfmt_str(&poname[0], &G.g_srcpth[nmidx]);
	  unfmt_str(&pnname[0], &G.g_dstpth[nmidx]);
						/* do the DOS rename	*/
	  if ( strcmp(&G.g_srcpth[nmidx], &G.g_dstpth[nmidx]) )
	  {
	    dos_rename(ADDR(&G.g_srcpth[0]), ADDR(&G.g_dstpth[0]));
	    if ( (more = d_errmsg()) != 0 )
	      strlcpy(pfnode->f_name, G.g_dstpth + nmidx, LEN_ZFNAME);
	  } /* if */
					/* update the attributes	*/
	  attr = pfnode->f_attr;
	  if (tree[FIRO].ob_state & SELECTED)    attr |=  F_RDONLY;
	  else	                                 attr &= ~F_RDONLY;
/* These controls are disabled, because setting a file to hidden or 
 * system would instantly hide it, with no way to being it back 
	  if (tree[FIHID].ob_state & SELECTED)  attr |=  F_HIDDEN;
	  else	                                attr &= ~F_HIDDEN;
	  if (tree[FISYS].ob_state & SELECTED)  attr |=  F_SYSTEM;
	  else	                                attr &= ~F_SYSTEM;
*/
	  if (tree[FIARC].ob_state & SELECTED)  attr |=  F_ARCHIVE;
	  else	                                attr &= ~F_ARCHIVE;
	  if ( (BYTE) attr != pfnode->f_attr )
	  {
	    dos_chmod(ADDR(&G.g_dstpth[0]), F_SETMOD, attr);
	    if ( (more = d_errmsg()) != 0 )
	      pfnode->f_attr = attr;
	  }
	  graf_mouse(ARROW, 0x0L);
	  return(more);
	}
	else
	  return(FALSE);
} /* inf_file */

/************************************************************************/
/* i n f _ f o l d e r							*/
/************************************************************************/
WORD inf_folder(BYTE *ppath, FNODE *pf)
{
	LPTREE		tree;
	WORD		more;
	BYTE		*pname, fname[13];

	graf_mouse(HGLASS, 0x0L);	

	tree = G.a_trees[ADFOLDIN];

	strlcpy(G.g_srcpth, ppath, sizeof(G.g_srcpth));
	pname = strchr(G.g_srcpth, '*');
	if (pname) strlcpy(pname, pf->f_name, LEN_ZFNAME);
	strlcat(G.g_srcpth, "\\*.*", sizeof(G.g_srcpth));

	more = inf_fifo(tree, FOLNFILE, FOLNFOLD, &G.g_srcpth[0], sizeof(G.g_srcpth));

	graf_mouse(ARROW, 0x0L);
	if (more)
	{
	  fmt_str(&pf->f_name[0], &fname[0]);
	  inf_sset(tree, FOLNAME, &fname[0]);

	  inf_dttmsz(tree, pf, FOLDATE, FOLTIME, FOLSIZE, &G.g_size);
	  inf_finish(tree, FOLOK);
	}
	return(TRUE);
} /* inf_folder */

/************************************************************************/
/* i n f _ d i s k							*/
/************************************************************************/
WORD inf_disk(BYTE dr_id)
{
	LPTREE		tree;
	LONG		total, avail;
	WORD		more;
	BYTE		puse_str[12], pav_str[12], plab_str[12];
	BYTE		drive[2];

	graf_mouse(HGLASS, 0x0L);
	tree = G.a_trees[ADDISKIN];

	drive[0] = dr_id;
	drive[1] = 0;
	G.g_srcpth[0] = drive[0];
	strlcpy(G.g_srcpth + 1, ":\\*.*", 127);

	more = inf_fifo(tree, DINFILES, DINFOLDS, &G.g_srcpth[0], sizeof(G.g_srcpth));

	graf_mouse(ARROW, 0x0L);
	if (more)
	{
	  dos_space(dr_id - 'A' + 1, &total, &avail);
	  dos_label(dr_id - 'A' + 1, &plab_str[0]);

	  inf_sset(tree, DIDRIVE, &drive[0]);
	  inf_sset(tree, DIVOLUME, &plab_str[0]);

	  merge_str(&puse_str[0], "%L", G.g_size);
	  inf_sset(tree, DIUSED, &puse_str[0]);
	  
	  merge_str(&pav_str[0], "%L", avail);
	  inf_sset(tree, DIAVAIL, &pav_str[0]);

	  inf_finish(tree, DIOK);
	}
	return(TRUE);
} /* inf_disk */

/*
*	Set preferences dialog.
*/
	WORD
inf_pref()
{
	LPTREE		tree;
	WORD		cyes, cno, i;
	WORD		sndefpref;
	WORD		rbld;
	
	tree = G.a_trees[ADSETPRE];
	rbld = FALSE;

	cyes = (G.g_cdelepref) ? SELECTED : NORMAL;
	cno = (G.g_cdelepref) ? NORMAL : SELECTED;
	tree[SPCD].ob_state |= cyes;
	tree[SPCD].ob_state &= ~cno;

	cyes = (G.g_ccopypref) ? SELECTED : NORMAL;
	cno = (G.g_ccopypref) ? NORMAL : SELECTED;
	tree[SPCC].ob_state |=  cyes;
	tree[SPCC].ob_state &= ~cno;

	cyes = (G.g_covwrpref) ? SELECTED : NORMAL;
	cno = (G.g_covwrpref) ? NORMAL : SELECTED;
	tree[SPCO].ob_state |=  cyes;
	tree[SPCO].ob_state &= ~cno;

	if (G.g_cmclkpref) 
	{
		tree[SPMNCLKY].ob_state |= SELECTED; 
		tree[SPMNCLKN].ob_state &= ~SELECTED;
	}
	else
	{
		tree[SPMNCLKN].ob_state |= SELECTED; 
		tree[SPMNCLKY].ob_state &= ~SELECTED;
	}

	if (G.g_ctimeform) 
	{
		tree[SPTF12HR].ob_state |= SELECTED; 
		tree[SPTF24HR].ob_state &= ~SELECTED;
	}
	else
	{
		tree[SPTF24HR].ob_state |= SELECTED; 
		tree[SPTF12HR].ob_state &= ~SELECTED;
	}

	if (G.g_cdateform) 
	{
		tree[SPDFMMDD].ob_state |= SELECTED; 
		tree[SPDFDDMM].ob_state &= ~SELECTED;
	}
	else
	{
		tree[SPDFDDMM].ob_state |= SELECTED; 
		tree[SPDFMMDD].ob_state &= ~SELECTED;
	}
	if (G.g_detdrives)   tree[SPNOSCAN].ob_state &= ~SELECTED;
	else		     tree[SPNOSCAN].ob_state |=  SELECTED;
	if (G.g_probedrives) tree[SPNONET ].ob_state &= ~SELECTED;
	else		     tree[SPNONET ].ob_state |=  SELECTED;


	for(i=0; i<5; i++)
	  tree[SPDC1+i].ob_state &= ~SELECTED;

	G.g_cdclkpref = evnt_dclick(0, FALSE);
	tree[SPDC1+G.g_cdclkpref].ob_state |= SELECTED;

	sndefpref = !desk_sound(FALSE, -1, 0);

	cyes = (sndefpref) ? SELECTED : NORMAL;
	cno = (sndefpref) ? NORMAL : SELECTED;
	tree[SPSE].ob_state &= ~cno;
	tree[SPSE].ob_state |= cyes;

	inf_show(tree, 0);

	if ( inf_what(tree, SPOK, SPCNCL) )
	{
	  G.g_cdelepref = (tree[SPCD    ].ob_state & SELECTED) ? 1 : 0;
	  G.g_ccopypref = (tree[SPCC    ].ob_state & SELECTED) ? 1 : 0;
	  G.g_covwrpref = (tree[SPCO    ].ob_state & SELECTED) ? 1 : 0;
	  G.g_cmclkpref = (tree[SPMNCLKY].ob_state & SELECTED) ? 1 : 0;
	  G.g_detdrives = (tree[SPNOSCAN].ob_state & SELECTED) ? 0 : 1;
	  G.g_probedrives=(tree[SPNONET ].ob_state & SELECTED) ? 0 : 1;
	  G.g_cmclkpref = menu_click(G.g_cmclkpref, TRUE);
	  G.g_cdclkpref = inf_gindex(tree, SPDC1, 5);
	  G.g_cdclkpref = evnt_dclick(G.g_cdclkpref, TRUE);
	  sndefpref     = (tree[SPSE].ob_state & SELECTED) ? 1 : 0;
					/* changes if file display? */
	  cyes = (tree[SPTF12HR].ob_state & SELECTED) ? 1 : 0;
	  if (G.g_ctimeform != cyes)
	  {
	    rbld = (G.g_iview == V_TEXT);
	    G.g_ctimeform = cyes;
	  }
	  cyes = (tree[SPDFMMDD].ob_state & SELECTED) ? 1 : 0;
	  if (G.g_cdateform != cyes)
	  {
	    rbld |= (G.g_iview == V_TEXT);
	    G.g_cdateform = cyes;
	  }
	  desk_sound(FALSE, !sndefpref, 0);
	}
	return(rbld);
} /* inf_pref */

/*
*	Open application icon
*/
WORD opn_appl(BYTE *papname, BYTE *papparms, BYTE *pcmd, BYTE *ptail, 
		WORD taillen)
{
	LPTREE		tree;
	BYTE		poname[13];

	tree = G.a_trees[ADOPENAP];

	fmt_str(papname, &poname[0]);

	inf_sset(tree, APPLNAME, &poname[0]);

	inf_sset(tree, APPLPARM, papparms);

	inf_show(tree, APPLPARM);
					/* now find out what happened	*/
	if ( inf_what(tree, APPLOK, APPLCNCL) )
	{
	  inf_sget(tree, APPLNAME, &poname[0], sizeof(poname));
	  unfmt_str(&poname[0], pcmd);
	  inf_sget(tree, APPLPARM, ptail, taillen);
	  return(TRUE);
	}
	else
	  return(FALSE);
}
