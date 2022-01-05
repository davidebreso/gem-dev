

/*	DESKTOP.C	05/04/84 - 09/05/85	Lee Lorenzen		*/
/*	for 3.0		3/12/86  - 1/29/87	MDF			*/
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

#define INSTANTIATE
#include "wccdesk.h"

/* For tracing a problem in SD256: Set the SD256 debugging flag 
#define TRACE_ON   { LPBYTE fp = MK_FP(0, 0x501); *fp = 1; }
#define TRACE_OFF  { LPBYTE fp = MK_FP(0, 0x501); *fp = 0; }
*/

/* DESKTOP v1.2: Different sets of illegal items */
#if MULTIAPP
  GLOBAL BYTE	ILL_ITEM[] = {L2ITEM,L3ITEM,L4ITEM,L5ITEM, 0};
#else
  GLOBAL BYTE	ILL_ITEM[] = {L2ITEM,L3ITEM,L4ITEM,L5ITEM,IACCITEM, 0};
#endif
GLOBAL BYTE	ILL_FILE[] = {FORMITEM,IDSKITEM,0};
GLOBAL BYTE	ILL_DOCU[] = {FORMITEM,IDSKITEM,IAPPITEM,0};
GLOBAL BYTE	ILL_FOLD[] = {OUTPITEM,FORMITEM,TYPITEM,IDSKITEM,IAPPITEM,0};
GLOBAL BYTE	ILL_FDSK[] = {OUTPITEM,IAPPITEM,TYPITEM,0};
GLOBAL BYTE	ILL_HDSK[] = {FORMITEM,OUTPITEM,TYPITEM,IAPPITEM,0};
GLOBAL BYTE	ILL_TRASH[] = {OPENITEM,OUTPITEM,TYPITEM,FORMITEM,
	                       IDSKITEM,IAPPITEM,0};
GLOBAL BYTE ILL_NOTOP[] = {NFOLITEM,CLOSITEM,CLSWITEM,0};
GLOBAL BYTE	ILL_DESKTOP[] = {NFOLITEM,CLOSITEM,CLSWITEM,ICONITEM,TEXTITEM,
						NAMEITEM,DATEITEM,SIZEITEM,TYPEITEM,0};
GLOBAL BYTE 	ILL_NOSEL[]={OPENITEM,SHOWITEM,FORMITEM,
				IDSKITEM,IAPPITEM,TYPITEM,0};
GLOBAL BYTE	ILL_YSEL[] = {OPENITEM, IDSKITEM, FORMITEM, 
				TYPITEM, SHOWITEM, 0};
GLOBAL BYTE	ILL_TYPE[] = {OPENITEM,SHOWITEM,FORMITEM,TYPITEM,
				IAPPITEM, IDSKITEM,
				NAMEITEM, TYPEITEM, SIZEITEM, DATEITEM,
				0 };

/* DESKTOP v1.2 has the easter egg */
GLOBAL WORD	freq[]=
{
	262, 349, 329, 293, 349, 392, 440, 392, 349, 329, 262, 293,
	349, 262, 262, 293, 330, 349, 465, 440, 392, 349, 698
};

GLOBAL WORD	dura[]=
{
	4, 12, 4, 12, 4, 6, 2, 4, 4, 12, 4, 4, 
	4, 4, 4, 4, 4, 4, 4, 12, 4, 8, 4
};


/* 
GLOBAL WORD	gl_swtblks[3] =
{
	160,
	160,
	160
};
 */

/* Forward declarations */
MLOCAL VOID  cnx_put(VOID);

MLOCAL VOID copy_icon(LPTREE dst_tree, LPTREE tree, WORD dst_icon, WORD icon)
{
	dst_tree[dst_icon].ob_spec = tree[icon].ob_spec;
}

#if 0 // No such thing in DESKTOP v1.2
MLOCAL VOID  fix_wins()
{
/* this routine is supposed to keep track of the windows between	*/
/* runs of the Desktop. it assumes pws has already been set up;		*/
/* gl_savewin is set up by app_start.					*/
	WSAVE		*pws0, *pws1;

	gl_open1st = 0;				/* default case		*/
	pws0 = &G.g_cnxsave.win_save[0];
	pws1 = &G.g_cnxsave.win_save[1];
						/* upper window is full	*/
	if ( (gl_savewin[0].g_y != gl_savewin[1].g_y) &&
	     (gl_savewin[0].g_h != gl_savewin[1].g_h) )
	{
					/* which window is upper/lower?	*/
	  if (gl_savewin[0].g_h > gl_savewin[1].g_h)
	  {				/* [0] is (full) upper window	*/
	    gl_open1st = 1;
	    gl_idsiztop = 0;
	    pws0->y_save = G.g_yfull;
	    pws0->h_save = G.g_hfull;
	    pws1->y_save = gl_normwin.g_y + gl_normwin.g_h + (gl_hbox / 2);
	    pws1->h_save = gl_normwin.g_h;
	  }
	  else
	  {				/* [1] is (full) upper window	*/
	    gl_open1st = 0;
	    gl_idsiztop = 1;
	    pws1->y_save = G.g_yfull;
	    pws1->h_save = G.g_hfull;
	    pws0->y_save = gl_normwin.g_y + gl_normwin.g_h + (gl_hbox / 2);
	    pws0->h_save = gl_normwin.g_h;
	  } /* else */
	} /* if */
	  				/* case 3: lower window is full	*/
	else if ( (gl_savewin[0].g_y == gl_savewin[1].g_y) &&
		  (gl_savewin[0].g_h != gl_savewin[1].g_h) )
	{
					/* which window is upper/lower?	*/
	  if (gl_savewin[0].g_h > gl_savewin[1].g_h)
	  {				/* [0] is (full) lower window	*/
	    gl_open1st = 1;
	    gl_idsiztop = 0;
	    pws0->y_save = G.g_yfull;
	    pws0->h_save = G.g_hfull;
	    pws1->y_save = gl_normwin.g_y;
	    pws1->h_save = gl_normwin.g_h;
	  }
	  else
	  {				/* [1] is (full) lower window	*/
	    gl_open1st = 0;
	    gl_idsiztop = 1;
	    pws1->y_save = G.g_yfull;
	    pws1->h_save = G.g_hfull;
	    pws0->y_save = gl_normwin.g_y;
	    pws0->h_save = gl_normwin.g_h;
	  } /* else */
	} /* if */
} /* fix_wins */
#endif // 0


/*
*	Turn on the hour glass to signify a wait and turn it off when were
*	done.
*/
MLOCAL VOID  desk_wait(WORD turnon)
{
	graf_mouse( (turnon) ? HGLASS : ARROW, 0x0L);
}


/*
*	Routine to update all of the desktop windows
*/
MLOCAL VOID  desk_all(WORD sort)
{
	desk_wait(TRUE);
	if (sort) win_srtall();
	win_bdall();
	win_shwall();
	desk_wait(FALSE);
}

