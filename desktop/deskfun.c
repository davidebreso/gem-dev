/*	DESKFUN.C	08/30/84 - 05/30/85		Lee Lorenzen	*/
/*			10/2/86  - 01/16/87		MDF		*/
/*	merge source	5/27/87  - 5/28/87		mdf		*/
/*	for 2.3		6/11/87				mdf		*/

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

#include "wccdesk.h"

MLOCAL WORD fnodes_found;
MLOCAL WNODE *search_window;

/* forward declarations */
MLOCAL WORD  cmp_names(BYTE *psrc, BYTE *pdst);

/*
*	Routine to transfer a string that requires integrated variables
*	that are merged in.  The resultant alert is then displayed;
*/
WORD  fun_alert(WORD defbut, WORD stnum, ...)
{
	va_list ap;
	va_start(ap, stnum);

	rsrc_gaddr(R_STRING, stnum, (LPVOID *)&G.a_alert);
	/* Always use merge_str() */
	lstlcpy((&G.g_2text[0]), G.a_alert, sizeof(G.g_2text));
	vsprintf(&G.g_1text[0], &G.g_2text[0], ap);
	G.a_alert = (&G.g_1text[0]);

	va_end(ap);
	return( form_alert(defbut, G.a_alert) );
}

VOID  fun_msg(WORD type, WORD w3, WORD w4, WORD w5, 
			 WORD w6, WORD w7)
{
	/* keep DESKTOP messages internal to DESKTOP -- no AES call	*/
	G.g_rmsg[0] = type;
	G.g_rmsg[1] = gl_apid;
	G.g_rmsg[2] = 0;
	G.g_rmsg[3] = w3;
	G.g_rmsg[4] = w4;
	G.g_rmsg[5] = w5;
	G.g_rmsg[6] = w6;
	G.g_rmsg[7] = w7;
	hndl_msg();
}

/*
*	Rebuild window path and pflist
*/
VOID  fun_rebld(WNODE *pwin)
{
	WORD		i, x, y, w, h;
	BYTE		*ptst;

	// fprintf(logfile, "fun_rebld(%d)\n", pwin->w_id);

	graf_mouse(HGLASS, 0x0L);
						/* set up path to check	*/
						/*   against all windows*/
	ptst = &pwin->w_path->p_spec[0];
						/* check all wnodes	*/
	for(i = NUM_WNODES; i; i--)
	{
	  pwin = win_ith(i);
						/* if opened and same	*/
						/*   path then rebuild	*/
	  if ( (pwin->w_id) && (!strcmp(&pwin->w_path->p_spec[0], ptst)) )
	  {
	    pn_active(pwin->w_path, TRUE);
	    desk_verify(pwin->w_id, TRUE);
// DESKTOP v1.2: Uncommented the "info line" bit.
	    win_sinfo(pwin, FALSE);

	    wind_get(pwin->w_id, WF_WXYWH, &x, &y, &w, &h);
	    fun_msg(WM_REDRAW, pwin->w_id, x, y, w, h);
	  } /* if */
	} /* for */
	graf_mouse(ARROW, 0x0L);
} /* fun_rebld */

/*
 *  Adds another folder to a pathname, assumed to be of the form:
 *      D:\X\Y\F.E
 *  where X,Y are folders and F.E is a filename.  In the above
 *  example, if the folder to be added was Z, this would change
 *  D:\X\Y\F.E to D:\X\Y\Z\F.E
 *
 *  Note: if the folder to be added is an empty string, we do nothing.
 *  This situation occurs when building the path string for a desktop
 *  shortcut that points to the root folder.
 *
 *  returns FALSE iff the resulting pathname would be too long
 */
MLOCAL BOOLEAN add_one_level(BYTE *pathname, BYTE *folder)
{
    WORD plen, flen;
    BYTE filename[LEN_ZFNAME+1], *p;

    flen = strlen(folder);
    if (flen == 0)
        return TRUE;

    plen = strlen(pathname);
    if (plen+flen+1 >= MAXPATHLEN)
        return FALSE;

    p = filename_start(pathname);
    strcpy(filename,p);     /* save filename portion */
    strcpy(p,folder);       /* & copy in folder      */
    p += flen;
    *p++ = '\\';            /* add the trailing path separator */
    strcpy(p,filename);     /* & restore the filename          */
    return TRUE;
}


