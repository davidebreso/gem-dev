/*	DESKINS.C	09/22/84 - 06/19/85	Lee Lorenzen		*/
/*	for 3.0		02/28/86			MDF		*/
/*	merge source	5/27/87	 - 5/28/87		mdf		*/

/*
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985 - 1987		Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "ppddesk.h"

/* forward declarations	*/
MLOCAL VOID   ob_actxywh();
MLOCAL VOID   ob_relxywh();

/*
*	Routine to tell if an icon has an associated document type.
*/


WORD __near is_installed(ANODE *pa)
{
	return ( !((*pa->a_pappl == '*') || (*pa->a_pappl == '?') ||
		   (*pa->a_pappl == 0)  ) );
}


/*
*	Routine to find out if this icon is the last disk icon on the
*	screen.
*/
MLOCAL 	WORD 
lastdisk()
{
	WORD 		i;
	ANODE		*pa;

	i = 0;
	for(pa=G.g_ahead; pa; pa=pa->a_next)
	{
	  if (pa->a_type == AT_ISDISK)
	    i++;
	}
	return( (i < 2) );
}


/*
*	Routine to place disk icon between the current disk icon and
*	the trash can
*/

/* DESKTOP v1.2: uncommented */
/* JCE 14-8-2005: Try to reduce the likelihood of one icon appearing 
 * atop another */
VOID __near ins_posdisk(WORD dx, WORD dy, WORD *pdx, WORD *pdy)
{
	WORD		tx, ty, na, n, nx, ny;
	WORD		xdiff, ydiff, xdir, ydir;
	ANODE		*pa;
	WORD		ax[NUM_ANODES], ay[NUM_ANODES];

	na = 0;
	/* Store the coordinates of all desktop objects */
	for(pa=G.g_ahead; pa; pa=pa->a_next)
	{
	  if (pa->a_type == AT_ISDISK || pa->a_type == AT_ISTRSH ||
	      pa->a_type == AT_ISGRP)
	  {
	    ax[na] = (pa->a_xspot - G.g_xdesk) / G.g_icw;
	    ay[na] = (pa->a_yspot - G.g_ydesk) / G.g_ich;
	    ++na;
	  }
	  if (pa->a_type == AT_ISTRSH)
	  {
	    tx = pa->a_xspot;
	    ty = pa->a_yspot;
	  }
	}

	xdiff = tx - dx;
	ydiff = ty - dy;

	xdir = (xdiff < 0) ? -1 : 1;
	ydir = (ydiff < 0) ? -1 : 1;

	xdiff *= xdir;
	ydiff *= ydir;

	if (ydiff > xdiff)
	{
	  *pdx = dx;
	  *pdy = dy + (G.g_ich * ydir);
/* If we've gone off the screen, try going the other way */
	  if (*pdy >= G.g_ydesk + G.g_hdesk) *pdy = dy - (G.g_ich * ydir);
	}
	else
	{
	  *pdx = dx + (G.g_icw * xdir);
	  *pdy = dy;
/* If we've gone off the screen, try going the other way */
	  if (*pdx >= G.g_xdesk + G.g_wdesk) *pdx = dx - (G.g_icw * xdir);
	}

/* See if this icon appears on top of any others. If it does, find a 
 * new home for it. Set dx and dy to tend toward the centre of the screen. */
	if (*pdx >= G.g_xdesk + (G.g_wdesk / 2)) dx = -G.g_icw;
	else					 dx =  G.g_icw;
	if (*pdy >= G.g_ydesk + (G.g_ydesk / 2)) dy = -G.g_ich;
	else					 dy =  G.g_ich;

	nx = (pdx[0] - G.g_xdesk) / G.g_icw;
	ny = (pdy[0] - G.g_ydesk) / G.g_ich;
	for (n = 0; n < na; n++)
	{
	    if (nx == ax[n] && ny == ay[n]) /* Conflict! */
	    {
/* Move one X-step. */
	      *pdx += dx;
/* If that's off the edge of the screen, move one Y-step and reset X */
	      if (*pdx >= G.g_xdesk + G.g_wdesk) 
	      {
		*pdx = G.g_xdesk;
		*pdy += dy;
	      }
	      if (*pdx < 0)
	      {
		*pdx = G.g_xdesk - G.g_icw;
		*pdy += dy;
	      }
/* If Y has moved off the edge, come on at the opposite edge */
	      if (*pdy < G.g_ydesk)              
		      *pdy = G.g_ydesk + G.g_hdesk - G.g_ich;
	      if (*pdy >= G.g_ydesk + G.g_hdesk) 
		      *pdy = G.g_ydesk;
	      nx = (pdx[0] - G.g_xdesk) / G.g_icw;
	      ny = (pdy[0] - G.g_ydesk) / G.g_ich;
	      n = -1;	/* Restart loop */
	  }

	}
}


