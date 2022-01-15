/*****************************************************************************
*   SETTINGS.APP - settings program for FreeGEM                              *
*   Copyright 1999, John Elliott                                             *
*                                                                           *
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
*                                                                           *
******************************************************************************/


#include "settings.h"
#include <stdio.h>

WORD cur_id;
//WORD round;

LPTEDI  ti_act, ti_inact, ti_info;

PPDUBLK col_ublk[16];
PPDUBLK id_ublk[16];
PPDUBLK stl_ublk[4];
PPDUBLK idx_ublk[24];
PPDUBLK pre_ublk[7];
PPDUBLK scheme_ublk[11];

#define CC_MAX 24

LPTREE gl_clnames;
CLRCAT cur_col[CC_MAX];
CLRCAT gem_col[CC_MAX];

char buf[2];

MLOCAL VOID set_clip(WORD clip_flag, GRECT *s_area)	/* set clip to specified area		*/
{
	WORD	pxy[4];

	rc_grect_to_array(s_area, pxy);
	vs_clip(vdi_handle, clip_flag, pxy);
}

MLOCAL LPBYTE clr_id(WORD id)
{
	if (id >= CC_NAME)
	{
		return gl_clnames[id - CC_NAME + 1].ob_spec;
	}
	return ADDR("?");
}



VOID write_colours(CLRCAT *cc)
{
	WORD xo;
	BYTE xc;
	
	use_colset(cc);
	for (xo = CC_NAME; xo < CC_MAX; xo++) if (xo != 15)
	{
		char buf[20], nbuf[20];

		if (xo < 10) xc = xo + '0';
		else         xc = xo + 'A' - 10;
		
		sprintf(nbuf, "Colour.%c", xc);
		sprintf(buf,  "%04x%04x%04x%04x", 
		                 cc[xo].cc_foreground,
		                 cc[xo].cc_background,
		                 cc[xo].cc_style,
		                 cc[xo].cc_pattern);
		prop_put(ADDR("GEM.AES"), ADDR(nbuf), ADDR(buf), 0);
	}
}

	

WORD paint_all(LPPARM lp)
{
	PARMBLK pb;
	LPCCAT ptr;
	
	LBCOPY((LPBYTE)ADDR(&pb), (LPBYTE)lp ,sizeof(PARMBLK));

    set_clip(TRUE, (GRECT *) &pb.pb_xc);

    ptr = (LPCCAT)pb.pb_parm;
    
	filled_rect((GRECT *)(&pb.pb_x), ptr->cc_foreground,
									 ptr->cc_background,
									 ptr->cc_style,
									 ptr->cc_pattern);

	vsl_width(vdi_handle, 1);
	vsl_color(vdi_handle, BLACK);
	draw_rect((GRECT *) &pb.pb_x);
	
	set_clip(FALSE, (GRECT *)&pb.pb_xc);

	return 0;
}




WORD paint_id(LPPARM lp)
{
	PARMBLK pb;
	WORD	pid;
	LPCCAT	ptr;
	char str[30];
	GRECT   rc;

	LBCOPY((LPBYTE)ADDR(&pb), (LPBYTE)lp ,sizeof(PARMBLK));
	
    set_clip(TRUE, (GRECT *) &pb.pb_xc);

    pid = (WORD)pb.pb_parm;
	ptr = &(cur_col[pid]); 
    
	filled_rect((GRECT *)(&pb.pb_x), WHITE, WHITE, 1, 1);
	rc.g_x  = pb.pb_x + pb.pb_w - 64;
	rc.g_y  = pb.pb_y + 1;
	rc.g_w  = 63;
	rc.g_h  = pb.pb_h - 2;
	filled_rect(&rc, ptr->cc_foreground,
					 ptr->cc_background,
					 ptr->cc_style,
					 ptr->cc_pattern);

	sprintf(str, "%-16.16s", clr_id(pid));

	vswr_mode(vdi_handle, 2);
	vst_color(vdi_handle, BLACK);
	v_gtext(vdi_handle, pb.pb_x + 1, pb.pb_y + pb.pb_h - 2, str);
	if (pb.pb_currstate & SELECTED)
		{
		vswr_mode(vdi_handle, 2);
		vst_color(vdi_handle, 0x2);
		v_gtext(vdi_handle, pb.pb_x + 1, pb.pb_y + pb.pb_h - 2, str);
		}

	vsl_width(vdi_handle, 1);
	vsl_color(vdi_handle, BLACK);
	--pb.pb_y;
	++pb.pb_h;
	draw_rect((GRECT *) &pb.pb_x);
	
	set_clip(FALSE, (GRECT *) &pb.pb_xc);
	
	return 0;
}





