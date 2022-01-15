/*****************************************************************************
*   SETTINGS.APP - settings program for FreeGEM                              *
*   Copyright 1999, John Elliott                                             *
*                                                                            *
* This program is free software; you can redistribute it and/or              *
* modify it under the terms of the GNU General Public License                *
* as published by the Free Software Foundation; either version 2             *
* of the License, or (at your option) any later version.                     *
*                                                                            *
* This program is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
* GNU General Public License for more details.                               *
*                                                                            *
* You should have received a copy of the GNU General Public License          *
* along with this program; if not, write to the Free Software                *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
*                                                                            *
******************************************************************************/

#include "settings.h"
/*------------------------------*/
/*	defines			*/
/*------------------------------*/

#define	ARROW		0
#define	HOUR_GLASS	2			

#define	DESK		0

#define END_UPDATE	0
#define	BEG_UPDATE	1




/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Data Structures			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	Global			*/
/*------------------------------*/

X_BUF_V2 gl_xbuf;

/* No scrollbars. No resize */
#define WDW_STYLE	(NAME | CLOSER | MOVER | INFO)

/*------------------------------*/
/*	Local			*/
/*------------------------------*/

WORD	gl_wchar;			/* character width		*/
WORD	gl_hchar;			/* character height		*/
WORD	gl_wbox;			/* box (cell) width		*/
WORD	gl_hbox;			/* box (cell) height		*/
WORD	gem_handle;			/* GEM vdi handle		*/
WORD	vdi_handle;			/* settings vdi handle		*/
WORD	work_out[57];			/* open virt workstation values	*/
GRECT	work_area;			/* current window work area	*/
WORD	gl_apid;			/* application ID		*/
WORD	gl_rmsg[8];			/* message buffer		*/
LPBYTE	ad_rmsg;			/* LONG pointer to message bfr	*/
WORD	gl_xfull;			/* full window 'x'		*/
WORD	gl_yfull;			/* full window 'y'		*/
WORD	gl_wfull;			/* full window 'w' width	*/
WORD	gl_hfull;			/* full window 'h' height	*/
WORD	ev_which;			/* event message returned value	*/
WORD	settings_whndl = 0;		/* settings window handle		*/
BYTE	*wdw_title = "Skeleton";		/* blank window title		*/
BYTE	*info_line = "info line";
WORD	gl_itemsettings = 0;	/* Menu item (cf Hello) */
WORD	settings_wnd_w, settings_wnd_h;
LPTREE	gl_menu;			/* Menu bar */
LPTREE	gl_desk;			/* Desktop */
WORD	gl_deskcount;		/* No. of icons in desktop */

/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Local Procedures			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/



/*------------------------------*/
/*	min			*/
/*------------------------------*/
WORD min(WORD a, WORD b)			/* return min of two values	*/
{
	return( (a < b) ? a : b );
}


/*------------------------------*/
/*	max			*/
/*------------------------------*/
WORD max(WORD a, WORD b)			/* return max of two values	*/
{
	return( (a > b) ? a : b );
}



/*------------------------------*/
/*	do_open			*/
/*------------------------------*/
WORD do_open(WORD wh, WORD org_x, WORD org_y, 
             WORD x,  WORD y, WORD w, WORD h)	/* grow and open specified wdw	*/
{
	WORD	ret_code;

	graf_mouse(2,0x0L);
	graf_growbox(org_x, org_y, 21, 21, x, y, w, h);
	ret_code = wind_open(wh, x, y, w, h);
	graf_mouse(ARROW,0x0L);
	return(ret_code);
}


/*------------------------------*/
/*	do_close		*/
/*------------------------------*/
VOID do_close(WORD wh, WORD org_x, WORD org_y)	/* close and shrink specified window	*/
{
	WORD	x, y, w, h;

	graf_mouse(2,0x0L);
	wind_get(wh, WF_CXYWH, &x, &y, &w, &h);
	wind_close(wh);
	graf_shrinkbox(org_x, org_y, 21, 21, x, y, w, h);
	graf_mouse(ARROW,0x0L);
}

/*------------------------------*/
/*	set_clip		*/
/*------------------------------*/
VOID set_clip(WORD clip_flag, GRECT *s_area)	/* set clip to specified area		*/
{
	WORD	pxy[4];

	rc_grect_to_array(s_area, pxy);
	vs_clip(vdi_handle, clip_flag, pxy);
}

/*------------------------------*/
/*	align_x			*/
/*------------------------------*/
WORD align_x(WORD x)		/* forces word alignment for column positon,	*/
							/*   rounding to nearest word			*/
{
	return((x & 0xfff0) + ((x & 0x000c) ? 0x0010 : 0));
}	