/*
*	Routine to find out if there is another icon with this letter already
*	on the desktop.
*/
MLOCAL ANODE *  get_disk(WORD letter)
{
	ANODE		*pa;

	for(pa=G.g_ahead; pa; pa=pa->a_next)
	{
	  if ((pa->a_type == AT_ISDISK) &&
	      (pa->a_letter == letter) )
	    return(pa);
	}
	return( 0 );
}

/************************************************************************/
/* i n s _ d i s k							*/
/************************************************************************/
/* Modified 19990721 by BALJ for CD-ROM/Network icon support.           */
/* Also added 'Unknown' button - displays 5.25" icon, for removables?   */
/************************************************************************/
WORD __near ins_disk(ANODE *pa)
{			       
	LPTREE		tree;
	WORD		change, icon, flop, hard, cd, net, other;
	BYTE		cletter[2], clabel[13];
	BYTE		nletter[2], nlabel[13];
	ANODE		*newpa;

	tree = G.a_trees[ADINSDIS];

	change = FALSE;
	cletter[0] = pa->a_letter;
	cletter[1] = 0;
	strlcpy(clabel, pa->a_pappl, sizeof(clabel));

	inf_sset(tree, DRID, &cletter[0]);
	inf_sset(tree, DRLABEL, &clabel[0]);

	flop  = (pa->a_aicon == IG_FLOPPY) ? SELECTED : NORMAL;
	hard  = (pa->a_aicon == IG_HARD)   ? SELECTED : NORMAL;
	cd    = (pa->a_aicon == IG_CD)     ? SELECTED : NORMAL;  /* BALJ */
	net   = (pa->a_aicon == IG_NET)    ? SELECTED : NORMAL;
	other = (pa->a_aicon == IG_5QRTR)  ? SELECTED : NORMAL;

	if (flop)  tree[DRFLOPPY].ob_state |= SELECTED;
	else	   tree[DRFLOPPY].ob_state &= ~SELECTED;
	if (hard)  tree[DRHARD  ].ob_state |= SELECTED;
	else	   tree[DRHARD  ].ob_state &= ~SELECTED;
	if (cd)    tree[DRCD    ].ob_state |= SELECTED;
	else	   tree[DRCD    ].ob_state &= ~SELECTED;
	if (net)   tree[DRNET   ].ob_state |= SELECTED;
	else	   tree[DRNET   ].ob_state &= ~SELECTED;
	if (other) tree[DR5QRTR ].ob_state |= SELECTED;
	else	   tree[DR5QRTR ].ob_state &= ~SELECTED;
	if (lastdisk()) tree[DRREM].ob_state |=  DISABLED;
	else            tree[DRREM].ob_state &= ~DISABLED;

	inf_show(tree, 0);	

	inf_sget(tree, DRID, &nletter[0], sizeof(nletter));
	inf_sget(tree, DRLABEL, &nlabel[0], sizeof(nlabel));
	if      (tree[DRFLOPPY ].ob_state & SELECTED) icon = IG_FLOPPY;
	else if (tree[DRHARD   ].ob_state & SELECTED) icon = IG_HARD;
	else if (tree[DRCD     ].ob_state & SELECTED) icon = IG_CD;
	else if (tree[DRNET    ].ob_state & SELECTED) icon = IG_NET;
	else /*if (tree[DR5QRTR].ob_state & SELECTED)*/ icon = IG_5QRTR;

	if ( tree[DRINST].ob_state & SELECTED) /* Install */
	{
	  pa->a_flags &= ~AF_WASDET;	/* No longer an auto-detected drive */
	  /* (keeps customisations - BALJ) */
/* BugFix	*/
	  if ( (cletter[0] != nletter[0]) && (nletter[0] != 0) )
	  {
	    newpa = get_disk(nletter[0]);
	    if (!newpa)
	    {
	      newpa = app_alloc(FALSE);
	      if (newpa)
	      {
	        newpa->a_flags = pa->a_flags;
	        newpa->a_type = pa->a_type;
  	        newpa->a_obid = pa->a_obid;
	        newpa->a_pappl = pa->a_pappl;
		scan_str("@", &newpa->a_pdata);
	        newpa->a_aicon = pa->a_aicon;
   	        newpa->a_dicon = NIL;
  	        newpa->a_letter = nletter[0];
  	        /* DESKTOP v1.2 */
  	        ins_posdisk(pa->a_xspot, pa->a_yspot, &newpa->a_xspot, 
  	        			&newpa->a_yspot);

  	        snap_disk(newpa->a_xspot, newpa->a_yspot, 
  	        			&newpa->a_xspot, &newpa->a_yspot);			
	      } /* if newpa */
	      else
	        fun_alert(1, STAPGONE);
	    } /* if !newpa */
	    if (newpa) pa = newpa;
	    change = TRUE;
	  } /* if cletter */
						/* see if icon changed	*/
	  if (pa->a_aicon != icon)
	  {
	    pa->a_aicon = icon;
	    change = TRUE;
	  }
						/* see if label changed	*/
/* BugFix	*/
	  if ( (strcmp(&clabel[0], &nlabel[0])) && (nlabel[0] != 0) )
	  {
	    nlabel[ strlen(&nlabel[0]) ] = '@';
	    scan_str(&nlabel[0], &pa->a_pappl);
	    change = TRUE;
	  }
	} /* if INSTALL */
	else if ( tree[DRREM].ob_state & SELECTED )			/* Remove 		*/
	{
						/* find matching anode	*/
						/*   delete it		*/
	  for (pa = G.g_ahead; pa; pa = pa->a_next)
	  {
	    if ( (pa->a_aicon == icon) && (pa->a_letter == nletter[0]) )
	    {
	      app_free(pa);
	      change = TRUE;
	    }
	  } /* for */
	} /* if REMOVE */
	tree[DRINST].ob_state &= ~SELECTED;
	tree[DRREM ].ob_state &= ~SELECTED;
	tree[DRCNCL].ob_state &= ~SELECTED;
	
	return(change);
} /* ins_disk */