/*
*	Given an icon index, go find the ANODE which it represents
*/
ANODE *  i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp)
{
	ANODE 	*pa;
	BYTE 	*pname;
	WNODE	*pw;
	FNODE	*pf;

	pa = (ANODE *) NULL;
	pf = (FNODE *) NULL;

	if (!wh)	// DESKTOP v1.2: On desktop?
	{
		pname = (BYTE *)(WORD)(win_iname(item));
		pa = app_afind(TRUE, -1, item, pname, pisapp);
	}
	else
	{
		pw = win_find(wh);
		pf = fpd_ofind(pw->w_path->p_flist, item);
		if (pf)
	 	{
		  pname = &pf->f_name[0];
		  // DESKTOP v1.2: Completely different check here
		  if (pf->f_attr & F_SUBDIR) 
		  {
			pa = app_afind(FALSE, 1, -1, pname, pisapp);
		  }
		  else
		  {
		 	pa = app_afind(FALSE, 0, -1, pname, pisapp);
		  }

		  /* 
		  pa = pf->f_pa;
		  if (pf->f_attr & (F_SUBDIR | F_DESKTOP))
	  	  	*pisapp = FALSE;
	  	  else 
	    	*pisapp = wildcmp(pa->a_pappl, pname);
	    	*/
	 	}
	}
	*ppf = pf;
	return (pa);
}

/*
*	Enable/Disable the menu items in dlist
*/
VOID  men_list(LPTREE mlist, BYTE *dlist, WORD enable)
{
	while (*dlist)
	  menu_ienable(mlist, *dlist++, enable);
}

/*
* 	Based on current selected icons, figure out which
*	menu items should be selected (deselected)
*/
MLOCAL VOID  men_update(LPTREE tree)
{
	WORD		item, nsel, isapp;
	BYTE		*pvalue;
	ANODE		*appl;
	FNODE		*pjunk;
						/* enable all items	*/
	for (item = OPENITEM; item <= PREFITEM; item++)
	  menu_ienable(tree, item, TRUE);
//  Not in DESKTOP v1.2
	can_iapp = TRUE;
	can_show = TRUE;
	can_del = TRUE;
	can_output = TRUE;
	  					/* disable some items	*/
	men_list(tree, ILL_ITEM, FALSE);

	nsel = 0;
	for (item = 0; (item = win_isel(G.g_screen, G.g_croot, item)) != 0;
	     nsel++)
	{
	  appl = i_find(G.g_cwin, item, &pjunk, &isapp);
	  switch (appl->a_type)
	  {
	    case AT_ISFILE:
		if ( (isapp) || is_installed(appl) )
		  pvalue = ILL_FILE;
		else
		{
		  pvalue = ILL_DOCU;
		  can_iapp = FALSE;
		} 
		break;
	    case AT_ISFOLD:
		pvalue = ILL_FOLD;
		can_iapp = FALSE;
		can_output = FALSE;
		break;
	    case AT_ISDISK:
		pvalue = (appl->a_aicon == IG_FLOPPY) ? ILL_FDSK : ILL_HDSK;
		can_iapp = FALSE;
		can_output = FALSE;
		break;
		case AT_ISTRSH:	/* DESKTOP v1.2 */
		pvalue = ILL_TRASH;
		can_iapp = FALSE;
		can_output = FALSE; 
	  } /* switch */
	  men_list(tree, pvalue, FALSE);       /* disable certain items	*/
	} /* for */

	if (win_ontop()) pvalue = ILL_DESKTOP;	/* DESKTOP v1.2 */
	else			 pvalue = ILL_NOTOP;
	if (pvalue == ILL_DESKTOP) men_list(tree, pvalue, TRUE);
	else					   men_list(tree, pvalue, FALSE);

	if ( nsel != 1 )
	{
	  if (nsel)
	  {
	    pvalue = ILL_YSEL;
	    can_show = FALSE;
	  }
	  else
	  {
	    pvalue = ILL_NOSEL;
	    can_show = FALSE;
	    can_del = FALSE;
	    can_iapp = FALSE;
	  }
	  men_list(tree, pvalue, FALSE);
	} /* if */
	/* If showing contents of one file, can't repeat for another */
	if( in_type ) menu_ienable(tree, TYPITEM, FALSE);

} /* men_update */

MLOCAL WORD  do_deskmenu(WORD item)
{
	WORD		done, touchob;
	WORD		i;	/* DESKTOP v1.2 */
	LPTREE		tree;

	done = FALSE;
	switch( item )
	{
	  case ABOUITEM:
		tree = G.a_trees[ADDINFO];
						/* draw the form	*/
		show_hide(FMD_START, tree);
		while( !done )
		{
	          touchob = form_do(tree, 0);
	          touchob &= 0x7fff;
		  if ( touchob == DEICON )
		  {
/* DESKTOP v1.2: Easter egg is in */
		    for(i=0; i<23; i++)
		      desk_sound(TRUE, freq[i], dura[i]);
		  }
		  else
		    done = TRUE;
		}  
		tree[DEOK].ob_state &= ~SELECTED;//	LWSET(OB_STATE(DEOK), NORMAL);
		show_hide(FMD_FINISH, tree);
		done = FALSE;
		break;
	}
	return(done);
}