/*
 *  Removes the lowest level of folder from a pathname, assumed
 *  to be of the form:
 *      D:\X\Y\Z\F.E
 *  where X,Y,Z are folders and F.E is a filename.  In the above
 *  example, this would change D:\X\Y\Z\F.E to D:\X\Y\F.E
 */
MLOCAL VOID remove_one_level(BYTE *pathname)
{
    BYTE *stop = pathname+2;    /* the first path separator */
    BYTE *filename, *prev;

    filename = filename_start(pathname);
    if (filename-1 <= stop)     /* already at the root */
        return;

    for (prev = filename-2; prev >= stop; prev--)
        if (*prev == '\\')
            break;

    strcpy(prev+1,filename);
}

/*
 *  converts string to wildcard-format spec
 *
 *  the string is obtained from the dialog box via unfmt_str()
 *
 *  sample conversions:
 *      "A"         => "A*.*"
 *      ".DOC"      => "*.DOC"
 *      "X?Y.?Z"    => "X?Y*.?Z*"
 */
MLOCAL VOID convert(BYTE *wildcard, BYTE *str)
{
    WORD i;
    BYTE *p, *q;

    /* convert name */
    for (i = 0, p = str, q = wildcard; i < 8; i++)
    {
        if (!*p || (*p == '.'))
        {
            *q++ = '*';
            break;
        }
        if (*p == '*')
        {
            *q++ = *p++;
            break;
        }
        *q++ = *p++;
    }
    *q++ = '.';

    /* look for end of name */
    while(TRUE)
    {
        if (!*p)
            break;
        if (*p++ == '.')
            break;
    }

    /* convert type */
    for (i = 0; i < 3; i++)
    {
        if (!*p)
        {
            *q++ = '*';
            break;
        }
        if (*p == '*')
        {
            *q++ = *p++;
            break;
        }
        *q++ = *p++;
    }
    *q++ = '\0';
}


/*
 *  Prompt for Search specification
 *
 *  returns FALSE if Cancel, or specification is empty
 */
MLOCAL BOOLEAN search_prompt(BYTE *searchname)
{
    BYTE filemask[LEN_ZFNAME];
    LPTREE tree;

    tree = G.a_trees[ADSEARCH];

    /*
     * clear any wildcard in dialog
     */
    inf_sset(tree, SFNAME, "");

    /*
     * get user input & if not 'OK', return FALSE
     */
    inf_show(tree, ROOT);
    if (inf_what(tree, SFOK, SFCNCL) != 1)
        return FALSE;

    /*
     * extract searchname from dialog
     *
     * returns TRUE iff the input is not empty
     */
    inf_sget(tree, SFNAME, filemask, sizeof(filemask));
    unfmt_str(filemask, searchname);

    return *searchname ? TRUE : FALSE;
}


/*
 *  Mark files/folders matching specification and, if one or more
 *  are found, redisplays window with the first-found match as high
 *  as possible within the window
 *
 *  if no matches are found, returns FALSE and does not redisplay
 */
MLOCAL BOOLEAN mark_matching_fnodes(WNODE *pw, BYTE *searchwild)
{
    WORD first_match = -1, n;
    FNODE *pf;

    /*
     * select all matching FNODEs
     */
    for (pf = pw->w_path->p_flist, n = 0; pf; pf = pf->f_next, n++)
    {
        if (wildcmp(searchwild, pf->f_name))
        {
            pf->f_selected = TRUE;
            fnodes_found++;
            if (first_match < 0)
                first_match = n;
        }
    }
    if (first_match < 0)
        return FALSE;

    /*
     * update info line & redisplay window, showing first match
     */
    win_sinfo(pw, TRUE);
    win_dispfile(pw, first_match);

    return TRUE;
}


/*
 *  Display a folder with matching FNODEs marked
 */