MLOCAL VOID  insa_icon(LPTREE tree, WORD obj, WORD nicon, 
			   ICONBLK *pic, BYTE *ptext)
{
	memcpy(pic, &G.g_iblist[nicon], sizeof(*pic));
	pic->ib_ptext = ADDR( ptext );
	tree[obj].ob_type = G_ICON;
	tree[obj].ob_spec = ADDR(pic);
}


MLOCAL VOID  insa_elev(LPTREE tree, WORD nicon, WORD numics)
{
	WORD		y, h, th;
	LPBYTE		lp;
	LPWORD		lw;

	y = 0;
	th = h = tree[APFSVSLI].ob_height;  
	if ( numics > 1)
	{
	  h = x_mul_div(1, h, numics);
	  if (h < ((gl_hbox/2) + 2)) h = (gl_hbox / 2) + 2;
	  y = x_mul_div(nicon, th-h, numics-1);
	}
	tree[APFSVELE].ob_y = y;
	tree[APFSVELE].ob_height = h;

	strlcpy(G.g_1text, ini_str(STAPPL), sizeof(G.g_1text));
	insa_icon(tree, APF1NAME, IA_GENERIC+nicon, &gl_aib, &G.g_1text[0]);

	strlcpy(G.g_2text, ini_str(STDOCU), sizeof(G.g_2text));
	insa_icon(tree, APF2NAME, ID_GENERIC+nicon, &gl_dib, &G.g_2text[0]);

	/* [JCE 6-5-2002] Remove superfluous & from array dereferences */
	lw = (LPWORD)(G.a_datastart + gl_iconstart);
	lp = G.a_datastart + lw[nicon];

	lstlcpy(ADDR(&gl_lngstr[0]), lp, sizeof(gl_lngstr));
	inf_sset(tree, APFTITLE, &gl_lngstr[0] );
} /* insa_elev */