WORD paint_style(LPPARM lp)
{
	PARMBLK pb;
	WORD style;
	WORD dotext;

	char str[3];
	LBCOPY((LPBYTE)ADDR(&pb), (LPBYTE)lp ,sizeof(PARMBLK));

    set_clip(TRUE, (GRECT *) &pb.pb_xc);

    style  =  (WORD)pb.pb_parm & 0xFFFF;
	dotext = ((LONG)pb.pb_parm & 0x10000L) ? 1 : 0;
    
	filled_rect((GRECT *)(&pb.pb_x), BLACK, WHITE, style, 3);

	if (pb.pb_currstate & SELECTED)
		{
	  	vsl_color(vdi_handle, 3);
		vsl_width(vdi_handle, 2);
		vsl_type(vdi_handle, FIS_SOLID);

		pb.pb_x+=2;
		pb.pb_y+=2;
		pb.pb_w-=4;
		pb.pb_h-=4;
		draw_rect((GRECT *) &pb.pb_x);
		pb.pb_x-=2;
		pb.pb_y-=2;
		pb.pb_w+=4;
		pb.pb_h+=4;
	}  
	if (dotext)
	{
		sprintf(str, "%d", style);

		vswr_mode(vdi_handle, 1);
		vst_color(vdi_handle, BLACK);
		v_gtext(vdi_handle, pb.pb_x + (pb.pb_w/4), pb.pb_y + (pb.pb_h/2), str);
	}
	vsl_width(vdi_handle, 1);
	vsl_color(vdi_handle, BLACK);
	draw_rect((GRECT *) &pb.pb_x);
	
	set_clip(FALSE, (GRECT *)&pb.pb_xc);

	return 0;
}


WORD paint_index(LPPARM lp)
{
	PARMBLK pb;
	WORD index;
	WORD dotext;

	char str[3];
	LBCOPY((LPBYTE)ADDR(&pb), (LPBYTE)lp ,sizeof(PARMBLK));

    set_clip(TRUE, (GRECT *) &pb.pb_xc);

    index  =  (WORD)pb.pb_parm & 0xFFFF;
	dotext = ((LONG)pb.pb_parm & 0x10000L) ? 1 : 0;
    
	filled_rect((GRECT *)(&pb.pb_x), BLACK, WHITE, cur_col[cur_id].cc_style, index);
    
	if (pb.pb_currstate & SELECTED)
		{
	  	vsl_color(vdi_handle, 3);
		vsl_width(vdi_handle, 2);
		vsl_type(vdi_handle, FIS_SOLID);

		pb.pb_x+=2;
		pb.pb_y+=2;
		pb.pb_w-=4;
		pb.pb_h-=4;
		draw_rect((GRECT *) &pb.pb_x);
		pb.pb_x-=2;
		pb.pb_y-=2;
		pb.pb_w+=4;
		pb.pb_h+=4;
	}  
	if (dotext)
	{
		sprintf(str, "%d", index);

		vswr_mode(vdi_handle, 1);
		vst_color(vdi_handle, BLACK);
		v_gtext(vdi_handle, pb.pb_x + (pb.pb_w/4), pb.pb_y + (pb.pb_h/2), str);
	}
	vsl_width(vdi_handle, 1);
	vsl_color(vdi_handle, BLACK);
	draw_rect((GRECT *) &pb.pb_x);
		
	set_clip(FALSE, (GRECT *)&pb.pb_xc);

	return 0;
}







