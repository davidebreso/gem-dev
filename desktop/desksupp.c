/*	DESKSUPP.C	05/04/84 - 06/20/85	Lee Lorenzen		*/
/*	for 3.0 (xm)	3/12/86	 - 01/17/87	MDF			*/
/*	for 3.0			11/13/87		mdf		*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 3.0
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "wccdesk.h"

/*
*	Clear out the selections for this particular window
*/
VOID  desk_clear(WORD wh)
{
	WNODE		*pw;
	GRECT		c;
	WORD	    root;
	
	// fprintf(logfile, "desk_clear(%d)\n", wh);
						/* get current size	*/
	wind_get(wh, WF_WXYWH, &c.g_x, &c.g_y, &c.g_w, &c.g_h);
						/* find its tree of 	*/
						/*   items		*/
	pw = win_find(wh);

	/* DESKTOP v1.2: We have to clear selected desktop objects */
	if (wh != DESKWH) root = pw->w_root;
	else    root = DROOT;
						/* clear all selections	*/
	act_allchg(wh, G.a_screen, root, 0, &gl_rfull, &c, FALSE, TRUE);
	// fprintf(logfile, "END of desk_clear()\n");
}

/*
*	Verify window display by building a new view.
*/
VOID  desk_verify(WORD wh, WORD changed)
{
	WNODE		*pw;
	WORD		xc, yc, wc, hc;

    // fprintf(logfile, "desk_verify(%d)\n", wh);
	/* DESKTOP v1.2: The desktop itself... */
	if (wh)
	{ 
		pw = win_find(wh);
		if (pw)
		{
		  if (changed)
		  {
	    	wind_get(wh, WF_WXYWH, &xc, &yc, &wc, &hc);
	    	win_bldview(pw, xc, yc, wc, hc);
	  	  }
	  	G.g_croot = pw->w_root;
		}
	}
	else G.g_croot = 1;	// DESKTOP v1.2: The Desktop

	G.g_cwin = wh;
	G.g_wlastsel = wh;
}