MLOCAL BOOLEAN search_display(WORD curr, BYTE *pathname, BYTE *searchwild)
{
    BOOLEAN newwin = FALSE;
	WORD	drv;
	BYTE	path[66], name[9], ext[4];
    

    if (!search_window)
    {
        search_window = win_alloc(curr);
        if (!search_window)
        {
            desk_wait(FALSE);
            fun_alert(1, STNOWIND);
            return FALSE;
        }
        newwin = TRUE;
    }

    /*
     * we open the new path, after closing the previous one (which
     * doesn't exist if this is a new window)
     */
    if (!newwin)
        pn_close(search_window->w_path);
    fpd_parse(pathname, &drv, path, name, ext);
    if (!do_diropen(search_window, newwin, curr, drv, path, name, ext,
                    (GRECT *)&G.g_screen[search_window->w_root].ob_x, FALSE))
        return FALSE;   /* bad pathname or error reading directory */
    
    /*
     * now mark matching FNODEs
     */
    mark_matching_fnodes(search_window, searchwild);

    /*
     * we marked one or more FNODEs, ask if user wants to continue.
     * note that we don't need to call desk_busy_off() here, since
     * do_diropen() always does that before returning.
     */
    if (fun_alert(1, STCNSRCH) != 1)
        return FALSE;   /* user cancelled */

    desk_wait(TRUE);

    return TRUE;
}


/*
 *  Recursively search folder icons
 *
 *  returns FALSE iff we should stop immediately, e.g. because user cancelled
 */
static BOOLEAN search_recursive(WORD curr, BYTE *pathname, BYTE *searchwild)
{
    // DTA dta, *save_dta;
    BYTE *p;
    WORD ret;
    BOOLEAN ok = TRUE;

    /*
     * we must use a local DTA to manage the recursive search
     */
    // save_dta = dos_gdta();
    dos_sdta(G.a_wdta);

    /*
     * check if there is a filename match; if so, display the folder
     */
    p = filename_start(pathname);
    strcpy(p, searchwild);
    ret = dos_sfirst(pathname, DISPATTR);
    strcpy(p, "*.*");
    // dos_sdta(save_dta); /* in case we must return */
    // set ret to error code, or 0 if dos_sfirst was successful */
    ret = (ret ? 0 : DOS_AX);
    switch(ret) {
    case 0:             /* file found, display folder */
        if (!search_display(curr, pathname, searchwild))
            return FALSE;   /* user cancelled */
        /** FALLTHROUGH **/
    case E_NOFILES:        /* nothing found, continue processing */
    case E_FILENOTFND:
        break;
    default:            /* some strange kind of error, ignore silently */
        return TRUE;
    }

    /*
     * at this point, either there were no matching filenames, or we found
     * some but the user wants to continue.  we do an fsfirst/fsnext loop
     * and call ourselves for every folder found.
     */
    // dos_sdta(&dta);     /* original DTA is already saved */

    for (ret = dos_sfirst(pathname, DISPATTR), ok = TRUE; ret; ret = dos_snext())
    {
        if (G.g_wdta[30] == '.')  /* ignore . and .. */
            continue;

        if (G.g_wdta[21] & F_SUBDIR)
        {
            if (!add_one_level(pathname, &G.g_wdta[30]))
                continue;   /* pathname is too long, silently ignore: FIXME */
            ok = search_recursive(0, pathname, searchwild);
            remove_one_level(pathname);
            if (!ok)
                break;
        }
    }

    // dos_sdta(save_dta);

    /*
     * by design, errors from fsfirst/fsnext are ignored
     */
    return ok;
}


/*
 *  Process the specified icon
 *
 *  returns TRUE iff we should continue
 */
MLOCAL BOOLEAN search_icon(WORD win, WORD curr, BYTE *searchwild)
{
    ANODE *pa;
    FNODE *pf;
    BYTE pathname[LEN_ZPATH];
    BYTE *p;

    pa = i_find(win, curr, &pf, NULL);
    if (!pa)
        return TRUE;

    switch(pa->a_type) {
    case AT_ISFOLD:
        {
            WNODE *temp = win_find(win);
            strcpy(pathname, temp->w_path->p_spec);
            strcpy(filename_start(pathname), pf->f_name);
        }
        break;
    case AT_ISDISK:
        p = pathname;
        *p++ = pa->a_letter;
        *p++ = ':';
        *p = '\0';
        break;
    default:            /* do nothing for file, trash or printer icon */
        return TRUE;
    }

    strcat(pathname, "\\*.*");

    if (!search_recursive(curr, pathname, searchwild))
        return FALSE;   /* propagate error to fun_search() */

    return TRUE;
}