static VOID userdef(LPTREE tree)
{
	pre_ublk[0].ub_code = paint_colour;
	pre_ublk[1].ub_code = paint_colour;
	pre_ublk[2].ub_code = paint_style;
	pre_ublk[3].ub_code = paint_index;
	pre_ublk[4].ub_code = paint_all;
	
	pre_ublk[0].ub_parm = cur_col[cur_id].cc_foreground;
	pre_ublk[1].ub_parm = cur_col[cur_id].cc_background;
	pre_ublk[2].ub_parm = cur_col[cur_id].cc_style;
	pre_ublk[3].ub_parm = cur_col[cur_id].cc_pattern;
	pre_ublk[4].ub_parm = (LONG)(ADDR(&cur_col[cur_id]));

	ti_act->te_color &= 0x7F;
	ti_act->te_color |= ((cur_col[19].cc_foreground) << 8) & 0x0F00;
	ti_act->te_color |= ((cur_col[19].cc_background) <<12) & 0xF000;
	ti_act->te_color |= ((cur_col[19].cc_style) ? 0x80 : 0);

	ti_inact->te_color &= 0x7F;
	ti_inact->te_color |= ((cur_col[20].cc_foreground) << 8) & 0x0F00;
	ti_inact->te_color |= ((cur_col[20].cc_background) <<12) & 0xF000;
	ti_inact->te_color |= ((cur_col[20].cc_style) ? 0x80 : 0);

	ti_info->te_color &= 0x7F;
	ti_info->te_color |= ((cur_col[21].cc_foreground) << 8) & 0x0F00;
	ti_info->te_color |= ((cur_col[21].cc_background) <<12) & 0xF000;
	ti_info->te_color |= ((cur_col[21].cc_style) ? 0x80 : 0);
}


WORD do_drop(LPTREE tr, WORD ob, WORD rsc)
{
	LPTREE tr2;
	GRECT r;
	WORD opt;
	
	rsrc_gaddr(R_TREE, rsc, (LPVOID *)&tr2);

	ob_xywh(tr, ob, &r);

	opt = menu_popup(r.g_x + r.g_w - tr2->ob_width, r.g_y + r.g_h, tr2);

	ob_xywh(tr, ROOT, &r);
	
	set_clip(TRUE, &r);
	objc_draw(tr, ROOT, MAX_DEPTH, tr->ob_x - 1, tr->ob_y - 1, 
	                               tr->ob_width + 2, tr->ob_height + 2);
	set_clip(FALSE, &r);
	
	return opt;
}

WORD do_clrdrop(LPTREE tr, WORD ob)
{
	WORD opt;
	GRECT r;
	
	ob_xywh(tr, ob, &r);

	opt = colour_popup(vdi_handle, r.g_x + r.g_w - clr_tree_width(), r.g_y + r.g_h);

	set_clip(TRUE, &r);
	objc_draw(tr, ROOT, MAX_DEPTH, tr->ob_x - 1, tr->ob_y - 1, 
	                               tr->ob_width + 2, tr->ob_height + 2);
	set_clip(FALSE, &r);
	return opt;
}



WORD do_stldrop(LPTREE tr, WORD ob, WORD rsc)
{
	LPTREE tr2;
	WORD n;
	
	rsrc_gaddr(R_TREE, rsc, (LPVOID *)&tr2);

	for (n = 0; n < 4; n++)
	{
		tr2[n + 1].ob_state &= ~SELECTED;
		tr2[n + 1].ob_flags |= SELECTABLE | TOUCHEXIT; 

		stl_ublk[n].ub_parm = 0x10000L | n;
		stl_ublk[n].ub_code = paint_style;
		ppd_userdef(tr2, n+1, &stl_ublk[n]);
	}
	return do_drop(tr, ob, rsc);
}