WORD  do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc)
{
	GRECT		clip_r, t;
	WNODE		*pw;
	LPTREE		tree = G.a_screen;	// DESKTOP v1.2
	WORD		root;

	clip_r.g_x = xc;
	clip_r.g_y = yc;
	clip_r.g_w = wc;
	clip_r.g_h = hc;

	if (w_handle != 0)
	{
	  pw = win_find(w_handle);
	//  tree = G.a_screen; // DESKTOP v1.2 does this above
	  root = pw->w_root;
	}
	else root = 1;    // DESKTOP v1.2 draws the desk  return( TRUE );

	graf_mouse(M_OFF, 0x0L);

	wind_get(w_handle, WF_FIRSTXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
	while ( t.g_w && t.g_h )
	{
	  if ( rc_intersect(&clip_r, &t) )
	  {
	    objc_draw(tree, root, MAX_DEPTH, t.g_x, t.g_y, t.g_w, t.g_h);
	  }
	  wind_get(w_handle, WF_NEXTXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
	}
	graf_mouse(M_ON, 0x0L);
	return FALSE;
}


/*
*	Picks ob_x, ob_y, ob_width, ob_height fields out of object list.
*/

VOID  get_xywh(OBJECT *olist, WORD obj, WORD *px, WORD *py, WORD *pw, WORD *ph)
{
	*px = olist[obj].ob_x;
	*py = olist[obj].ob_y;
	*pw = olist[obj].ob_width;
	*ph = olist[obj].ob_height;
}

/*
*	Picks ob_spec field out of object list.
*/

LPICON  get_spec(OBJECT *olist, WORD obj)
{
	return (LPICON)(olist[obj].ob_spec);
}

VOID  do_xyfix(WORD *px, WORD *py)
{
	WORD		tx, ty, tw, th;

	wind_get(0, WF_WXYWH, &tx, &ty, &tw, &th);
	tx = *px;
	*px = (tx & 0x000f);
	if ( *px < 8 ) *px =  tx & 0xfff0;
	else           *px = (tx & 0xfff0) + 16;
	*py = max(*py, ty);
}

VOID  do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h)
{
	GRECT		d,c;
    
    // fprintf(logfile, "do_wopen()\n");
	do_xyfix(&x, &y);
	// DESKTOP v1.2: Zooming box effect
	get_xywh(G.g_screen, curr,      &d.g_x, &d.g_y, &d.g_w, &d.g_h);
	get_xywh(G.g_screen, G.g_croot, &c.g_x, &c.g_y, &c.g_w, &c.g_h);

	// DESKTOP v1.2: Zooming box effect
	d.g_x += c.g_x;
	d.g_y += c.g_y;
				
	graf_growbox(d.g_x, d.g_y, d.g_w, d.g_h, x, y, w, h);
	
	act_chg(G.g_cwin, G.a_screen, G.g_croot, curr, &c, FALSE, TRUE, TRUE);
	if (new_win)
	{
	  wind_open(wh, x, y, w, h);
	}
	G.g_wlastsel = wh;
}


/* DESKTOP v1.2 version... */
VOID  do_wfull(WORD wh)
{
	GRECT		curr, prev, full;

	gl_whsiztop = NIL;
	wind_get(wh, WF_CXYWH, &curr.g_x, &curr.g_y, &curr.g_w, &curr.g_h);
	wind_get(wh, WF_PXYWH, &prev.g_x, &prev.g_y, &prev.g_w, &prev.g_h);
	wind_get(wh, WF_FXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);

	if (rc_equal(&curr, &full))
	{
		wind_set(wh, WF_CXYWH, prev.g_x, prev.g_y, prev.g_w, prev.g_h);
		graf_shrinkbox(prev.g_x, prev.g_y, prev.g_w, prev.g_h,
					   full.g_x, full.g_y, full.g_w, full.g_h);
	}	
	else
	{
		graf_growbox(curr.g_x, curr.g_y, curr.g_w, curr.g_h,
					 full.g_x, full.g_y, full.g_w, full.g_h);
		wind_set(wh, WF_CXYWH, full.g_x, full.g_y, full.g_w, full.g_h);
		
	}
}



/* Crippled DESKTOP v2.x+ version...
MLOCAL VOID do_wfull(WORD wh)
{
	WORD		tmp_wh, y;
	GRECT		curr, prev, full, temp;

	gl_whsiztop = NIL;
	wind_get(wh, WF_CXYWH, &curr.g_x, &curr.g_y, &curr.g_w, &curr.g_h);
	wind_get(wh, WF_PXYWH, &prev.g_x, &prev.g_y, &prev.g_w, &prev.g_h);
	wind_get(wh, WF_FXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);
			// have to check for shrinking a window that	//
			// was full when Desktop was first started.	//
	if ( (rc_equal(&curr, &prev)) && (curr.g_h > gl_normwin.g_h) )
	{	// shrink full window		//
		// find the other window (assuming only 2 windows)	//
	  if ( G.g_wlist[0].w_id == wh)
	    tmp_wh = G.g_wlist[1].w_id;
	  else
	    tmp_wh = G.g_wlist[0].w_id;
	    			// decide which window we're shrinking	//
	  wind_get(tmp_wh, WF_CXYWH, &temp.g_x, &temp.g_y,
	  	   &temp.g_w, &temp.g_h);
	  if (temp.g_y > gl_normwin.g_y)
	    y = gl_normwin.g_y;		// shrinking upper window	//
	  else				// shrinking lower window	//
	    y = gl_normwin.g_y + gl_normwin.g_h + (gl_hbox / 2);
 	  wind_set(wh, WF_CXYWH, gl_normwin.g_x, y,
	  	   gl_normwin.g_w, gl_normwin.g_h);
	} // if //
					// already full, so change	//
					// back to previous		//
	else if ( rc_equal(&curr, &full) )
	  wind_set(wh, WF_CXYWH, prev.g_x, prev.g_y, prev.g_w, prev.g_h);
	  				// make it full			//
	else
	{
	  gl_whsiztop = wh;
	  wind_set(wh, WF_SIZTOP, full.g_x, full.g_y, full.g_w, full.g_h);
	}
} // do_wfull //
*/


/*
*	Open a directory, it may be the root or a subdirectory.
*/

WORD  do_diropen(WNODE *pw, WORD new_win, WORD curr_icon, WORD drv, 
				BYTE *ppath, BYTE *pname, BYTE *pext, GRECT *pt,
				WORD redraw)
{
	WORD		ret;
	PNODE		*tmp;

    // fprintf(logfile, "do_diropen: ppath=%s pname=%s pext=%s\n", ppath, pname, pext);	
						/* convert to hourglass	*/
	graf_mouse(HGLASS, 0x0L);
						/* open a path node	*/
	tmp = pn_open(drv, ppath, pname, pext, F_SUBDIR);
	if ( tmp == NULL)
	{
 	  //dbg("do_diropen: pn_open failed; returning.\n");
	  graf_mouse(ARROW, 0x0L);
	  return(FALSE);
	}
	else
	  pw->w_path = tmp;


						/* activate path by 	*/
						/*   search and sort	*/
						/*   of directory	*/
	ret = pn_active(pw->w_path);
	if ( ret != E_NOFILES )
	{
						/* some error condition	*/
	}
						/* set new name and info*/
						/*   lines for window	*/
	win_sname(pw);
	win_sinfo(pw);		// DESKTOP v1.2 reinstated 
	wind_setl(pw->w_id, WF_NAME, ADDR(&pw->w_name[0]));
	wind_setl(pw->w_id, WF_INFO, ADDR(&pw->w_info[0])); // DESKTOP v1.2
	
						/* do actual wind_open	*/
	if (curr_icon)
	{
		do_wopen(new_win, pw->w_id, curr_icon, 
				pt->g_x, pt->g_y, pt->g_w, pt->g_h);
		if (new_win)
	  		win_top(pw);
	}
						/* verify contents of	*/
						/*   windows object list*/
						/*   by building view	*/
						/*   and make it curr.	*/
	desk_verify(pw->w_id, TRUE);
						/* make it redraw	*/
	if (/*redraw && */( !new_win ))
	  fun_msg(WM_REDRAW, pw->w_id, pt->g_x, pt->g_y, pt->g_w, pt->g_h);

    // fprintf(logfile,"END OF do_diropen\n");
	graf_mouse(ARROW, 0x0L);
	return(TRUE);
} /* do_diropen */

/*
*	Open an application
*/

MLOCAL WORD  do_aopen(ANODE *pa, WORD atype, WORD curr, WORD drv, 
			  BYTE *ppath, BYTE *pname)
{
	WORD		ret, done;
	WORD		isgraf, isover, isparm;
	BYTE		*ptmp, *pcmd, *ptail;
	BYTE		name[13];

	// dbg("do_aopen: %c %s %s\n", drv, ppath, pname);

	done = FALSE;
						/* set flags		*/
	isgraf = pa->a_flags & AF_ISGRAF;
#if MULTIAPP
	pr_kbytes = pa->a_memreq;		/* K bytes needed for app */
	isover = (pa->a_flags & AF_ISFMEM) ? 2 : -1;
	if ((isover == 2) && gl_keepac)		/* full-step ok?	*/
	{
	  rsrc_gaddr(R_STRING, STNOFSTP, (LPVOID *)&G.a_alert);
	  form_alert(1, G.a_alert);
	  return(FALSE);
	}
#else
	isover = (pa->a_flags & AF_ISFMEM) ? 2 : 1;
#endif
	isparm = pa->a_flags & AF_ISPARM;
						/* change current dir.	*/
						/*   to selected icon's	*/
	pro_chdir(drv, ppath);
						/* see if application	*/
						/*   was selected 	*/
						/*   directly or a 	*/
						/*   data file with an	*/
						/*   associated primary	*/
						/*   application	*/
	pcmd = ptail = NULLPTR;
	G.g_cmd[0] = G.g_tail[1] = 0;
	ret = TRUE;

	if ( atype == FT_ISINST )
	{
						/* an installed	docum.	*/
	  pcmd = pa->a_pappl;
	  ptail = pname;
	  /* [JCE] If the app is DESKTOP.APP, then use the builtin file 
	   * viewer */
	  if (!strcmp(pcmd, "DESKTOP.APP"))
	  {
		return do_type(curr);
	  }
	}
	else
	{
	  if ( atype == FT_ISAPP )
	  {
						/* DOS-based app. has	*/
						/*   been selected	*/
	    if (isparm)
	    {
	      pcmd = &G.g_cmd[0];
	      ptail = &G.g_tail[1];
	      ret = opn_appl(pname, "\0", pcmd, ptail, sizeof(G.g_tail) - 1);
	    }
	    else
	      pcmd = pname;
	  } /* if FT_ISAPP */
	  else
	  {
						/* DOS-based document 	*/
						/*   has been selected	*/
	    fun_alert(1, STNOAPPL);
	    ret = FALSE;
	  } /* else */
	} /* else */
						/* see if it is a 	*/
						/*   batch file		*/
	if ( wildcmp( ini_str(STGEMBAT), pcmd) )
	{
						/* if is app. then copy	*/
						/*   typed in parameter	*/
						/*   tail, else it was	*/
						/*   a document installed*/
						/*   to run a batch file*/
	  strlcpy(G.g_1text, (atype == FT_ISAPP) ? &G.g_tail[1] : ptail, sizeof(G.g_1text));
	  ptmp = &name[0];
	  pname = pcmd;
	  while ( *pname != '.' )
	    *ptmp++ = *pname++;
	  *ptmp = 0;
	  ret = pro_cmd(&name[0], &G.g_1text[0], TRUE);
	  pcmd = &G.g_cmd[0];
	  ptail = &G.g_tail[1];
	} /* if */
						/* user wants to run	*/
						/*   another application*/
	if (ret)
	{
	  if ( (pcmd  !=  G.g_cmd)     && 
	       (pcmd  !=  NULL)  ) strlcpy(G.g_cmd, pcmd, sizeof(G.g_cmd));
	  if ( (ptail != &G.g_tail[1]) && 
	       (ptail !=  NULL)  ) strlcpy(G.g_tail+1, ptail, sizeof(G.g_tail)-1);
	  done = pro_run(isgraf, isover, G.g_cwin, curr);
	} /* if ret */
#if MULTIAPP
	return(FALSE);				/* don't want to exit	*/
#else
	return(done);
#endif
} /* do_aopen */


/*
*	Open a disk
*/

MLOCAL WORD  do_dopen(WORD curr)
{
	WORD		drv;
	WNODE		*pw;
	LPICON		pib;

	pib = get_spec(G.g_screen, curr);
	pw = win_alloc(curr);
	if (pw)
	{
	  drv = (0x00ff & pib->ib_char);
	  pro_chdir(drv, "");
	  if (!DOS_ERR)
	  {
 	    do_diropen(pw, TRUE, curr, drv, "", "*", "*", 
			(GRECT *)&G.g_screen[pw->w_root].ob_x, TRUE);
	  }
	  else
	    win_free(pw);
	}
	else
	{
	  rsrc_gaddr(R_STRING, STNOWIND, (LPVOID *)&G.a_alert);
	  form_alert(1, G.a_alert);
	}
	return( FALSE );
}


/*
*	Open a folder
*/
/* Extra parameters not in DESKTOP v1.2 */
VOID  do_fopen(WNODE *pw, WORD curr, WORD drv, 
	      BYTE *ppath, BYTE *pname, BYTE *pext, WORD chkall, WORD redraw)
{
	GRECT		t;
	WORD		ok;
	BYTE		*pp, *pnew;

//	dbg("do_fopen: drv=%d ppath=%s pname=%s pext=%s\n", drv, ppath, pname, pext);	
	ok = TRUE;
	pnew = ppath;
	wind_get(pw->w_id, WF_WXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
	pro_chdir(drv, "");
	if (DOS_ERR)
	{
	  //dbg("do_fopen: pro_chdir() failed!\n", drv);
	  true_closewnd(pw);
	/* Elaborate checks not present in DESKTOP v1.2; bring them back
	 * for FreeGEM */
	  if ( DOS_AX == E_PATHNOTFND )
	  {
	    if (!chkall)
	    {
	      fun_alert(1, STDEEPPA);
	      //dbg("do_fopen: STDEEPPA 2\n");
	    }
	    else
	    {
	      pro_chdir(drv, "");
	      pnew = "";
	    }
	  } 
	  else
	    return;			
	} /* if DOS_ERR */
	else
	{
	  pro_chdir(drv, ppath);
	  if (DOS_ERR)
	  {
	    //dbg("do_fopen: DOS error %d!\n", DOS_AX);
	    if ( DOS_AX == E_PATHNOTFND )
	    {				/* DOS path is too long?	*/
	      if (chkall)
	      {
	        pro_chdir(drv, "");
		pnew = "";
	      }
	      else
	      {
	        fun_alert(1, STDEEPPA);
	    					/* back up one level	*/
		pp = ppath + strlen(ppath);
		while(pp[0] != '\\' && pp[0] != '/') pp--;
		*pp = 0;
		pname = "*";
		pext  = "*";
		return;
	      } /* else */
	    } /* if DOS_AX */
	    else
	      return;			/* error opening disk drive	*/
	  } /* if DOS_ERR */
	} /* else */
	pn_close(pw->w_path);
/* Desktop 1 does this, but it screws up the 
 * deep path error handling which is probably why it isn't in 
 * Desktop 3.
 *	if (!DOS_ERR)
	{
		ppath = "";
		pname = "*";
		pext  = "*";
	} */
	/* Again, DESKTOP v1.2 doesn't do all this checking */
	if (ok)
	{
	  ok = do_diropen(pw, FALSE, curr, drv, pnew, pname, pext, &t, redraw);
	  if ( !ok )
	  {
	    fun_alert(1, STDEEPPA);
	    //dbg("do_fopen: Current path: %s\n", ppath);
	    					/* back up one level	*/
	    pp = ppath + strlen(ppath);
	    while(pp[0] != '\\' && pp[0] != '/') pp--;
	    pp[0] = 0;
	    //dbg("do_fopen: Truncated path: %s\n", ppath);
	    do_diropen(pw, FALSE, curr, drv, pnew, pname, pext, &t, redraw);
	  }
	}
} /* do_fopen */


/*
*	Open an icon
*/

WORD  do_open(WORD curr)
{
	WORD		done;
	ANODE		*pa;
	WNODE		*pw;
	FNODE		*pf;
	WORD		drv, atype;
	BYTE		path[66], name[9], ext[4];

	done = FALSE;

	pa = i_find(G.g_cwin, curr, &pf, &atype);
	pw = win_find(G.g_cwin);
	if ( pf )
	  fpd_parse(&pw->w_path->p_spec[0],&drv,&path[0],&name[0],&ext[0]);

	if ( pa )
	{	
	  switch( pa->a_type )
	  {
	    case AT_ISFILE:
#if MULTIAPP
		if (!strcmp("DESKTOP.APP", &pf->f_name[0]))
		  break;
#endif
		done = do_aopen(pa,atype,curr,drv,&path[0],&pf->f_name[0]);
		break;
	    case AT_ISFOLD:
	    /* No "New folder" in DESKTOP v1.2 
		if ( (pf->f_attr & F_FAKE) && pw )
		  fun_mkdir(pw);
		else*/
		{
		  if (path[0] != 0)
		    strlcat(path, "\\", sizeof(path));
		  /* No check in DESKTOP v1.2, but that's no reason to
		   * omit it from the FreeGEM version */
//		  dbg("do_open: path=%s pf->f_name=%s\n", path, pf->f_name);
		  if ( (strlen(&path[0]) + LEN_ZFNAME) >= (LEN_ZPATH-3) )
		  {
		    fun_alert(1, STDEEPPA);
//		    dbg("STDEEPPA 1\n");
		  }
		  else
		  {
		    strlcat(path, pf->f_name, sizeof(path));
		    pw->w_cvrow = 0;		/* reset slider		*/
//		    dbg("do_open: new path=%s\n", path);
		    do_fopen(pw, curr, drv, &path[0], &name[0],
		    	     &ext[0], FALSE, TRUE);
//		    dbg("do_open: do_fopen() returned\n");
		  }
		}
		break;
	    case AT_ISDISK:

/* DESKTOP v1.2 opens a new window here
		drv = (0x00ff & pa->a_letter);
		path[0] = 0;
		name[0] = ext[0] = '*';
		name[1] = ext[1] = 0;
		do_fopen(pw, curr, drv, &path[0], &name[0], &ext[0], FALSE, TRUE);*/
		do_dopen(curr);
		break;
		// DESKTOP v1.2: Trash
		case AT_ISTRSH:
		form_alert(1, ini_str(STNOOPEN));
		break;
	  }
	}

	return(done);
}



/*
*	Get information on an icon.
*/

WORD  do_info(WORD curr)
{
	WORD		ret, junk;
	ANODE		*pa;
	WNODE		*pw;
	FNODE		*pf;
	LPTREE		tree;

	pa = i_find(G.g_cwin, curr, &pf, &junk);
	pw = win_find(G.g_cwin);

	if ( pa )
	{	
	  switch( pa->a_type )
	  {
	    case AT_ISFILE:
		ret = inf_file(&pw->w_path->p_spec[0], pf);
		if (ret)
		  fun_rebld(pw);
		break;
	    case AT_ISFOLD:
/* No fake folders in DESKTOP v1.2
		if (pf->f_attr & F_FAKE)
		{
		  tree = G.a_trees[ADTRINFO];
		  inf_show(tree, 0);
		  tree[TRINFOK].ob_state = NORMAL;
		}
		else */
		  inf_folder(&pw->w_path->p_spec[0], pf);
		break;
	    case AT_ISDISK:
		// DESKTOP v2.x+ version inf_disk( pf->f_junk );
		junk = (get_spec(G.g_screen, curr)->ib_char) & 0xFF;
		inf_disk(junk);
		break;
		// Trashcan in DESKTOP v1.2
		case AT_ISTRSH:
		tree = G.a_trees[ADTRINFO];
		inf_show(tree, 0);
		tree[TRINFOK].ob_state = NORMAL;
		break;
	  }
	}
	return( FALSE );
}


/*
*	Routine to check the all windows directory by doing a change
*	disk/directory to it and redrawing the window;
*/
	VOID 
do_chkall(redraw)
	WORD		redraw;
{
	WORD		ii;
	WORD		drv;
	BYTE		path[66], name[9], ext[4];
	WNODE		*pw;
	for(ii = 0; ii < NUM_WNODES; ii++)
	{
	  pw = &G.g_wlist[ii];
	  if (pw->w_id)
	  {
	    fpd_parse(&pw->w_path->p_spec[0], &drv, &path[0],
	    	      &name[0], &ext[0]);
	    do_fopen(pw, 0, drv, &path[0], &name[0], &ext[0], TRUE, redraw);
	  }
	  else
	  {
	    desk_verify(0, TRUE);

	  }
	}
} /* do_chkall */

WORD  alert_s(WORD defbut, WORD alert_num, BYTE *s)
{
	char tmp[256];
	WORD ret;
	LPBYTE found_string;

	rsrc_gaddr(R_STRING, alert_num, (LPVOID *)&found_string);
	lstlcpy(tmp, found_string, sizeof(tmp));
	sprintf(G.g_2text, tmp, s);
	ret = form_alert(defbut, G.g_2text);
	return ret;
}


WORD  menu_item_to_alert_s( WORD def_but, WORD alert_num, WORD item )
{
OBJECT far	*ptrItem;
char *		ptr ;

    /* get a local copy of the menu item text */
    ptrItem = &G.a_trees[ADMENU][item] ;
    lstlcpy( (char far *)G.g_1text, ptrItem->ob_spec , sizeof(G.g_1text));

    /* remove leading spaces, strip out underbars and ellipses */
    ptr = G.g_1text + strspn( G.g_1text, " " ) ;
    memmove( (void *)G.g_1text, (void *)ptr, strlen( G.g_1text ) ) ;
    
    ptr = strchr( G.g_1text, '_' ) ;
    if ( ptr )
	memmove( (void *)ptr, (void *)(ptr+1), strlen( G.g_1text ) ) ;
	
    ptr = strchr( G.g_1text, '.' ) ;
    if ( ptr )
	*ptr = '\0' ;
    
    return( alert_s( def_but, alert_num, G.g_1text ) ) ;
    
} /* menu_item_to_alert_s() */