/*
 *  Perform the desktop Search function
 */
VOID fun_search(WNODE *pw, WORD curr)
{
    BYTE searchname[LEN_ZFNAME], searchwild[LEN_ZFNAME];
    WORD win = G.g_cwin;    /* save because the global variables */
    WORD root = G.g_croot;  /*  will be changed by search_icon() */
    GRECT gr;

    if (!search_prompt(searchname))     /* prompt for name to search for */
        return;

    convert(searchwild, searchname);    /* convert to standard wildcard */

    /*
     * if there are one or more highlighted icons, process them
     */
    fnodes_found = 0;
    if (curr)
    {
        desk_wait(TRUE);
        search_window = NULL;
        for ( ; curr; curr = win_isel(G.g_screen, root, curr))
        {
            if (!search_icon(win, curr, searchwild))
                return;         /* user cancelled search (busy has been turned off) */
        }
        desk_wait(FALSE);
        if (fnodes_found)
        {
            fun_alert(1, STNOMORE); /* no more files */
             /* clear and redraw the original window (may be desktop) */
            desk_clear(win);
			wind_get(win, WF_WXYWH, &gr.g_x, &gr.g_y, &gr.g_w, &gr.g_h);
			do_wredraw(win, gr.g_x, gr.g_y, gr.g_w, gr.g_h);
            return;
        }
    }
    else    /* otherwise handle an open window with no highlighted icons */
    {
        mark_matching_fnodes(pw, searchwild);
    }

    if (!fnodes_found)
        fun_alert(1, STFILENF, searchname);
}


VOID fun_selectall(WNODE *pw)
{
	WORD		xc, yc, wc, hc;
    FNODE *pf;

    // fprintf(logfile, "fun_selectall(%d)\n", pw->w_id);
    /* paranoia - check for desktop pseudo-window */
    if (pw->w_root == DROOT)
        return;

    /*
     * select all filenodes & corresponding screen objects
     */
    for (pf = pw->w_path->p_flist; pf; pf = pf->f_next)
    {
        // fprintf(logfile, "%s\n", pf->f_name);
        pf->f_selected = TRUE;        
        G.g_screen[pf->f_obid].ob_state |= SELECTED;
    }
    /*
     * update info line & redisplay window
     */
    win_sinfo(pw, TRUE);
	wind_get(pw->w_id, WF_WXYWH, &xc, &yc, &wc, &hc);
    fun_msg(WM_REDRAW, pw->w_id, xc, yc, wc, hc);
}

/*
 *  Routine to update the file mask for the current window
 */
VOID fun_mask(WNODE *pw)
{
    BYTE *maskptr, filemask[LEN_ZFNAME];
    LPTREE tree;

    tree = G.a_trees[ADFMASK];

    /*
     * get current filemask & insert in dialog
     */
    maskptr = filename_start(pw->w_path->p_spec);
    fmt_str(maskptr, filemask);
    inf_sset(tree, MKNAME, filemask);

    /*
     * get user input
     */
    inf_show(tree, ROOT);

    /*
     * if 'OK', extract filemask from dialog, update pnode/display
     */
    if (inf_what(tree, FMOK, FMCNCL))
    {
        inf_sget(tree, FMMASK, filemask, sizeof(filemask));
        if (filemask[0])
        {
            unfmt_str(filemask, maskptr);
        }
        else    /* empty string => use the default of "*.*" */
        {
            strcpy(maskptr, "*.*");
        }
        win_sname(pw);
        fun_rebld(pw);
    }
}