MLOCAL WORD  do_filemenu(WORD item)
{
	WORD		done;
	WORD		curr, savwin, junk, first;
	WNODE		*pw;
	FNODE		*pf;

#if MULTIAPP
	ANODE		*pa;
#endif
	
	done = FALSE;
	pw = win_ontop();	/* DESKTOP v1.2  win_find(G.g_cwin);*/

	curr = win_isel(G.g_screen, G.g_croot, 0);
	switch( item )
	{
	  case OPENITEM:
		if (curr)
		  done = do_open(curr);
		break;
	  case SHOWITEM:
		if (curr)
		  do_info(curr);
		break;
/* case FINDITEM */
	  case TYPITEM:
/* 'Show contents' - copied from ViewMAX */
		if (curr)
		{
			in_type = TRUE;
			men_list(G.a_trees[ADMENU], ILL_TYPE, FALSE);
/*			enable_cursor(); */
			savwin = G.g_cwin;
			done = do_type(curr);
			if (done != FALSE && done != TRUE)
			{
				done = FALSE;
			}
			G.g_cwin = savwin;
/*			disable_cursor(); */
			men_list(G.a_trees[ADMENU], ILL_TYPE, TRUE);
			in_type = FALSE;
		}
		win_top(pw);
/*		win_vistop(pw->w_id, FALSE);
 *		cursor_init();  */
		do_chkall( FALSE );
		break;
	  case NFOLITEM:	/* DESKTOP v1.2 No delete, but new folder */
		if (pw)			/* instead */
		  fun_mkdir(pw);/* [JCE 19-7-2002] Do correct check: for window */
		break;			/* open rather than item highlighted */
	/* These next two only in DESKTOP v1.2 */
          case CLOSITEM:
		if (pw)
		  fun_close(pw, 0);
		break;
	  case CLSWITEM:
		if (pw)
		  fun_close(pw, 1);
		break;
	  case FORMITEM:
		if (curr)
		  do_format(curr);
		break;   
	  case OUTPITEM:

			/* build cmd tail that looks like this:		*/
			/* C:\path\*.*,file.ext,file.ext,file.ext	*/
		G.g_tail[1] = 0;
		if (pw && curr)
		{
		  strlcpy(G.g_tail + 1, pw->w_path->p_spec, 127);
		  if (pw->w_path->p_spec[0] == '@')
		    G.g_tail[1] = gl_defdrv;
						/* while there are	*/
						/*   filenames append	*/
						/*   them in		*/
		  first = TRUE;
		  while(curr)
		  {
		    if (first)
		    {
		      BYTE *pds1 = strrchr(G.g_tail + 1, '\\');
		      BYTE *pds2 = strrchr(G.g_tail + 1, '/');

		      if (pds1 && pds2)
		      {
			if (pds1 > pds2) pds2 = NULL;
			else		 pds1 = NULL;
		      }

		      if (pds1) pds1[1] = 0;
		      if (pds2) pds2[1] = 0;
		      first = FALSE;
		    }
		    else
		    {
		      strlcat(G.g_tail+1, ",", sizeof(G.g_tail)-1);
		    }
		    i_find(G.g_cwin, curr, &pf, &junk);
		    strlcat(G.g_tail+1, pf->f_name, sizeof(G.g_tail)-1);
						/* if there is room for	*/
						/*   another filename &;*/
						/*   then go fetch it	*/ 
		    if ( strlen(G.g_tail) < (127-16))
		      curr = win_isel(G.g_screen, G.g_croot, curr);
		    else
		      curr = 0;
		  } /* while */
		} /* if pw */
		strlcpy(G.g_cmd, ini_str(STGEMOUT), 127);
#if MULTIAPP
		pa = app_afind(FALSE, AT_ISFILE, -1, &G.g_cmd[0], &junk);
		pr_kbytes = (pa ? pa->a_memreq : 256);
		pro_run(TRUE, -1, -1, -1);
#else
		done = pro_run(TRUE, TRUE, -1, -1);
#endif
		break;
	  case QUITITEM:
#if MULTIAPP
		if (fun_alert(1,STEXTDSK,NULLPTR) == 2)		/* CANCEL */
		  break;
		else
#endif
		pro_exit(G.a_cmd, G.a_tail);
		done = TRUE;
		break;
/* #if DEBUG
	  case DBUGITEM:
		debug_run();
		break;
#endif */
	}
	return(done);
} /* do_filemenu */


MLOCAL WORD  do_viewmenu(WORD item)
{
	WORD		newview, newsort;
// not in DESKTOP v1.2	LPBYTE		ptext;

	newview = G.g_iview;
	newsort = G.g_isort;
	switch( item )
	{
	  case ICONITEM:
		newview = V_ICON; //Doesn't toggle in DESKTOP v1.2 (G.g_iview == V_ICON) ? V_TEXT : V_ICON;
		break;
	  case TEXTITEM:
	    newview = V_TEXT;
	    break;
	  case NAMEITEM:
		newsort = S_NAME;
		break;
	  case DATEITEM:
		newsort = S_DATE;
		break;
	  case SIZEITEM:
		newsort = S_SIZE;
		break;
	  case TYPEITEM:
		newsort = S_TYPE;
		break;
	}
	if ( (newview != G.g_iview) ||
	     (newsort != G.g_isort) )
	{
	  if (newview != G.g_iview)
	  {
/* DESKTOP v1.2 shows this choice by having 2 menu entries.
	    G.g_iview = newview;
	    ptext = (newview == V_TEXT) ? ad_picon : ad_ptext;
	    menu_text(G.a_trees[ADMENU], ICONITEM, ptext);
*/
	    menu_icheck(G.a_trees[ADMENU], G.g_viewpref, FALSE);
	    G.g_viewpref = item;
	    menu_icheck(G.a_trees[ADMENU], item, TRUE);
	  }
	  if (newsort != G.g_isort)
	  {
	    menu_icheck(G.a_trees[ADMENU], G.g_csortitem, FALSE);
	    G.g_csortitem = item;
	    menu_icheck(G.a_trees[ADMENU], item, TRUE);
	  }
	  win_view(newview, newsort);
	  return(TRUE);			/* need to rebuild	*/
	}
	return( FALSE );
}