WORD do_idxdrop(LPTREE tr, WORD ob)
{
	LPTREE tr2;
	WORD n, rsc, maxn;
	
	rsrc_gaddr(R_TREE, rsc, (LPVOID *)&tr2);

	if (cur_col[cur_id].cc_style == 3) 
	{
		maxn = 12;
		rsc = DDHATCH;
	}
	else
	{
		maxn = 24;
		rsc = DDINDEX;
	}
	rsrc_gaddr(R_TREE, rsc, (LPVOID *)&tr2);
	
	for (n = 0; n < maxn; n++)
	{
		tr2[n + 1].ob_state &= ~SELECTED;
		tr2[n + 1].ob_flags |= SELECTABLE | TOUCHEXIT; 

		idx_ublk[n].ub_parm = 0x10000L | (n+1);
		idx_ublk[n].ub_code = paint_index;
		ppd_userdef(tr2, n+1, &idx_ublk[n]);
	}
	return do_drop(tr, ob, rsc);
}

VOID use_colset(CLRCAT *cc)
{
	WORD n;

	for (n = 0; n < CC_MAX; n++)
	{
		xgrf_colour(n, cc[n].cc_foreground, 
		               cc[n].cc_background,
		               cc[n].cc_style,
		               cc[n].cc_pattern);
	}
}


VOID draw_pre(LPTREE tr, WORD o)
{
	GRECT r;

	/* Copy in the current "editing" colour set; draw preview; copy in
	 * GEM's colours */
	
	LBCOPY(ADDR(gem_col),	 ADDR(gl_xbuf.cc), CC_MAX * sizeof(CLRCAT));

	use_colset(cur_col);

	ob_xywh(tr, o, &r);
	
	objc_draw(tr, o, MAX_DEPTH, r.g_x, r.g_y, r.g_w, r.g_h);

	use_colset(gem_col);

}