/*
*	Routine that creates a new directory in the specified window/path
*/
WORD  fun_mkdir(WNODE *pw_node)
{
	PNODE		*pp_node;
	LPTREE		tree;
	WORD		more, cont, i;
	BYTE		fnew_name[12], unew_name[13], *ptmp;

	tree = G.a_trees[ADMKDBOX];
	pp_node = pw_node->w_path;
	ptmp = &G.g_srcpth[0];
	strlcpy(ptmp, pp_node->p_spec, sizeof(G.g_srcpth));
	i = 0;
	while (*ptmp++)
	{
	  if (*ptmp == '\\' || *ptmp == '/')
	    i++;
	}
	if (i > MAX_LEVEL)
	{
	  fun_alert(1, STFO8DEE);
	  return(FALSE);
	}
	cont = TRUE;
	while (cont)
	{
	  fnew_name[0] = 0;
	  inf_sset(tree, MKNAME, &fnew_name[0]);
	  show_hide(FMD_START, tree);
	  form_do(tree, 0);
	  cont = FALSE;
	  if (inf_what(tree, MKOK, MKCNCL))
	  {
	    inf_sget(tree, MKNAME, &fnew_name[0], sizeof(fnew_name));
	    unfmt_str(&fnew_name[0], &unew_name[0]);
	    if ( unew_name[0])
	    {
	      add_fname(&G.g_srcpth[0], &unew_name[0], sizeof(G.g_srcpth));
	      dos_mkdir(ADDR(&G.g_srcpth[0]));
	      if (DOS_ERR)
	      {
	 	cont = fun_alert(2, STFOEXIS) - 1;
		del_fname(&G.g_srcpth[0]);
	      }
	      else
	      {
	        more = d_errmsg();
	        if (more)
	          fun_rebld(pw_node);
	      } /* else */
	    } /* if */
	  } /* if OK */
	} /* while */
	show_hide(FMD_FINISH, tree);
	return(TRUE);
} /* fun_mkdir */

WORD  fun_op(WORD op, PNODE *pspath, BYTE *pdest, 
			WORD dulx, WORD duly, WORD from_disk, WORD src_ob)
{
	WORD		fcnt, dcnt;
	LONG		size;
	FNODE       *pf;
	

/* 
    fprintf(logfile,"fun_op(%d,...) with selected items:\n", op);
    for (pf = pspath->p_flist; pf; pf = pf->f_next)
    {
        if(pf->f_selected)
            fprintf(logfile, "%s\n", pf->f_name);
    }
 */

						/* do the operation	*/
	if (op != -1)
	{
	  if (op == OP_COPY)
	  {
	    if ( !par_chk(&pspath->p_spec[0], pspath->p_flist, 
				&G.g_tmppth[0]) )
	      return(FALSE);
	  }
						/* get count of source	*/
						/*   files		*/
	  dir_op(OP_COUNT, &pspath->p_spec[0], pspath->p_flist, pdest,
	  	 &fcnt, &dcnt, &size, dulx, duly, from_disk, src_ob);
						/* do the operation	*/
	  if ( fcnt || dcnt )
	  {
	    dir_op(op, &pspath->p_spec[0], pspath->p_flist, pdest,
	    	   &fcnt, &dcnt, &size, dulx, duly, from_disk, src_ob);
	    return(TRUE);
	  } /* if */
	} /* if */
	graf_mouse(ARROW, 0x0L);
	return(FALSE);
} /* fun_op */

/*
*	Routine to call when a list of files has been dragged on
*	top of a particular destination inside of a window.
*/
MLOCAL WORD  fun_wdst(PNODE *pspath, BYTE *pdspec, WORD datype, FNODE *pdf, 
			WORD dulx, WORD duly, WORD from_disk, WORD src_ob, WORD *pdo_both)