MLOCAL WORD  do_optnmenu(WORD item)
{
	ANODE		*pa;
	WORD		done, rebld, curr, ret;
	FNODE		*pf;
	WORD		isapp;
	BYTE		*pstr;
	GRECT		rect;
#if MULTIAPP
	WORD		junk;
#endif

	done = FALSE;
	rebld = FALSE;

	curr = win_isel(G.g_screen, G.g_croot, 0);
	if (curr)
	  pa = i_find(G.g_cwin, curr, &pf, &isapp);

	switch( item )
	{
	  case IDSKITEM:
		if (pa)		rebld = ins_disk(pa);
		if (rebld)
		{
/* DESKTOP v2.x...
		  gl_defdrv = app_blddesk();
		  do_chkall(TRUE);
*/
/* << DESKTOP v1.2: */
			app_blddesk();
			wind_get(0, WF_WXYWH, &rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
			do_wredraw(0, rect.g_x, rect.g_y, rect.g_w, rect.g_h);
/* >> DESKTOP v1.2 */
	    }
		break;
	  case IAPPITEM:
		if (pa)
		{
		  if (isapp)
		    pstr = &pf->f_name[0];
		  else if (is_installed(pa))
		    pstr = pa->a_pappl;
		  rebld = ins_app(pstr, pa);
		  rebld = TRUE;	/* WORKAROUND for bug that shows up with */
				/* remove followed by cancel when icon is */
				/* partially covered by dialog.  Icon not */
				/* properly redrawn as deselected -- mdf   */
		}
		if (rebld)
		{
		 	// DESKTOP v2.x desk_all(FALSE);
		 	desk_wait(TRUE);
			win_bdall();
		 	win_shwall();
		 	desk_wait(FALSE);
		}
		break;
#if MULTIAPP
	  case IACCITEM:
		ins_acc();
		break;
#endif
	  case PREFITEM:
		if (inf_pref())
		  desk_all(FALSE);
		break;
	  case SAVEITEM:
		desk_wait(TRUE);
		cnx_put();
		app_save(TRUE);
		desk_wait(FALSE);
		break;
	  case DOSITEM:


#if MULTIAPP
		ret = appl_find(ADDR("COMMAND "));
		if (ret == -1)
		{
		  ret = pro_cmd( "\0", "\0", FALSE);
		  if (ret)
		  {
		    pa = app_afind(FALSE, AT_ISFILE, -1,"COMMAND.COM", &junk);
		    pr_kbytes = (pa ? pa->a_memreq : 128);
		    pro_run(FALSE, -1, -1, -1);
		  }
		}
		else
		{
		  menu_tnormal(G.a_trees[ADMENU], OPTNMENU, TRUE);
		  proc_switch(ret);
		}

#else
		ret = pro_cmd( "\0", "\0", FALSE);
		if (ret)
		{
		  done = pro_run(FALSE, TRUE, -1, -1);
		}
#endif
		break;
	}
	return(done);
}


WORD  hndl_button(WORD clicks, 	// bp+1e
				 WORD mx, 	  	// bp+20
				 WORD my, 	  	// bp+22
				 WORD button, 	// bp+24
				 WORD keystate)	// bp+26
{
	WORD		done;			//bp+02
	WORD		junk;			//bp+04
	GRECT		c;				//bp+06
	WORD		wh;				//bp+0e
	WORD		root;			//bp+10
//	WORD		unused;			//bp+12 DESKTOP v1.2 declares but doesn't use this
	WORD		dobj;			//bp+14
	WORD		dest_wh;		//bp+16
	WNODE		*wn;			//bp+18
	
	done = FALSE;

	wh = wind_find(mx, my);

	if (wh != G.g_cwin) desk_clear(G.g_cwin);

/* BUGFIX	not in DESKTOP v1.2
	if (wh == 0)				// if click outside win's
	{
	  men_update(G.a_trees[ADMENU]);
	  wind_update(BEG_UPDATE);
	  while(button & 0x0001)
	    graf_mkstate(&junk, &junk, &button, &junk);
	  wind_update(END_UPDATE);
	  return(done);
	}
// END BUGFIX */

	desk_verify(wh, FALSE);

	wind_get(G.g_wlastsel = wh, WF_WXYWH, &c.g_x, &c.g_y, &c.g_w, &c.g_h);

	if (clicks == 1)
	{
	  act_bsclick(G.g_cwin, G.a_screen, G.g_croot, mx, my,
		      keystate, &c, FALSE);
	  graf_mkstate(&junk, &junk, &button, &junk);
	  if (button & 0x0001)
	  {
	    dest_wh = act_bdown(G.g_cwin, G.a_screen, G.g_croot, &mx, &my, 
				keystate, &c, &dobj);

// DESKTOP v1.2 can do things when dest_wh == 0
//	    if ( (dest_wh != NIL) && (dest_wh != 0) )
		if (dest_wh != NIL)
	    {
		  if (dest_wh == 0)
		  {
			root = 1;
		  }
		  else
		  {
			wn = win_find(dest_wh);
			root = wn->w_root;
		  }
		  desk1_drag(wh, dest_wh, root, dobj, mx, my);
	      // DESKTOP v2.x+ version fun_drag(wh, dest_wh, dobj, mx, my);
	      desk_clear(wh);
		
	    } /* if !NIL */
	  } /* if button */
	} /* if clicks */
	else
	{
	  act_bsclick(G.g_cwin, G.a_screen, G.g_croot, mx, my, keystate, &c, TRUE);
	  done = do_filemenu(OPENITEM);
	} /* else */
	men_update(G.a_trees[ADMENU]);
	return(done);
}

	WORD 
hndl_kbd(thechar)
	WORD		thechar;
{
	WORD		done;

	done = FALSE;

	switch(thechar)
	{
	  case ESCAPE:
	  	do_chkall(TRUE);
		break;
/* Bring in the keyboard shortcuts which weren't in 1.2 */
	  case ALTA:	/* Options: Install App	*/
	  	if (can_iapp)		/* if it's ok to install app	*/
		{
		  menu_tnormal(G.a_trees[ADMENU], OPTNMENU, FALSE);
	  	  done = hndl_menu(OPTNMENU, IAPPITEM);
		}
		break;
	  case ALTC:	/* Options: Enter DOS commands	*/
		menu_tnormal(G.a_trees[ADMENU], OPTNMENU, FALSE);
	  	done = hndl_menu(OPTNMENU, DOSITEM);
		break;
#if 0	/* No delete hotkey */
	  case ALTD:	/* File: Delete		*/
	  	if (can_del)		/* if it's ok to delete		*/
		{
	  	  menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
	  	  done = hndl_menu(FILEMENU, DELTITEM);
		}
		break;
#endif
	  case ALTI:	/* File: Info/Rename	*/
	  	if (can_show)		/* if it's ok to show		*/
		{
	  	  menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
	  	  done = hndl_menu(FILEMENU, SHOWITEM);
		}
		break;
	  case ALTN:	/* View: Sort by Name	*/
	  	menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
	  	done = hndl_menu(VIEWMENU, NAMEITEM);
		break;
	  case ALTP:	/* View: Sort by Type	*/
	  	menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
	  	done = hndl_menu(VIEWMENU, TYPEITEM);
		break;
	  case ALTS:	/* View: Show as Text/Icons	*/
	  	menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
	  	done = hndl_menu(VIEWMENU, ICONITEM);
		break;
	  case ALTT:	/* View: Sort by Date	*/
	  	menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
	  	done = hndl_menu(VIEWMENU, DATEITEM);
		break;
	  case ALTV:	/* Options: Save Desktop	*/
	  	menu_tnormal(G.a_trees[ADMENU], OPTNMENU, FALSE);
	  	done = hndl_menu(OPTNMENU, SAVEITEM);
		break;
	  case ALTX:	/* View: Show as Text		*/
	  	menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
	  	done = hndl_menu(VIEWMENU, TEXTITEM);
		break;
	  case ALTZ:	/* View: Sort by Size	*/
	  	menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
	  	done = hndl_menu(VIEWMENU, SIZEITEM);
		break;
	  case CNTLU:
		if (can_output)
	  	{
		  menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
	  	  done = hndl_menu(FILEMENU, OUTPITEM);
		}
		break;
	  case CNTLQ:
	  	menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
	  	done = hndl_menu(FILEMENU, QUITITEM);
		break;
	  case CNTLW:	/* [JCE] Add ^W for 'close window' */
		if (win_ontop())
		{
		  menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
		  done = hndl_menu(FILEMENU, CLSWITEM);
		}
		break;
	} /* switch */

	men_update(G.a_trees[ADMENU]);		/* clean up menu info	*/
	return(done);
} /* hndl_kbd */



WORD hndl_menu(WORD title, WORD item)
{
	WORD		done;

	done = FALSE;
	switch( title )
	{
	  case DESKMENU:
		done = do_deskmenu(item);
		break;
	  case FILEMENU:
		done = do_filemenu(item);
		break;
	  case VIEWMENU:
		done = FALSE;
						/* for every window	*/
						/*   go sort again and	*/
						/*   rebuild views	*/
		if (do_viewmenu(item))
			desk_all(TRUE);
		break;
	  case OPTNMENU:
		done = do_optnmenu(item);
		break;
	}
	menu_tnormal(G.a_trees[ADMENU], title, TRUE);
	return(done);
}

VOID  wind_setl(WORD hw, WORD var, LPVOID data)
{
	wind_set(hw, var, FP_OFF(data), FP_SEG(data), 0, 0);
}

#if 0 /* Not present in DESKTOP v1.2 */
MLOCAL 	VOID
hot_close(wh)
	WORD		wh;
{
/* "close" the path but don't set a new dir. & don't redraw the window	*/
/* until the button is up or there are no more CLOSED messages		*/
	WORD		drv, done, at_drvs, cnt;
	WORD		mx, my, button, kstate;
	BYTE		path[66], name[9], ext[4];
	BYTE		*pend, *pname, *ppath;
	WNODE		*pw;

	pw = win_find(wh);
	at_drvs = FALSE;
	done = FALSE;
	cnt = 0;
/*	wind_update(BEG_UPDATE);*/
	do
	{
	  cnt++;
	  ppath = &pw->w_path->p_spec[0];
	  pname = &pw->w_name[0];
	  fpd_parse(ppath, &drv, &path[0], &name[0], &ext[0]);
	  if (!path[0])	/* No more path; show disk icons */
	  {
	    strlcpy(pname, ini_str(STDSKDRV), LEN_ZPATH);
	    ppath[3] = '*';
	    ppath[4] = '.';
	    ppath[5] = '*';
	    ppath[6] = 0;
	    at_drvs = TRUE;
	  }
	  else
	  {			/* Some path left; scan off last dir.	*/
	    pend = pname;
	    pend += (strlen(pname) - 3);	/* skip trailing '\ '	*/
						/* Remove last name	*/
	    while ( (pend != pname) && (*pend != '\\') )
	      pend--;
	    pend++;			  	/* save trailing '\'	*/
/*	    *pend++ = SPACE;*/
	    *pend = 0;
	    					/* now fix path		*/
	    pend = ppath;
	    pend += (strlen(ppath) - 5);	/* skip trailing '\*.*'	*/
						/* Remove last name	*/
	    while ( (pend != ppath) && (*pend != '\\') )
	      pend--;
	    pend++;				/* save trailing '\'	*/
	    *pend = 0;
	    strcat(ppath, "*.*");		/* put '*.*' back on	*/
	  } /* else */
	  wind_setl(pw->w_id, WF_NAME, ADDR(&pw->w_name[0]));
/*	  wind_update(END_UPDATE);*/
	  graf_mkstate(&mx, &my, &button, &kstate);
	  if (button == 0x0)
	    done = TRUE;
	  else
	  {
	    evnt_timer(750, 0);
						/* grab the screen	*/
/*	    wind_update(BEG_UPDATE);*/
	    graf_mkstate(&mx, &my, &button, &kstate);
	    if (button == 0x0)
	      done = TRUE;
	  } /* else */
	} while ( !done );
/*	wind_update(END_UPDATE);*/
	if (cnt > 1)
	  ig_close = TRUE;
/*	desk_verify(G.g_rmsg[3], FALSE);*/
	fpd_parse(ppath, &drv, &path[0], &name[0], &ext[0]);
	if (at_drvs)
	  drv = '@';
	pw->w_cvrow = 0;			/* reset slider		*/
	do_fopen(pw, 0, drv, path, name, ext, FALSE, TRUE);
	cnx_put();
} /* hot_close */

#endif

WORD  hndl_msg()
{
	WORD		x,y,w,h;
	WORD		done;
	WNODE		*pw;
	WORD		change, menu;
	WORD		cols, shrunk;

	done = change = menu = shrunk = FALSE;
	if ( G.g_rmsg[0] == WM_CLOSED && ig_close )
	{
	  ig_close = FALSE;
	  return(done);
	}
	switch( G.g_rmsg[0] )
	{
	  case WM_TOPPED:
	  case WM_CLOSED:
	  case WM_FULLED:
	  case WM_ARROWED:
	  case WM_HSLID: // reinstated in DESKTOP v1.2
	  case WM_VSLID:
	  case WM_SIZED: // reinstated in DESKTOP v1.2
	  case WM_MOVED: // reinstated in DESKTOP v1.2
		desk_clear(G.g_cwin);
		break;
	}
	switch( G.g_rmsg[0] )
	{
	  case MN_SELECTED:
		desk_verify(G.g_wlastsel, FALSE);
		done = hndl_menu(G.g_rmsg[3], G.g_rmsg[4]);
		break;
	  case WM_REDRAW:
		menu = TRUE;
#if MULTIAPP
		if (gl_untop)
		{
		  gl_untop = FALSE;
		  do_chkall(FALSE);
	          men_update(G.a_trees[ADMENU]); 	/* disable some items */
		}
#endif
		if (G.g_rmsg[3])
		{
		  do_wredraw(G.g_rmsg[3], G.g_rmsg[4], G.g_rmsg[5], 
					G.g_rmsg[6], G.g_rmsg[7]);
		}
		break;
	  case WM_TOPPED:
		wind_set(G.g_rmsg[3], WF_TOP, 0, 0, 0, 0);
		// DESKTOP v1.2 version
		wind_get(G.g_rmsg[3], WF_WXYWH, &x, &y, &w, &h);
		
		pw = win_find(G.g_rmsg[3]);
		//if (pw) Check omitted in DESKTOP v1.2
		//{
		  win_top(pw);
		  desk_verify(pw->w_id, FALSE);
		//}
		change = TRUE;
		G.g_wlastsel = pw->w_id;
		break;
#if MULTIAPP
	  case WM_ONTOP:
		gl_untop = TRUE;
		break;
	  case PR_FINISH:
		gl_fmemflg &= (0xff ^ (1 << G.g_rmsg[3]));
		break;
#endif
	  case WM_CLOSED:
	//	hot_close(G.g_rmsg[3]);	// DESKTOP v1.2 change
		do_filemenu(CLOSITEM);
		break;
	  case WM_FULLED:
/* DESKTOP v1.2 doesn't bother with these checks...
		pw = win_find(G.g_rmsg[3]);
		if (pw)
		{
		  win_top(pw);*/
		  do_wfull(G.g_rmsg[3]);
		  desk_verify(G.g_rmsg[3], TRUE);
/*		} */

		change = TRUE;
		break;
	  case WM_ARROWED:
		win_arrow(G.g_rmsg[3], G.g_rmsg[4]);
		break;
	  case WM_HSLID:	// Reinstated in DESKTOP v1.2
	  	win_slide(G.g_rmsg[3], G.g_rmsg[4], FALSE);
	  	break;
	  case WM_VSLID:
		win_slide(G.g_rmsg[3], G.g_rmsg[4], TRUE);
		break;
	  case WM_MOVED:
	  case WM_SIZED:	// Reinstated in DESKTOP v1.2
		x = G.g_rmsg[4];
		y = G.g_rmsg[5];
		do_xyfix(&x, &y);
		wind_set(G.g_rmsg[3], WF_CXYWH, x, y, G.g_rmsg[6], G.g_rmsg[7]);
		if (G.g_rmsg[0] == WM_SIZED)
		{
			cols = pw->w_pncol;
			wind_get(G.g_rmsg[3], WF_PXYWH, &x, &y, &w, &h);
			if ((G.g_rmsg[6] <= w) && (G.g_rmsg[7] <= h))
			    shrunk = TRUE;
			desk_verify(G.g_rmsg[3], TRUE);	 /* Build window, update w_pncol*/
		}
		else
		{
			wind_get(G.g_rmsg[3],WF_WXYWH, &x, &y, &w, &h);
			pw = win_find(G.g_rmsg[3]);
			r_set((GRECT *)(&G.g_screen[pw->w_root].ob_x), x, y, w, h);
		}
		change = TRUE;
		break;
	}

	/*
	 * if our window has shrunk AND we're displaying a different number
	 * of columns, we need to send a redraw message because the AES won't
	 */
	if (shrunk) // && (pw->w_pncol != cols))
	{
		wind_get(G.g_rmsg[3], WF_WXYWH, &x, &y, &w, &h);
		fun_msg(WM_REDRAW, G.g_rmsg[3], x, y, w, h);
	}

	if (change)
	  cnx_put();
	G.g_rmsg[0] = 0;
	if (!menu)
	  men_update(G.a_trees[ADMENU]);
	return(done);
} /* hndl_msg */

MLOCAL VOID  cnx_put()
{
	WORD		iwin;
	WORD		iwsave;
	WSAVE		*pws;
	WNODE		*pw;

	G.g_cnxsave.vitem_save = (G.g_iview == V_ICON) ? 0 : 1;
	G.g_cnxsave.sitem_save = G.g_csortitem - NAMEITEM;
	G.g_cnxsave.ccopy_save = G.g_ccopypref;
	G.g_cnxsave.cdele_save = G.g_cdelepref;
	G.g_cnxsave.covwr_save = G.g_covwrpref;	// Commented out bits not in DESKTOP v1.2
	G.g_cnxsave.cdclk_save = G.g_cdclkpref;
	G.g_cnxsave.cmclk_save = G.g_cmclkpref;
	G.g_cnxsave.ctmfm_save = G.g_ctimeform;
	G.g_cnxsave.cdtfm_save = G.g_cdateform;
	/* Copied these from BALJ's Desktop */
	G.g_cnxsave.cdetd_save = G.g_detdrives;
	G.g_cnxsave.cdetn_save = G.g_probedrives;

/* DESKTOP v1.2 version of this function doesn't assume 2 fixed windows */
	for (iwsave=0, iwin = 0; iwin < NUM_WNODES; iwin++)
	{
		pw = win_ith(iwin + 1);
		if (!pw->w_id) continue;

		pws = &G.g_cnxsave.win_save[iwsave++];
		wind_get(pw->w_id, WF_CXYWH, &pws->x_save, &pws->y_save,
		   &pws->w_save, &pws->h_save);
		do_xyfix(&pws->x_save, &pws->y_save);
		pws->hsl_save  = pw->w_cvcol;
		pws->vsl_save  = pw->w_cvrow;
		pws->obid_save = pw->w_obid;
	  	strlcpy(pws->pth_save, pw->w_path->p_spec, LEN_ZPATH);
	}
	while (iwsave < NUM_WNODES)
	{
		pws = &G.g_cnxsave.win_save[iwsave++];
		pws->obid_save = 0;
		pws->pth_save[0] = 0;
	}
	

/* DESKTOP v2.x version
	for (iwin = 0; iwin < NUM_WNODES; iwin++) 
	{
	  pw = win_find(G.g_wlist[iwin].w_id);
	  pws = &G.g_cnxsave.win_save[iwin];
	  wind_get(pw->w_id, WF_CXYWH, &pws->x_save, &pws->y_save,
		   &pws->w_save, &pws->h_save);
	  do_xyfix(&pws->x_save, &pws->y_save);
	  pws->hsl_save = pw->w_cvcol;
	  pws->vsl_save = pw->w_cvrow;
	  pws->obid_save = 0;
	  strlcpy(pws->pth_save, pw->w_path->p_spec, LEN_ZPATH);
	} // for 
	*/


	
} /* cnx_put */
#if 0 // Not present in DESKTOP v1.2

/************************************************************************/
/* c n x _ o p e n							*/
/************************************************************************/
MLOCAL 	VOID 
cnx_open(idx)
	WORD		idx;
{
	WSAVE		*pws;
	WNODE		*pw;
	WORD		drv;
	BYTE		pname[9];
	BYTE		pext[4];

	pws = &G.g_cnxsave.win_save[idx];
	pw = win_alloc();
	if (pw)
	{
	  if (idx == gl_idsiztop)		/* if a window is fulled*/
	    gl_whsiztop = pw->w_id;		/*  save the wh		*/
	  pw->w_cvcol = pws->hsl_save;
	  pw->w_cvrow = pws->vsl_save;
	  fpd_parse(&pws->pth_save[0], &drv, &G.g_tmppth[0], 
		    &pname[0], &pext[0]);
	  if (!pname[0]) drv = 0;
	  do_xyfix(&pws->x_save, &pws->y_save);
	  pro_chdir(drv, &G.g_tmppth[0]);
	  if (DOS_ERR)
	  {
	    drv = '@';
	    G.g_tmppth[0] = 0;
	    pname[0] = pext[0] = '*';
	    pname[1] = pext[1] = 0;
	  } /* if */
	  do_diropen(pw, TRUE, pws->obid_save, drv, &G.g_tmppth[0], 
		     &pname[0], &pext[0], (GRECT *)&pws->x_save, TRUE);
	} /* if pw */
} /* cnx_open */

#endif



MLOCAL VOID  cnx_get(VOID)
{
	// DESKTOP v1.2: This function is a lot more involved
	// because CNX_OPEN is no longer a separate function.
	WORD drv;
	WORD nw;
	WSAVE *pws;
	WNODE *pw;
	BYTE fname[9];
	BYTE fext[4];
		
// DESKTOP v1.2 does not set g_iview here
//	G.g_iview = (G.g_cnxsave.vitem_save == 0) ? V_ICON : V_TEXT;
	do_viewmenu(ICONITEM + G.g_cnxsave.vitem_save);
	do_viewmenu(NAMEITEM + G.g_cnxsave.sitem_save);
	G.g_ccopypref = G.g_cnxsave.ccopy_save;
	G.g_cdelepref = G.g_cnxsave.cdele_save;
/* Bring back the bits that weren't in 1.2 */
	G.g_covwrpref = G.g_cnxsave.covwr_save;
	G.g_cdclkpref = G.g_cnxsave.cdclk_save;
	G.g_cmclkpref = G.g_cnxsave.cmclk_save;	// Commented out bits not in DESKTOP v1.2
	G.g_ctimeform = G.g_cnxsave.ctmfm_save;
	G.g_cdateform = G.g_cnxsave.cdtfm_save;
	G.g_cdclkpref = evnt_dclick(G.g_cdclkpref, TRUE);
	if (global[0] < MIN_MENU_CLICK) G.g_cmclkpref = 0;
	else G.g_cmclkpref = menu_click(G.g_cmclkpref, TRUE);
	G.g_detdrives = G.g_cnxsave.cdetd_save;
	G.g_probedrives = G.g_cnxsave.cdetn_save;

/* DESKTOP v1.2: Remove 2-window limit; and cnx_open() inlined.
	cnx_open(gl_open1st);
	cnx_open( abs(1 - gl_open1st) );
	*/
	for (nw = 0; nw < NUM_WNODES; nw++)
	{
		pws = &G.g_cnxsave.win_save[nw];
		if (pws->pth_save[0])
		{
			if ((pw = win_alloc(pws->obid_save)))
			{
				pw->w_cvcol = pws->hsl_save;
				pw->w_cvrow = pws->vsl_save;
				fpd_parse(pws->pth_save, &drv, G.g_tmppth, fname, fext);
				do_xyfix(&pws->x_save, &pws->y_save);
				pro_chdir(drv, G.g_tmppth);
				if (DOS_ERR)
				{
					win_free(pw);
					continue;
				}
				do_diropen(pw, TRUE, pws->obid_save, drv, G.g_tmppth, 
							fname, fext, (GRECT *)pws, TRUE);
			}
			
		}
	}
	cnx_put();
} /* cnx_get */


WORD GEMAIN(WORD ARGC, BYTE *ARGV[])
{
	WORD		ii, done, flags;
	UWORD		mx, my, button, kstate, kret, bret;
	WORD		ev_which;
//	WSAVE		*pws;
	LPTREE		tree;
	LPUWORD		plong[2];
	WORD		hsave[2];
#if MULTIAPP
	WORD		junk1, junk2;
	LONG		csize;
	LPVOID		templn;
	BYTE		memszstr[8];
#endif
	BYTE		docopyrt;
/* initialize libraries	*/

#if DEBUG
	remove("c:/gemapp.log");
#endif
    
	memset(&gl_xbuf, 0, sizeof(gl_xbuf));
	gl_xbuf.buf_len = sizeof(gl_xbuf);
	gl_apid = appl_init(&gl_xbuf);
						/* get GEM's gsx handle	*/
	gl_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
						/* open a virtual work-	*/
						/*   station for use by	*/
						/*   the desktop	*/
	gsx_vopen();
						/* init gsx and related	*/
						/*   device dependent	*/
						/*   variables		*/
	gsx_start();
	gl_whsiztop = NIL;
	gl_idsiztop = NIL;
						/* set clip to working	*/
						/*   desk and calc full	*/
	wind_get(0, WF_WXYWH, &G.g_xdesk, &G.g_ydesk, &G.g_wdesk, &G.g_hdesk);

	wind_calc(1, -1, G.g_xdesk,  G.g_ydesk,  G.g_wdesk,  G.g_hdesk,
					&G.g_xfull, &G.g_yfull, &G.g_wfull, &G.g_hfull);
/* Done differently by DESKTOP v1.2 
	wind_get(0, WF_WXYWH, &G.g_xfull, &G.g_yfull, &G.g_wfull, &G.g_hfull);
	G.g_xfull += min(16, gl_wbox);
	G.g_yfull += gl_hbox;
	do_xyfix(&G.g_xfull, &G.g_yfull);
	G.g_wfull -= (2 * G.g_xfull);
	G.g_hfull -= (2 * gl_hbox);
	gl_normwin.g_x = G.g_xfull;
	gl_normwin.g_y = G.g_yfull;
	gl_normwin.g_w = G.g_wfull;
	gl_normwin.g_h = (G.g_hfull - (gl_hbox / 2)) / 2;*/
	
						/* init long addrs that	*/
						/*   will be used alot	*/
	G.a_cmd = ADDR(&G.g_cmd[0]);
	G.a_tail = ADDR(&G.g_tail[0]);
	G.a_fcb1 = ADDR(&G.g_fcb1[0]);
	G.a_fcb2 = ADDR(&G.g_fcb2[0]);
	G.a_rmsg = ADDR(&G.g_rmsg[0]);
						/* initialize mouse	*/
	wind_update(BEG_UPDATE);
	desk_wait(TRUE);
	wind_update(END_UPDATE);
						/* initialize resources	*/
	// DESKTOP v1.2 puts this in its own function
	rsrc_init();
	//rsrc_load( ADDR("DESKTOP.RSC") );
						/* initialize menus and	*/
						/*   dialogs		*/
	for(ii = 0; ii < NUM_ADTREES; ii++)
	  rsrc_gaddr(0, ii, (LPVOID *)&G.a_trees[ii]);


#if MULTIAPP
/* Not done in FreeGEM desktop since it only has an icon, not a graphical
 * title 
	templn = G.a_trees[ADDINFO];
	if (gl_height <= 300)
	{
	  templn2 = G.a_trees[ADLRSINF];	
	  copy_icon(templn, templn2, DEICON, LDEICON);
	  copy_icon(templn, templn2, DENAME1, LDENAME1);
	  copy_icon(templn, templn2, DENAME2, LDENAME2);
	  copy_icon(templn, templn2, DENAME3, LDENAME3);
	  copy_icon(templn, templn2, DENAME4, LDENAME4);
	} */
#endif

/* Not done in DESKTOP v1.2 since it has 
   no graphical GEM name
   	for (ii=0; ii<NUM_BB; ii++)		
	  app_tran(ii);
*/
	app_tran(0);
/* DESKTOP v1.2 has no splash screen. But later desktops do. */

	shel_get(ADDR(&gl_afile[0]), 1);
	docopyrt = (gl_afile[0] != '#');

	if (docopyrt)
	{
	  tree = G.a_trees[ADDINFO];		// copyright notice	
	  plong[0] = &tree[ROOT].ob_height;	// just upper part
	  plong[1] = &tree[INFOINNR].ob_height;	
	  hsave[0] = plong[0][0];
	  hsave[1] = plong[1][0];
	  plong[0][0] = 12*gl_hchar;
	  plong[1][0] =  7*gl_hchar;
	  tree[DEMEMCAP].ob_flags |= HIDETREE;
	  tree[DEMEMSIZ].ob_flags |= HIDETREE;
	  tree[AUTHOR1].ob_flags |= HIDETREE;
	  tree[AUTHOR2].ob_flags |= HIDETREE;
	  show_hide(FMD_START, tree);
	}

// not in DESKTOP v1.2	rsrc_gaddr(R_STRING, STASTEXT, (LPVOID *)&ad_ptext);
// not in DESKTOP v1.2	rsrc_gaddr(R_STRING, STASICON, (LPVOID *)&ad_picon);

						/* These strings are 	*/
						/* used by dr_code.  Can't */
						/* get to resource then	*/
						/* because that would	*/
						/* reenter AES.		*/
	strlcpy(gl_amstr, ini_str(STAM), sizeof(gl_amstr));
	strlcpy(gl_pmstr, ini_str(STPM), sizeof(gl_pmstr));
						/* initialize icons 	*/
						/*   and apps from	*/
						/*   shell memory or	*/
						/*   from DESKTOP.INF	*/
						/*   and DESKHI/LO.ICN	*/
	if (!app_start())
	{
	  fun_alert(1, STNOFILE, NULL);
	  pro_exit(G.a_cmd, G.a_tail);
	  return(FALSE);
	}

#if MULTIAPP
#define LOFFSET(x) ((((x)&0xFFFF0000l)>>12)+((x)&0x0FFFFl))
	lstlcpy(G.a_cmd, ADDR("GEMVDI.EXE"), sizeof(G.a_cmd));	
						/* get boot drive */
	shel_find(G.a_cmd);
	gl_bootdr = G.g_cmd[0];
	gl_untop = 0;

	proc_shrink(DESKPID);

	proc_info(DESKPID,&junk1,&junk2,(LPVOID *)&pr_begdsk,
			&csize,(LPVOID *)&pr_topmem,
			&pr_ssize,(LPVOID *)&pr_itbl);

	pr_begdsk = LOFFSET(pr_begdsk);		/* start of desk	*/
	pr_topdsk = pr_begdsk + csize;		/* addr above desktop	*/
	pr_topmem = LOFFSET(pr_topmem);

	csize = (pr_topmem - pr_begdsk) >> 10;		/* K app space	*/
	merge_str(&memszstr[0], "%L", csize);		/* to ASCII	*/
	iac_strcop(G.a_trees[ADDINFO], DEMEMSIZ, ADDR(&memszstr[0]));
	
	proc_info(GEMPID,&junk1,&junk2,(LPVOID *)&pr_beggem,&csize,
			&templn, &pr_ssize, &templn);
	pr_beggem = LOFFSET(pr_beggem);
	pr_begacc = pr_beggem + csize;			/* start of acc's */
	iac_init();
	G.a_trees[ADDINFO][DEMEMSIZ].ob_flags &= ~HIDETREE;
	G.a_trees[ADDINFO][DEMEMCAP].ob_flags &= ~HIDETREE;
#else
	/* Hide XM-only fields */
	G.a_trees[ADDINFO][DEMEMSIZ].ob_flags |= HIDETREE;
	G.a_trees[ADDINFO][DEMEMCAP].ob_flags |= HIDETREE;
#endif
	if (docopyrt)
	{
	  app_detect();
	}
						/* initialize windows	*/
	win_start();
						/* initialize folders,	*/
						/*   paths, and drives	*/
	fpd_start();
						/* show menu		*/
	desk_verify(0, FALSE);			/* should this be here	*/
	wind_update(BEG_UPDATE);
	menu_bar(G.a_trees[ADMENU], TRUE);
	wind_update(END_UPDATE);
						/* establish menu items	*/
						/*   that need to be	*/
						/*   checked, check 'em	*/
	G.g_viewpref  = ICONITEM;
	menu_icheck(G.a_trees[ADMENU], G.g_viewpref, TRUE);
	/* DESKTOP v1.2 uses the lines above instead 
	G.g_iview = V_ICON;
	menu_text(G.a_trees[ADMENU], ICONITEM, ad_ptext);
	*/
	G.g_csortitem = NAMEITEM;
	menu_icheck(G.a_trees[ADMENU], G.g_csortitem, TRUE);
						/* set up initial prefs	*/
	G.g_ccopypref = G.g_cdelepref = TRUE;
	G.g_ctimeform = TRUE;
	G.g_cdateform = TRUE;
	G.g_cdclkpref = 3;
						/* initialize desktop	*/
						/*   and its objects	*/
	gl_defdrv = app_blddesk();

	if (docopyrt)	/* Move this up to before it gets overdrawn by the
					 * desktop */
	{
	  evnt_timer(1000, 0);
	}

/* Not done in DESKTOP v1.2 					
	for (ii = 0; ii < NUM_WNODES; ii++)
	{
	  pws = &G.g_cnxsave.win_save[ii];
	  rc_copy(&gl_normwin, (GRECT *)&pws->x_save);	// copy in normal size	
	  if (pws->pth_save[0])
	    do_xyfix(&pws->x_save, &pws->y_save);
	} 
				// fix up y loc. of lower window	
	pws->y_save += pws->h_save + (gl_hbox / 2);
*/
	if (docopyrt)
	{
	  plong[0][0] = hsave[0];			// ABOUT height restored
	  plong[1][0] = hsave[1]; 
	  tree[AUTHOR1].ob_flags &= ~HIDETREE;
	  tree[AUTHOR2].ob_flags &= ~HIDETREE;
	  show_hide(FMD_FINISH, tree);	// copyright notice	
	}
/*
	fix_wins();		// update with latest window x,y,w,h	
*/
	/* Take over the desktop */
	wind_set(0, WF_NEWDESK, FP_OFF(G.a_screen), FP_SEG(G.a_screen), TRUE, FALSE);
	do_wredraw(0, G.g_xdesk, G.g_ydesk, G.g_wdesk, G.g_hdesk);
						/* set up current parms	*/

	
	desk_verify(0, FALSE);
						/* establish desktop's	*/
						/*   state from info 	*/
						/*   found in app_start,*/
						/*   open windows	*/
	G.g_wlastsel = 0;
	// not done in DESKTOP v1.2 wind_update(BEG_UPDATE);
	cnx_get();
	win_shwall();				/* [JCE 18-Aug-2005] */
	// not done in DESKTOP v1.2 wind_update(END_UPDATE);
	men_update(G.a_trees[ADMENU]); 
						/* get ready for main	*/
						/*   loop		*/
	flags = MU_BUTTON | MU_MESAG | MU_KEYBD;
	done = FALSE;
	// not done in DESKTOP v1.2 ig_close = FALSE;
						/* turn mouse on	*/
	desk_wait(FALSE);
						/* loop handling user	*/
						/*   input until done	*/


	while( !done )
	{
						/* block for input	*/
	  ev_which = evnt_multi(flags, 0x02, 0x01, 0x01, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				G.a_rmsg, 0, 0, 
				&mx, &my, &button, &kstate, &kret, &bret);

						/* grab the screen	*/
	  wind_update(BEG_UPDATE);
						/* handle keybd message */
	  if (ev_which & MU_KEYBD)
	    done = hndl_kbd(kret);

						/* handle button down	*/
	  if (ev_which & MU_BUTTON)
	    done = hndl_button(bret, mx, my, button, kstate);

						/* handle system message*/
	  while (ev_which & MU_MESAG)
	  {
	    done = hndl_msg();
						/* use quick-out to clean */
						/* out all messages	  */
	    if (done)
	      break;
	    ev_which = evnt_multi(MU_MESAG | MU_TIMER, 0x02, 0x01, 0x01, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				G.a_rmsg, 0, 0, 
				&mx, &my, &button, &kstate, &kret, &bret);
	  }

						/* free the screen	*/
	  wind_update(END_UPDATE);
	}
						/* save state in memory	*/
						/*   for when we come	*/
						/*   back to the desktop*/
	cnx_put();
	app_save(FALSE);
						/* turn off the menu bar*/
	menu_bar(0x0L, FALSE);
						/* close gsx virtual ws	*/
	v_clsvwk(gl_handle);
						/* exit the gem AES	*/
	appl_exit();

	return(TRUE);
} /* main */