MLOCAL WORD  insa_dial(LPTREE tree, WORD nicon, WORD numics)
{
	WORD		firstslot, ystate, i;
	WORD		touchob, oicon, value;
	WORD		mx, my, kret, bret, cont;
	BYTE		*pstr, doctype[4];
	GRECT		pt;
						/* draw the form	*/
	show_hide(FMD_START, tree);
						/* init for while loop	*/
						/*   by forcing initial	*/
						/*   fs_newdir call	*/
	
	cont = TRUE;
	while( cont )
	{
	  firstslot = 6;
	  for(i = 0; i < firstslot; i++)
	  {
	    pstr = &doctype[0];
	    inf_sget(tree, APDFTYPE+i, pstr, sizeof(doctype));
	    if (*pstr == 0)
	      firstslot = i;
	  }
	  touchob = form_do(tree, APDFTYPE+firstslot);
	  graf_mkstate(&mx, &my, &kret, &bret);
	
	  value = 0;
	  ystate = tree[APYMEM].ob_state;
	  touchob &= 0x7fff;
	  switch( touchob )
	  {
	    case APINST:
	    case APREMV:
	    case APCNCL:
		cont = FALSE;
		break;
	    case APFUPARO:
		value = -1;
		break;
	    case APFDNARO:
		value = 1;
		break;
	    case APGEM:
		ystate |= DISABLED;
		ystate &= ~SELECTED;
		break;
	    case APDOS:
	    case APPARMS:
		ystate &= ~DISABLED;
		break;
	    case APFSVSLI:
/* BugFix	*/
		ob_actxywh(tree, APFSVELE, &pt);
		pt.g_x -= 3;
		pt.g_w += 6;
		if ( inside(mx, my, &pt) )
		  goto dofelev;
		value = (my <= pt.g_y) ? -1 : 1;
		break;
	    case APFSVELE:
dofelev:	wind_update(3);
		ob_relxywh(tree, APFSVSLI, &pt);
		pt.g_x += 3;
		pt.g_w -= 6;
		tree[APFSVSLI].ob_x     = pt.g_x;
		tree[APFSVSLI].ob_width = pt.g_w;
		value = graf_slidebox(tree, APFSVSLI, APFSVELE, TRUE);
		pt.g_x -= 3;
		pt.g_w += 6;
		tree[APFSVSLI].ob_x     = pt.g_x;
		tree[APFSVSLI].ob_width = pt.g_w;
		wind_update(2);
		value = x_mul_div(value, numics-1, 1000) - nicon;
		break;
	  }
	  tree[APYMEM].ob_state = ystate;
	  draw_fld(tree, APYMEM);

	  if (value)
	  {
	    oicon = nicon;
	    nicon += value;
	    if (nicon < 0)
	      nicon = 0;
	    if (nicon >= numics)
	      nicon = numics - 1;	
	    if (oicon != nicon)
	    {
	      insa_elev(tree, nicon, numics);
	      draw_fld(tree, APFTITLE);
	      draw_fld(tree, APFSVSLI);
	      draw_fld(tree, APFILEBO);
	    }
	  }
	}
						/* undraw the form	*/
	show_hide(FMD_FINISH, tree);
	return(nicon);
}