//	PNODE		*pspath;		/* source path		*/
//	BYTE		*pdspec;		/* destination file spec*/
//	WORD		datype;			/* destination icon	*/
//	FNODE		*pdf;			/* destination file/fold*/
//	WORD		dulx, duly;		/* destination x, y	*/
//	WORD		from_disk;		/* TRUE if src is disk	*/
//	WORD		src_ob;			/* source object	*/
//	WORD		*pdo_both;		/* see AT_ISFOLD below  */
{
	WORD		op;
	BYTE		*pdname, drv_ltr, *ptmp;
	LPICON		spib;
	WNODE		*pwname1, *pwname2;
	WORD		pdlen;
						/* default case: don't	*/
	*pdo_both = FALSE;			/* redraw both windows	*/
						/* set up destination	*/
						/*   path name		*/
/* BugFix	*/
	if (G.g_iview == V_TEXT)
	{
	  ptmp = &G.g_tmppth[0];		/* set in dr_fnode()	*/
	  while (*ptmp < 'A' - 1)		/* find drive letter	*/
	    ptmp++;
	  drv_ltr = *ptmp; 
	} /* if V_TEXT */
/* */
	strlcpy(G.g_tmppth, pdspec, sizeof(G.g_tmppth));
	pdname = &G.g_tmppth[0];
	while (*pdname != '*')
	  pdname++;
	*pdname = 0;
						/* check destination	*/
	op = -1;
	pdlen = sizeof(G.g_tmppth) - (pdname - G.g_tmppth);
	switch( datype )
	{
	  case AT_ISFOLD:
						/* if destination is	*/
						/*   folder then append	*/
						/*   folder to window	*/
						/*   path, if it is a	*/
						/*   fake then treat it	*/
						/*   like open space	*/
		if (pdf->f_attr & F_FAKE)
		  strlcat(pdname, "*.*", pdlen);
		else
		{
		  strlcpy(pdname, pdf->f_name, pdlen);
		  strlcat(pdname, "\\*.*", pdlen);
		}
		op = OP_COPY;
/* BugFix	*/
				/* check to see if this dest. folder	*/
				/*   is also an open window		*/
		pwname1 = &G.g_wlist[0];
		pwname2 = &G.g_wlist[1];
		if ( cmp_names(&G.g_tmppth[0], pwname1->w_name) ||
		     cmp_names(&G.g_tmppth[0], pwname2->w_name) )
		  *pdo_both = TRUE;
/* */
		break;
 	  case AT_ISDISK:
						/* if destination is	*/
						/*   disk then use	*/
						/*   icon char. to build*/
						/*   dest path		*/
		if (G.g_iview == V_TEXT)
		  G.g_tmppth[0] = drv_ltr;
		else
		{
		  spib = get_spec(G.g_screen, pdf->f_obid);
	          G.g_tmppth[0] = (0x00FF & spib->ib_char);
		} /* else */
		strlcpy(G.g_tmppth + 1, ":\\*.*", sizeof(G.g_tmppth)-1);
		op = OP_COPY;
		break;
	  case AT_ISFILE:
						/* if destination is	*/
						/*   window or a file	*/
						/*   icon then use	*/
						/*   window path	*/
		strlcat(pdname, "*.*", pdlen);
		op = OP_COPY;
		break;
	  case AT_ISTRSH:
		op = OP_DELETE;
		break;
	} /* switch */
	return( fun_op(op, pspath, &G.g_tmppth[0], dulx, duly,
		from_disk, src_ob) );
} /* fun_wdst */

/*
*	Routine to call when the source of a drag is a disk
*	and the destination is either a window or another
*	disk.
*/
MLOCAL WORD  fun_disk(WORD src_ob, WNODE *pdw, WORD datype, FNODE *pdf, WORD dulx, WORD duly)
{
	WORD		ret, do_both;
	FNODE		*pf;
	LPICON		spib;
	PNODE		*pspath;
	BYTE		chr;

	if (G.g_iview == V_TEXT)
	  chr = LBGET(G.g_udefs[src_ob].ub_parm); /* get drive from user obj */
	else
	{
	  spib = get_spec(G.g_screen, src_ob);
	  chr = spib->ib_char;
	} /* else */
						/* build a source path	*/
	pspath = pn_open(chr, "", "*", "*", F_SUBDIR);
						/* if one available	*/
	if (pspath)
	{
						/* read the directory	*/
	  ret = pn_active(pspath, FALSE);
						/* if files to copy	*/
	  ret = FALSE;
	  if (pspath->p_flist)
	  {
						/* select all files	*/
	    for(pf = pspath->p_flist; pf; pf = pf->f_next)
	      pf->f_obid = 0;
	    G.g_screen[0].ob_state = SELECTED;
						/* do the copy or delete*/
	    if (pdw != (WNODE *) 0)
	      ret = fun_wdst(pspath, &pdw->w_path->p_spec[0], datype,
	      		     pdf, dulx, duly, TRUE, src_ob, &do_both);
	    else
	      ret = fun_op(OP_DELETE, pspath, &G.g_tmppth[0],
	      		   dulx, duly, TRUE, src_ob);
						/* return to normalcy	*/
	    G.g_screen[0].ob_state = NORMAL;
	  } /* if */
	  pn_close(pspath);
  		/* rebuild any windows with dpib.ib_char in title	*/
	  desk_clear(DESKWH);
	} /* if pspath */
	return(ret);
} /* fun_disk */