/*------------------------------*/
/*	wdw_size		*/
/*------------------------------*/
VOID wdw_size(GRECT *box)	/* compute window size for given settings size	*/
{
	WORD	pw = settings_wnd_w, ph = settings_wnd_h;

	wind_calc(WC_BORDER, WDW_STYLE, gl_wfull/2-pw/2, gl_hfull/2-ph/2, pw, ph, &box->g_x, &box->g_y, &box->g_w, &box->g_h);
	box->g_x = align_x(box->g_x) - 1;	/* byte-align */
	if (box->g_y < gl_yfull) box->g_y = gl_yfull;
}


/*------------------------------*/
/*	disp_message		*/
/*------------------------------*/
VOID disp_mesag(GRECT *clip_area)	/* display message applying input clip	*/
{
	WORD	pxy[4];
//	WORD	ycurr;

	set_clip(TRUE, clip_area);
	vsf_interior(vdi_handle, 1);
	vsf_color(vdi_handle, BLACK);
	rc_grect_to_array(&work_area, pxy);
	vr_recfl(vdi_handle, pxy);	/* Blank entire area	*/

	set_clip(FALSE, clip_area);
}


VOID draw_desk(GRECT *clip)
{
	objc_draw(gl_desk, ROOT, MAX_DEPTH, 
			  clip->g_x, clip->g_y, clip->g_w, clip->g_h);
}


/*------------------------------*/
/*	do_redraw		*/
/*------------------------------*/
VOID
do_redraw(wh, area)		/* redraw message applying area clip	*/
WORD	wh;
GRECT	*area;
{
	GRECT	box;

	graf_mouse(M_OFF, 0x0L);
	wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	while ( box.g_w && box.g_h )
	{
		if (rc_intersect(area, &box))
		{
			if (wh == settings_whndl)
			{
				disp_mesag(&box);
			}
			if (wh == 0) /* Desktop */
			{
				draw_desk(&box);
			}
		}
		wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	}
	graf_mouse(M_ON, 0x0L);
}