MLOCAL VOID  insa_gtypes(LPTREE tree, BYTE *ptypes)
{
	WORD		i, j;
	BYTE		*pstr, doctype[4];

	j = 0;
	*ptypes = 0;
	for(i=0; i<8; i++)
	{
	  pstr = &doctype[0];
	  inf_sget(tree, APDFTYPE+i, pstr, sizeof(doctype));
	  if (*pstr)
	  {
	    if (j != 0) ptypes[j++] = ','; 
	    strlcpy(ptypes + j, "*.*", 4);
	    strlcpy(ptypes + j + 2, pstr, 1 + strlen(pstr));
	    j += 2 + strlen(pstr);
	  }
	}
}



MLOCAL VOID  insa_stypes(LPTREE tree, BYTE *pdata)
{
	WORD		i;
	BYTE		*pstr, doctype[4];

	for(i=0; i<8; i++)
	{
	  pdata = scasb(pdata, '.');
	  if (*pdata == '.') pdata++;
	  pstr = &doctype[0];
	  while ( (*pdata) && (*pdata != ',') )
	    *pstr++ = *pdata++;
	  *pstr = 0;
	  inf_sset(tree, APDFTYPE + i, doctype);
	}
}


#if MULTIAPP

MLOCAL 	WORD 
ins_latoi(st_ad)
	LONG	st_ad;
{
	WORD	retval;
	BYTE	ch;

	retval = 0;
	while ((ch = LBGET(st_ad)) != '\0')
	{
	  retval = retval*10 + ch - '0';
	  st_ad += 1;
	}
	return(retval);
}

#endif