/*
*	Routine to call when several icons have been dragged from a
*	window to another window (it might be the same window) and 
*	dropped on a particular icon or open space.
*/
VOID  fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly)
{
	WORD		ret, junk, datype, src_ob, do_both;
	WNODE		*psw, *pdw;
	ANODE		*pda;
	FNODE		*pdf;

	psw = win_find(src_wh);
	pdw = win_find(dst_wh);
	if (pdw->w_path->p_spec[0] == '@' && dst_ob <= 3)
	  return;

	pda = i_find(dst_wh, dst_ob, &pdf, &junk);
	datype = (pda) ? pda->a_type : AT_ISFILE;

	if (psw->w_path->p_spec[0] != '@')
	{
	  ret = fun_wdst(psw->w_path, &pdw->w_path->p_spec[0], 
			 datype, pdf, dulx, duly, FALSE, 0, &do_both);
	  if (ret)
	  {
	    if (src_wh != dst_wh)
	      desk_clear(src_wh);
/* BugFix	*/
	    if (do_both)		/* force redraw of both windows	*/
	    {
	      fun_rebld(&G.g_wlist[0]);
	      fun_rebld(&G.g_wlist[1]);
	    }
/* */
	    else
	      fun_rebld(pdw);
	  } /* if ret */
	} /* if */
	else
	{
	  src_ob = 0;
	  while ( (src_ob = win_isel(G.g_screen, G.g_croot, src_ob)) != 0 )
	  {
	    ret = fun_disk(src_ob, pdw, datype, pdf, dulx, duly);
	    if (ret)
	      fun_rebld(pdw);
	  } /* while */
	} /* else */
} /* fun_drag */

/*
*	Routine to call when several icons have been dragged from a
*	desktop to the desktop and dropped on a particular icon.
*/
VOID  fun_del(WNODE *pdw)
{
	WORD		src_ob, ret;
	LPICON		spib;
	BYTE		drvch, *ptmp;
	FNODE       *pf;


/* 
	fprintf(logfile,"fun_del() for selected items:\n");
	for (pf = pdw->w_path->p_flist; pf; pf = pf->f_next)
    {
        if(pf->f_selected)
            fprintf(logfile, "%s\n", pf->f_name);
    }
 */


	if (pdw->w_path->p_spec[0] != '@')
	{
	  ret = fun_op(OP_DELETE, pdw->w_path, &G.g_tmppth[0], 0, 0, 0, 0);
	  if (ret)
/*	    fun_rebld(pdw);*/
	    do_chkall(TRUE);
	}
	else
	{
	  src_ob = 0;
	  while ( (src_ob = win_isel(G.g_screen, G.g_croot, src_ob)) != 0 )
	  {
	    ret = 1;
/* BugFix	*/
	    if (G.g_iview == V_TEXT)
	    {
	      ptmp = &G.g_tmppth[0];
	      while (*ptmp < 0x40)
	        ptmp++;
	      drvch = *ptmp;
	    } /* if V_TEXT */
/* */
	    else
	    {
	      spib = get_spec(G.g_screen, src_ob);
	      drvch = (0x00FF & spib->ib_char);
	    } /* else */
	    graf_mouse(ARROW, 0x0L);
	    ret = fun_alert(2, STDELDIS, drvch);
	    graf_mouse(HGLASS, 0x0L);
	    if (ret == 1)
	    {
	      fun_disk(src_ob, NULL, AT_ISTRSH, NULL, 0, 0);
	      do_chkall(TRUE);
	    }
	  } /* while */
	  graf_mouse(ARROW, 0x0L);
	} /* else */
} /* fun_del */

MLOCAL WORD  cmp_names(BYTE *psrc, BYTE *pdst)
{
	WORD		ret;
	BYTE		*lastslsh, *ptmp;
						/* scan to lastslsh	*/
	lastslsh = scan_slsh(psrc);
						/* null it		*/
	*lastslsh = 0;
						/* find trailing slash	*/
	ptmp = pdst;
	while (*ptmp) ptmp++;
	ptmp--;
	*ptmp = 0;				/* null it		*/
						/* see if they match	*/
	ret = !strcmp(psrc, pdst);	
						/* restore it		*/
	*lastslsh = '\\';
	*ptmp = '\\';
						/* return if same	*/
	return( ret );
} /* cmp_names */
