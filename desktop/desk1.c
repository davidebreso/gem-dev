/*	DESK1.C		*/
/*	Routines specific to Desktop 1.x */
/*
*       Copyright 2001, John Elliott.     
*       Copyright 1999, Lineo inc.
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

MLOCAL VOID  zoom_closed(WORD close, WORD w_id, WORD xicon, WORD yicon)
{
	GRECT rc;
	wind_get(w_id, WF_WXYWH, &rc.g_x, &rc.g_y, &rc.g_w, &rc.g_h);
	if (close) wind_close(w_id);

	graf_shrinkbox(xicon, yicon, G.g_wicon, G.g_hicon,
			rc.g_x, rc.g_y, rc.g_w, rc.g_h);
}



MLOCAL WORD  w_setpath(WNODE *pw, WORD drv, BYTE *path, BYTE *name, BYTE *ext)
{
	WORD icx, icy;	// bp10 bp12
	GRECT rc;	// bp08 ich xoff yoff
	WORD res = 0;

//	dbg("w_setpath: drv=%d path=%s name=%s ext=%s\n",
//			drv, path, name, ext);

	wind_get(pw->w_id,WF_WXYWH, &rc.g_x, &rc.g_y, &rc.g_w, &rc.g_h);
	icx = rc.g_x + (rc.g_w / 2) - (G.g_wicon / 2);
	icy = rc.g_y + (rc.g_h / 2) - (G.g_hicon / 2);
	zoom_closed(0, pw->w_id, icx, icy);
	do_fopen(pw, 0, drv, path, name, ext, FALSE, TRUE); // XXX The last
				// 2 parameters are guessed at	
	return res;
}

WORD  true_closewnd(WNODE *pw)
{
	GRECT rc;	// ich xoff yoff bp10
	WORD  res = 0;
	// DESKTOP v1.2 also has 3 unused WORD variables here!

	wind_get(pw->w_id,WF_WXYWH, &rc.g_x, &rc.g_y, &rc.g_w, &rc.g_h);
	zoom_closed(1, pw->w_id, G.g_screen[pw->w_obid].ob_x, 
			G.g_screen[pw->w_obid].ob_y);
	pn_close(pw->w_path);
	win_free(pw);
	do_chkall(FALSE);	// XXX This FALSE was guessed at
	return res;
}


WORD  fun_close(WNODE *pw, WORD trueclose)
{
	BYTE *ppath, *pend;
	BYTE ext[4];	// bp+50h
	BYTE name[9];	// bp+47h
	BYTE path[66];	// bp+6
	WORD drv;	// bp+4
	WORD rv;	// bp+2

	if (!pw->w_path) 
	{
		form_alert(1,"[1][Invalid WNODE passed to fun_close()][ OK ]");
		return 0;
	}
	graf_mouse(HOURGLASS, NULL);
	fpd_parse(pw->w_path->p_spec, &drv, &path[0], &name[0], &ext[0]);
	if (trueclose) path[0] = 0;
	if (!path[0])
	{
		rv = true_closewnd(pw);
	}
	else
	{
		ppath = pend = path;
		pend += strlen(path)-1;
		while (pend != ppath && (*pend != '\\'))
		{
			--pend;
		}
		*pend = 0;
		rv = w_setpath(pw, drv, path, name, ext);
	}
	graf_mouse(ARROW, NULL);
	return rv;
}	

WNODE *  win_ontop()
{
	WORD tail    = G.g_screen->ob_tail;

	if (!G.g_screen[tail].ob_width || !G.g_screen[tail].ob_height) return 0; 
	return &G.g_wlist[tail - 2];	// Windows are obs 2,3,4,5
}


/* Align drive icon on a grid */
VOID  snap_disk(WORD x, WORD y, LPWORD px, LPWORD py)
{
	WORD xgrid, ygrid, icw, ich, xoff, yoff;

	icw  = G.g_icw;
	xgrid  = x / icw;
	xoff = x % icw;
	if (xoff <= (icw / 2)) *px = xgrid * icw;
	else		 		   *px = (xgrid+1)*icw;
	*px = min(gl_width - icw, *px);
	if ( *px < (gl_width / 2)) *px += (gl_width % icw);

	y -= G.g_ydesk;
	ich = G.g_ich;
	ygrid  = y / G.g_ich;
	yoff = y % G.g_ich;
	if (yoff <= (ich / 2)) *py = ygrid     * ich;
	else				    *py = (ygrid+1) * ich;
	*py = min(G.g_hdesk - ich, *py);
	if ( *py < (G.g_hdesk / 2)) *py += (G.g_hdesk % ich);
	*py += G.g_ydesk;
}



MLOCAL WORD  fun_file2desk(PNODE *pn_src,	// 0E
			  ANODE *an_dest,	// 10
			  WORD dobj)		// 12
{
	LPICON dicon;
	WORD operation;

	operation = -1;
	if (an_dest)
	{
		switch(an_dest->a_type)
		{
			case AT_ISDISK:
				dicon = (LPICON)G.g_screen[dobj].ob_spec;
				G.g_tmppth[0] = dicon->ib_char & 0xFF;
				strlcpy(G.g_tmppth + 1, ":\*.*", sizeof(G.g_tmppth)-1);
				operation = OP_COPY;
				break;
			case AT_ISTRSH: 
				operation = OP_DELETE;
				break;
		}
	}
	return fun_op(operation, pn_src, G.g_tmppth,
			0, 0, 0, 0);	// GEM/1 doesn't *have* the last 5 arguments!
}