/************************************************************************/
/* i n s _ a p p	  						*/
/************************************************************************/
WORD __near ins_app(BYTE *pfname, ANODE *pa)
{			       
	LPTREE		tree;
	ANODE		*newpa;
	BYTE		pname[12];
	BYTE		ntypes[6*8];
	WORD		oicon, nicon;
	WORD		oflag, nflag;
	WORD		change, field;
	WORD		uninstalled, h;
#if MULTIAPP
	BYTE		memszstr[4];
	WORD		omemsz, nmemsz;
#endif
	
	tree = G.a_trees[ADINSAPP];
#if MULTIAPP
/*	tree[APMEMBOX].ob_next = APMEMSZ; */
#else
	tree[APMEMSZ].ob_flags |= HIDETREE;
#endif

	
	h = tree[APSCRLBA].ob_height;
	tree[APFUPARO].ob_height = gl_hbox + 2;
	tree[APFSVSLI].ob_y      = gl_hbox + 2;
	tree[APFSVSLI].ob_height = h - (2 * (gl_hbox + 2));
	tree[APFDNARO].ob_y      = h -      (gl_hbox + 2);
	tree[APFDNARO].ob_height = gl_hbox + 2;

	uninstalled = !is_installed(pa);
	if (uninstalled) tree[APREMV].ob_state |=  DISABLED;
	else			 tree[APREMV].ob_state &= ~DISABLED;
						/* stuff in appl name	*/
	fmt_str(pfname, &pname[0]);
	inf_sset(tree, APNAME, &pname[0]);
						/* stuff in docu types	*/
	insa_stypes(tree, pa->a_pdata);
/* BugFix	*/
	tree[APYMEM].ob_state &= ~SELECTED;
/* */
	oflag = pa->a_flags;
	if (pa->a_flags & AF_ISCRYS)
	{
	  field = APGEM;
	  tree[APYMEM].ob_state |= DISABLED;
	  tree[APYMEM].ob_state &= ~SELECTED;
	}
	else
	  field = (pa->a_flags & AF_ISPARM) ? APPARMS : APDOS;
	tree[field].ob_state |= SELECTED;

	if (pa->a_flags & AF_ISFMEM) tree[APYMEM].ob_state |= SELECTED;

	oicon = pa->a_aicon - IA_GENERIC;

#if MULTIAPP
	omemsz = pa->a_memreq;
	merge_str(&memszstr[0], "%W", omemsz);
	inf_sset(tree, APMEMSZ, &memszstr[0]);
#endif

	insa_elev(tree, oicon, gl_numics);
	nicon = insa_dial(tree, oicon, gl_numics);
	change = FALSE;

#if MULTIAPP
	{
		LPTEDI tedi = (LPTEDI)tree[APMEMSZ].ob_spec;
		LPBYTE text = tedi->te_ptext;
		nmemsz = ins_latoi(text);
	}
	/* nmemsz = ins_latoi(LLGET(LLGET(OB_SPEC(APMEMSZ)))); */
#endif

						/* set memory flag	*/
	nflag = (tree[APYMEM].ob_state & SELECTED) ? AF_ISFMEM : 0;
						/* set type flags	*/
	field = inf_gindex(tree, APGEM, 3);
	if (field == 0)
	  nflag = AF_ISCRYS | AF_ISGRAF;
	if (field == 2)
	  nflag |= AF_ISPARM;
	tree[APGEM + field].ob_state &= ~SELECTED;
						/* get button selection	*/

	if ( tree[APINST].ob_state & SELECTED)
	{
						/* install the appl.	*/
						/*   if its uninstalled	*/
						/*   or has new types	*/
	  insa_gtypes(tree, &ntypes[0]);
	  if ( (uninstalled) ||
	       (strcmp(&ntypes[0], pa->a_pdata)) )
	  {
	    newpa = (uninstalled) ? app_alloc(TRUE) : pa;

	    if (newpa)
	    {
	      if ( (uninstalled) ||
		   (strcmp(&ntypes[0], pa->a_pdata)) )
	      {
		change = TRUE;
	        ntypes[ strlen(&ntypes[0]) ] = '@';
	        scan_str(&ntypes[0], &newpa->a_pdata);
	      }

	      if (newpa != pa)
	      {
		uninstalled = change = TRUE;
	        strcpy(ntypes, pfname);
	        ntypes[ strlen(&ntypes[0]) ] = '@';
	        scan_str(&ntypes[0], &newpa->a_pappl);
	        newpa->a_flags = nflag;
	        newpa->a_type = AT_ISFILE;
	        newpa->a_obid = NIL;
	  	newpa->a_letter = 0;
		newpa->a_xspot = 0x0;
		newpa->a_yspot = 0x0;
	      }
	      pa = newpa;
	    }
	    else
	      fun_alert(1, STAPGONE);
	  }
						/* see if icon changed	*/
						/*   or flags changed	*/
	  if ( (uninstalled) ||
#if MULTIAPP
	       (omemsz != nmemsz) ||
#endif
	       (oicon != nicon) ||
	       (oflag != nflag) )
	  {
	    change = TRUE;
	    pa->a_aicon = nicon + IA_GENERIC;
	    pa->a_dicon = nicon + ID_GENERIC;
	    pa->a_flags = nflag;
#if MULTIAPP
	    pa->a_memreq = nmemsz;
#endif
	  }
	}
	else if ( tree[APREMV].ob_state & SELECTED)
	{
						/* remove installed app	*/
	  if ( !uninstalled )
	  {
	    app_free(pa);
	    change = TRUE;
	  }
	}
	tree[APCNCL].ob_state &= ~SELECTED;
	tree[APREMV].ob_state &= ~SELECTED;
	tree[APINST].ob_state &= ~SELECTED;

	return(change);
} /* ins_app */

/*------------------------------*/
/*	ob_actxywh		*/
/*------------------------------*/
MLOCAL VOID  ob_actxywh(LPTREE tree, WORD obj, GRECT *p)
{
				/* get x,y,w,h for specified object	*/
	objc_offset(tree, obj, &p->g_x, &p->g_y);
	p->g_w = tree[obj].ob_width;
	p->g_h = tree[obj].ob_height;
} /* ob_actxywh */

/*------------------------------*/
/*	ob_relxywh		*/
/*------------------------------*/
MLOCAL VOID  ob_relxywh(LPTREE tree, WORD obj, GRECT *prect)
{
				/* get x,y,w,h for specified object	*/
	LWCOPY(ADDR(prect), (LPVOID)&tree[obj].ob_x, sizeof(GRECT) / 2);
} /* ob_relxywh */