VOID set_colours(LPTREE tr, WORD o)
{
	LPTREE  tree;
	GRECT	or;
	WORD	os;
	WORD	xdial, ydial, wdial, hdial, exitobj, indial;
	LPTEDI  ti_name;
	WORD	xo;

	clr_vdi_handle(vdi_handle);	
	indial  = 1;
	cur_id  = CC_NAME;
	LBCOPY(ADDR(cur_col), ADDR(gl_xbuf.cc), CC_MAX * sizeof(CLRCAT));
	
	rsrc_gaddr(R_TREE, COLOURS, (LPVOID *)&tree);
	rsrc_gaddr(R_TREE, DDITEMS, (LPVOID *)&gl_clnames);

	ppd_userdef(tree, CHOICE0,  &pre_ublk[0]);
	ppd_userdef(tree, CHOICE1,  &pre_ublk[1]);
	ppd_userdef(tree, CHOICE2,  &pre_ublk[2]);
	ppd_userdef(tree, CHOICE3,  &pre_ublk[3]);
	ppd_userdef(tree, CLRSAMPL, &pre_ublk[4]);

	ti_name  = (LPTEDI)(tree[OBNAME] .ob_spec);
	ti_act   = (LPTEDI)(tree[TIACT]  .ob_spec);
	ti_inact = (LPTEDI)(tree[TIINACT].ob_spec);
	ti_info  = (LPTEDI)(tree[TIINFO ].ob_spec);

/* Moved to GUI options screen
	if (prop_get(ADDR("GEM.AES"), ADDR("Square3D"), ADDR(buf), 2, 0)) round = 1;
	else round = 0;

	if (round) tree[SQUARE3D].ob_state &= ~SELECTED;
	else	   tree[SQUARE3D].ob_state |=  SELECTED;
*/	
	userdef(tree);
	
	ob_xywh(tr, o, &or);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);
	form_dial(0, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	form_dial(1, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	while (indial)
	{
		ti_name->te_ptext = clr_id(cur_id);

		os = tree[DROP0].ob_state;
		tree[DROP0].ob_state &= ~DISABLED;
		tree[DROP1].ob_state &= ~DISABLED;
		tree[DROP2].ob_state &= ~DISABLED;
		tree[DROP3].ob_state &= ~DISABLED;

		if (cur_col[cur_id].cc_style < 2)
			 tree[DROP3].ob_state |= DISABLED;
		else tree[DROP3].ob_state &= ~DISABLED;

		if (cur_id >= 16)
		{
			 tree[DROP2].ob_state |= DISABLED;
			 tree[DROP3].ob_state |= DISABLED;
		}
		if (cur_id == 18)
		{
			 tree[DROP0].ob_state |= DISABLED;
		}
		if (cur_id >= 19)
		{
			 tree[DROP2].ob_state &= ~DISABLED;
		}
		if (os != tree[DROP0].ob_state)
		{
			objc_draw(tree, DROP0, 1, xdial, ydial, wdial, hdial);
		}
		
		for (xo = 0; !(tree[xo].ob_flags & LASTOB); xo++)
		{
			if ((tree[xo].ob_type >> 8) == 1)
				objc_draw(tree, xo, 1, xdial, ydial, wdial, hdial);
		}

		draw_pre(tree, DESKSMPL);

		exitobj = form_do(tree, 0) & 0x7FFF;

		if ((tree[exitobj].ob_type >> 8) >= CC_NAME)
		{
			cur_id = tree[exitobj].ob_type >> 8;
			userdef(tree);

		}
		
		switch(exitobj)
		{
			case DROPOBS: xo = do_drop(tree, exitobj, DDITEMS);
						  if (xo >= 1)
						  {
							  cur_id = xo - 1 + CC_NAME;
							  userdef(tree);
						  }
						  break;

			case DROP0:	xo = do_clrdrop(tree, exitobj);
						if (xo >= 1)
						{
							cur_col[cur_id].cc_foreground = xo - 1;
							userdef(tree);
						}
						break;
						
			case DROP1:	xo = do_clrdrop(tree, exitobj);
						if (xo >= 1)
						{
							cur_col[cur_id].cc_background = xo - 1;
							if (cur_id == 18)
								cur_col[cur_id].cc_foreground = xo - 1;

							userdef(tree);
						}
						break;
			
			case DROP2:	xo = do_stldrop(tree, exitobj, DDSTYLE);
						if (xo >= 1)
						{
							cur_col[cur_id].cc_style = xo - 1;
							userdef(tree);
						}
						break;
			
			case DROP3:	xo = do_idxdrop(tree, exitobj);
						if (xo >= 1)
						{
							cur_col[cur_id].cc_pattern = xo;
							userdef(tree);
						}
						break;
			case BTNHELP:
						rsrc_alert(1, HELPCLR);
						break;
			
			case BTNOK:
			case BTNCANCL:
						indial = 0;		
						break;
		}
		tree[exitobj].ob_state &= ~SELECTED;
		objc_draw(tree, exitobj, 1, xdial, ydial, wdial, hdial);
		
	}

	if (exitobj == BTNOK)
	{
		GRECT gr_full;
		
		write_colours(cur_col);
/*
		if (tree[SQUARE3D].ob_state & SELECTED)
		     prop_put(ADDR("GEM.AES"), ADDR("Square3D"), ADDR("1"), 0);
		else prop_del(ADDR("GEM.AES"), ADDR("Square3D"), 0);

	if (round) tree[SQUARE3D].ob_state |=  SELECTED;
	else	   tree[SQUARE3D].ob_state &= ~SELECTED;
*/		
		gr_full.g_x = 0;
		gr_full.g_y = 0;
		gr_full.g_w = work_out[0];
		gr_full.g_h = work_out[1];
		send_redraw(0, &gr_full);		
	}


	form_dial(2, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	form_dial(3, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);

	
}