VOID about_dlg(VOID)
{
	LPTREE  tree;
	GRECT	or;
	WORD	xdial, ydial, wdial, hdial;

	rsrc_gaddr(R_TREE, ABOUTDLG, (LPVOID *)&tree);
	tree[AESBUILD].ob_spec = ADDR(&gl_xbuf.info[0]);

	ob_xywh(gl_menu, DESKMENU, &or);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);
	form_dial(0, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	form_dial(1, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	
	form_do(tree, BTNAOK);

	tree[BTNAOK].ob_state &= ~SELECTED;

	form_dial(2, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	form_dial(3, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
}




/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Message Handling			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


WORD hndl_menu(WORD menu, WORD opt)
{
	switch(menu)
	{
		case FILEMENU: switch(opt)
						{
						case LDTHEME:
							load_theme();
							break;
						case SVTHEME:
							save_theme();
							break;

						case FILEQUIT: return 1;
						}
		case DESKMENU: if (opt == ABOUTBOX)
						{
							about_dlg();
						}
						break;
	}
	return 0;
}


WORD hndl_keybd(WORD kreturn)
{
	/* Menu shortcuts */
	WORD kcode = (kreturn & 0xFF);
	
	if (kcode == ('Q' - '@')) return hndl_menu(FILEMENU, FILEQUIT);
	return 0;
}


/*------------------------------*/
/*	hndl_mesag		*/
/*------------------------------*/
BOOLEAN	hndl_mesag()
{
	GRECT	box;
	BOOLEAN	done; 
	WORD	wdw_hndl;

	done = FALSE;
	wdw_hndl = gl_rmsg[3];			/* wdw handle of mesag	*/
	switch( gl_rmsg[0] )			/* switch on type of msg*/
	{
	case MN_SELECTED:	done = hndl_menu(wdw_hndl, gl_rmsg[4]);
						break;
		
	case AC_OPEN:				/* do accessory open	*/
		if ( (gl_rmsg[4] == gl_itemsettings) && 
		    (!settings_whndl) )		/* unless already open	*/
		{
			graf_mouse(HOUR_GLASS, 0x0L);
			settings_whndl = wind_create(WDW_STYLE, align_x(gl_xfull)-1, gl_yfull, gl_wfull, gl_hfull);
			if (settings_whndl == -1)
			{ 
				graf_mouse(ARROW, 0x0L);
				form_alert(1,
				ADDR("[3][Fatal Error !|Window not available|for settings.][ Abort ]"));
				settings_whndl = 0;
				return(TRUE); 
			}

			wind_set(settings_whndl, WF_NAME, FP_OFF(wdw_title), FP_SEG(wdw_title), 0, 0);
			wind_set(settings_whndl, WF_INFO, FP_OFF(info_line), FP_SEG(info_line), 0, 0);
			wdw_size(&box);
						/* open from screen cntr*/
			do_open(settings_whndl, gl_wfull/2, gl_hfull/2, box.g_x, box.g_y, box.g_w, box.g_h);

			wind_get(settings_whndl, WF_WXYWH,	&work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
			disp_mesag(&work_area);
			graf_mouse(ARROW,0x0L);
		}
		else   
		{
			graf_mouse(ARROW, 0x0L);
			wind_set(settings_whndl, WF_TOP, 0, 0, 0, 0); 
		}
		break;

	case AC_CLOSE:				/* do accessory close	*/
		if ( (gl_rmsg[3] == gl_itemsettings) &&
		    (settings_whndl) )
		{
			settings_whndl = 0;	/* reset window handle	*/
		}
		break;

	case WM_REDRAW:				/* do redraw wdw contnts*/
		do_redraw(wdw_hndl, (GRECT *) &gl_rmsg[4]);
		break;

	case WM_TOPPED:				/* do window topped	*/
		wind_set(wdw_hndl, WF_TOP, 0, 0, 0, 0);
		break;

	case WM_CLOSED:				/* do window closed	*/
		do_close(settings_whndl, gl_wfull/2, gl_hfull/2);
		wind_delete(settings_whndl);
		settings_whndl = 0;
		done = TRUE;
		break;

	case WM_MOVED:				/* do window move	*/

		if (gl_rmsg[4] + gl_rmsg[6] > gl_wfull)
		{
			gl_rmsg[4] = gl_xfull + gl_wfull - gl_rmsg[6] - 1;
		}
		if (gl_rmsg[5] + gl_rmsg[7] > gl_hfull)
		{
			gl_rmsg[5] = gl_yfull + gl_hfull - gl_rmsg[7] - 1;
		}
		gl_rmsg[4] = align_x(gl_rmsg[4]) - 1;

		wind_set(wdw_hndl, WF_CXYWH, align_x(gl_rmsg[4])-1, gl_rmsg[5], gl_rmsg[6], gl_rmsg[7]);
		wind_get(settings_whndl, WF_WXYWH,	&work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
		break;

	default:
		break;
	} /* switch */
	return(done);
} /* hndl_mesag */



void init_desk_tree(void)
{
	WORD n, x, y;
	
	/* initialize menu    */
	rsrc_gaddr(R_TREE, ICONSET, (LPVOID *)&gl_desk);

	gl_desk->ob_x      = 0; //gl_xfull;
	gl_desk->ob_y      = 0; //gl_yfull;
	gl_desk->ob_width  = gl_xfull + gl_wfull;
	gl_desk->ob_height = gl_yfull + gl_hfull;

	fix_icon       (vdi_handle, gl_desk);

	/* Arrange the icons on the "desktop" area */
	
	n = 0; 
	x = gl_xfull + (gl_desk[1].ob_width  / 2); 
	y = gl_yfull + (gl_desk[1].ob_height / 2);
	do
	{
		++n;
		gl_desk[n].ob_x = x;
		gl_desk[n].ob_y = y;

		x += (3 * gl_desk[n].ob_width) / 2;

		if ((x + gl_desk[n].ob_width) > gl_wfull)
		{
			y += (3 * gl_desk[n].ob_height) / 2;
			x = gl_xfull + (gl_desk[1].ob_width / 2);
		}
	}
	while (!(gl_desk[n].ob_flags & LASTOB));

	gl_deskcount = n;
	
	wind_set(0, WF_NEWDESK, FP_OFF(gl_desk), FP_SEG(gl_desk), ROOT, 0);

	graf_mouse(M_OFF, 0x0L);
	objc_draw(gl_desk, ROOT, MAX_DEPTH, gl_xfull, gl_yfull, gl_wfull, gl_hfull);
	graf_mouse(M_ON, 0x0L);
}


/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Hello Event Handler			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/



/*------------------------------*/
/*	settings			*/
/*------------------------------*/
settings()
{
	BOOLEAN	done;
	MEVENT e;
	
	memset(&e, 0, sizeof(MEVENT));
	
	e.e_flags = MU_BUTTON | MU_MESAG | MU_KEYBD;
	e.e_bclk  = 2;
	e.e_bmsk  = 1;
	e.e_bst   = 1;
	e.e_mepbuf = (LPWORD)ad_rmsg;
	
	/**/					/* loop handling user	*/
	/**/					/*   input until done	*/
	done = FALSE;				/*   -or- if DESKACC	*/
	while( !done )				/*   then forever	*/
	{
		ev_which = evnt_event(&e);

		if (ev_which & MU_MESAG)
		{
			wind_update(BEG_UPDATE);	/* begin window update	*/
			done = hndl_mesag();		/* handle event message	*/
			wind_update(END_UPDATE);	/* end window update	*/
		}
		if (ev_which & MU_KEYBD)
		{
			done = hndl_keybd(e.e_kr);
		}
		if (ev_which & MU_BUTTON)
		{
			WORD ob = objc_find(gl_desk, ROOT, MAX_DEPTH, e.e_mx, e.e_my);

			if (e.e_br < 2)
			{
				WORD n;

				wind_update(BEG_UPDATE);
				for (n = 1; n <= gl_deskcount; n++)
				{
					WORD os = gl_desk[n].ob_state;
					
					if (n == ob) gl_desk[n].ob_state ^= SELECTED;
					else         gl_desk[n].ob_state &= ~SELECTED;

					if (os != gl_desk[n].ob_state)
					{
						objc_draw(gl_desk, n, 0, gl_xfull, gl_yfull, gl_wfull, gl_hfull);
					}
				}
				wind_update(END_UPDATE);
			} 
			else switch(ob)
			{
				case SETCLRS: set_colours(gl_desk, ob); break;
				case SETWINS: set_windows(gl_desk, ob); break;
				case SETOPTS: set_options(gl_desk, ob); break;
			} 
		}
		
	}
}


/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Termination				     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	settings_term		*/
/*------------------------------*/  
settings_term() 
{
	v_clsvwk( vdi_handle );		/* close virtual work station	*/
	appl_exit();			/* application exit		*/ 
}

/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Initialization			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/

/*------------------------------*/
/*	settings_init		*/
/*------------------------------*/
WORD
settings_init()
{
	WORD	i;
	WORD	work_in[11];
	
	memset(&gl_xbuf, 0, sizeof(gl_xbuf));
	gl_xbuf.buf_len = sizeof(gl_xbuf);
	
	gl_apid = appl_init(&gl_xbuf);			/* initialize libraries	*/
	for (i = 0; i < 10; i++)
	{
		work_in[i]=1;
	}
	work_in[10] = 2;
	gem_handle  = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
	vdi_handle  = gem_handle;
	v_opnvwk(work_in, &vdi_handle, work_out);	/* open virtual work stn*/

	wind_update(BEG_UPDATE);
	graf_mouse(HOUR_GLASS, 0x0L);
	if (!rsrc_load( ADDR("GEMSET.RSC") ))
	{    	
		/* No Resource File  */
		graf_mouse(ARROW, 0x0L);
		form_alert(1,"[3][Fatal Error !|GEMSET.RSC|File Not Found][ Abort ]");
		v_clsvwk( vdi_handle );		/* close virtual work station	*/
		appl_exit();			/* application exit		*/ 
		return FALSE;
	}

	if (!(gl_xbuf.abilities & ABLE_PROP))	/* Can't do settings */
	{
		graf_mouse(ARROW, 0x0L);
		rsrc_alert(1, BADGEM);
		v_clsvwk( vdi_handle );		/* close virtual work station	*/
		appl_exit();			/* application exit		*/ 
		return FALSE;
	}
	if (!(gl_xbuf.abilities & ABLE_PROP2))	/* GUI options */
	{
		graf_mouse(ARROW, 0x0L);
		rsrc_alert(1, BADGEM2);
		v_clsvwk( vdi_handle );		/* close virtual work station	*/
		appl_exit();			/* application exit		*/ 
		return FALSE;
	}
	

	/* initialize menu    */
	rsrc_gaddr(R_TREE, MENUBAR, (LPVOID *)&gl_menu);

	if (vdi_handle == 0)
		return(FALSE);

	ad_rmsg = ADDR((BYTE *) &gl_rmsg[0]);
	wind_get(DESK, WF_WXYWH, &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull);

	init_desk_tree();

	/* show menu	      */
	menu_bar(gl_menu, TRUE);
	
	graf_mouse(ARROW, 0x0L);
	wind_update(END_UPDATE);	

	return(TRUE);
}

/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Main Program			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	GEMAIN			*/
/*------------------------------*/
WORD GEMAIN(WORD argc, BYTE *ARGV[])
{
	if (settings_init())			/* initialization	*/
	{
		settings();
	
		settings_term();			/* termination		*/
	}
	return 0;
}