MLOCAL WORD  fun_file2win(PNODE *pn_src, 	// 0a
			  BYTE  *spec,	 	// 0c
			  ANODE *an_dest,	// 0e
			  FNODE *fn_dest)	// 10
{
	BYTE *p;
	WORD buflen;

	strlcpy(G.g_tmppth, spec, sizeof(G.g_tmppth));

	/*
		for (p = G.g_tmppth; (*p) != '*'; ++p);
		*p = 0;
	*/
	p = strchr(G.g_tmppth, '*');
	if (p) 
	{
		*p = 0;
		buflen = sizeof(G.g_tmppth) - strlen(G.g_tmppth);
		if (an_dest && an_dest->a_type == AT_ISFOLD)
		{
			strlcpy(p, fn_dest->f_name, buflen);
			strlcat(p, "\\*.*", buflen);
		}
		else strlcat(p, "*.*", buflen);
	}
	return fun_op(OP_COPY, pn_src, G.g_tmppth,
			0, 0, 0, 0);	// GEM/1 doesn't *have* the last 5 arguments!
}

MLOCAL VOID  fun_win2desk(WORD wh, WORD obj)
{
	WNODE *wn_src;
	ANODE *an_dest;
	
	an_dest = app_afind(TRUE, AT_ISFILE, obj, NULL, NULL);
	wn_src = win_find(wh);
	if (fun_file2desk(wn_src->w_path, an_dest, obj))
	{
		fun_rebld(wn_src);
	}
}


MLOCAL WORD  fun_file2any(WORD sobj,	  // 12
			  WNODE *wn_dest, // 14
			  ANODE *an_dest, // 16
			  FNODE *fn_dest, // 18
			  WORD dobj)	  // 1A
{
	WORD okay = 0;
	FNODE *bp8;
	LPICON ib_src;
	PNODE *pn_src;
	
	ib_src = (LPICON)G.g_screen[sobj].ob_spec;
	pn_src = pn_open(ib_src->ib_char, "", "*", "*", F_SUBDIR);
	if (pn_src)
	{
		okay = pn_active(pn_src, FALSE);
		if (okay == 0x12) {}	// Meaningless test for DOS error...
		if (pn_src->p_flist) 
		{
            /* mark all files as selected */
			for (bp8 = pn_src->p_flist; bp8; bp8 = bp8->f_next)
			{
				bp8->f_obid = 0;
                bp8->f_selected = TRUE;
			}
			G.g_screen->ob_state = SELECTED;
			if (wn_dest)
			{
				okay = fun_file2win(pn_src, wn_dest->w_path->p_spec, an_dest, fn_dest);
			}
			else
			{
				okay = fun_file2desk(pn_src, an_dest, dobj);
			}
			G.g_screen->ob_state = 0;
		}
		pn_close(pn_src);
		desk_clear(DESKWH);
	}
	return okay;
}

MLOCAL VOID  fun_desk2win(WORD wh, WORD dobj) 
{
	WNODE *wn_dest;
	FNODE *fn_dest;
	WORD sobj, copied, junk;
	FNODE *fn_src;
	ANODE *an_src, *an_dest;

	wn_dest = win_find(wh);
	an_dest = i_find(wh, dobj, &fn_dest, &junk);
	sobj = 0;
	while ((sobj = win_isel(G.g_screen, 1, sobj)))
	{
		an_src = i_find(0, sobj, &fn_src, &junk);
		if (an_src->a_type == AT_ISTRSH)
		{
			fun_alert(1, STNODRA2);
			continue;
		}
		copied = fun_file2any(sobj, wn_dest, an_dest, fn_dest, dobj);
		if (copied) fun_rebld(wn_dest);
	}
}


MLOCAL VOID  fun_desk2desk(WORD dobj)
{
	WORD sobj,  junk;
	FNODE *fn;
	WORD cont;
	ANODE *source;
	ANODE *target;
	LPICON lpicon;
	char drvname;

	target = app_afind(1, 0, dobj, NULL, NULL);
	sobj  = 0;
	while (sobj = win_isel(G.g_screen, 1, sobj))
	{	
		source = i_find(0, sobj, &fn, &junk);
		
		if (source == target) continue;
		if (source->a_type == AT_ISTRSH)
		{
			fun_alert(1, STNOSTAK);
			continue;
		}
		cont = 1;
		if (target->a_type == AT_ISTRSH)
		{
			lpicon = (LPICON)(G.g_screen[sobj].ob_spec);
			drvname = lpicon->ib_char & 0xFF;
			cont = fun_alert(2, STDELDIS, drvname);
		}
		if (cont != 1) continue;
		fun_file2any(sobj, NULL, target, NULL, dobj);
	}
}



WORD  desk1_drag(WORD wh, WORD dest_wh, WORD sobj, WORD dobj, WORD mx, WORD my)
{
	WORD done = 0;

	if (wh)	// Dragging something from window
	{
		if (dest_wh) fun_drag(wh, dest_wh, dobj, mx, my);
		else		
		{
			if (sobj == dobj)
			{
				fun_alert(1, STNODRA1);
			}
			else
			{
				fun_win2desk(wh, dobj);
			}
		}
	}
	else	// Dragging something from desk
	{
		if (dest_wh)
		{
			fun_desk2win(dest_wh, dobj);
		}
		else	// Dragging from desk to desk
		{
			if (sobj != dobj) fun_desk2desk(dobj);
		}
	}
	return done;
}

