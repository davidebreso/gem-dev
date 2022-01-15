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


extern WORD work_out[];
extern WORD gl_hchar;

PPDUBLK ub_ckbox;


MLOCAL WINOBJ gemwin[NUM_WINOBJ];
MLOCAL WINOBJ mywin [NUM_WINOBJ];


WORD maybe_3d[] = {2,4,8,10,11,13,15,16,18, 0};


VOID get_specs(WINOBJ *o)
{
	WORD n, a, b, c, d;

	for (n = 0; n < NUM_WINOBJ; n++)
	{
		wind_get(n, WF_OBFLAG, &a, &b, &c, &d);
		o[n].ob_flags = a;
		wind_get(n, WF_OBTYPE, &a, &b, &c, &d);
		o[n].ob_type  = a;
		wind_get(n, WF_OBSPEC, &a, &b, &c, &d);
		o[n].ob_spec  = (LONG)MK_FP(b,a);
	}
}


VOID set_specs(WINOBJ *o)
{
	WORD n;
	LPVOID l;

	for (n = 0; n < NUM_WINOBJ; n++) 
	{
		if (o[n].ob_type == G_BOXCHAR)
		{
			l = (LPVOID)(o[n].ob_spec);

			wind_set(n, WF_OBFLAG, o[n].ob_flags, 0, 0, 0);
			wind_set(n, WF_OBSPEC, FP_OFF(l), FP_SEG(l), 0, 0);
		}
		else 
		{
			wind_set(n, WF_OBFLAG, o[n].ob_flags, 0, 0, 0);
		}
	}
}


VOID write_specs(WINOBJ *o)
{
	WORD xo;
	BYTE name[30], val[5];

	if (o[maybe_3d[0]].ob_flags & FLAG3D)
	{
		prop_del(ADDR("GEM.AES"), ADDR("Win2D"), 0);
		for (xo = 0; maybe_3d[xo]; xo++) o[maybe_3d[xo]].ob_flags |= FLAG3D;
	}
	else
	{
		prop_put(ADDR("GEM.AES"), ADDR("Win2D"), ADDR("1"), 0);
		for (xo = 0; maybe_3d[xo]; xo++) o[maybe_3d[xo]].ob_flags &= ~FLAG3D;
	}
	for (xo = 0; xo < NUM_WINOBJ; xo++) if (o[xo].ob_type == G_BOXCHAR)
	{
		sprintf(name, "Button.%c", (xo < 10) ? (xo + '0') : (xo + '7'));
		sprintf(val, "%02x", (WORD)(((o[xo].ob_spec) >> 24) & 0xFF));
		prop_put(ADDR("GEM.AES"), ADDR(name), ADDR(val), 0);
	}		
	set_specs(o);
}




WORD draw_ckbox(LPPARM lp)
{
	PARMBLK pb;
	WORD tx, ty, tw, th, n, m;
	WORD tpts[6];
	LPOBJ lpo;
	char txbuf[80];
	
	LBCOPY((LPBYTE)ADDR(&pb), (LPBYTE)lp ,sizeof(PARMBLK));
	lpo = &(((LPTREE)pb.pb_tree)[pb.pb_obj]);
	
	vsl_width(vdi_handle, 1);
	vsl_color(vdi_handle, BLACK);

	tw = pb.pb_w;
	pb.pb_w = pb.pb_h;
	draw_rect((GRECT *) &pb.pb_x);
	pb.pb_w = tw;
	
	tx = pb.pb_x + 2;
	ty = pb.pb_y + 2;
	tw = pb.pb_h - 4;
	th = pb.pb_h - 4;

	vsl_width(vdi_handle, 2);
	
	tpts[0] = tx;
	tpts[1] = ty + th - 4;
	tpts[2] = tx + 3;
	tpts[3] = ty + th - 1;
	tpts[4] = tx + tw - 1;
	tpts[5] = ty + 3;

	if (lpo->ob_state & CHECKED) 
	     vsl_color(vdi_handle, BLACK);
	else vsl_color(vdi_handle, WHITE);

	for (m = 0; m < 3; m++)
	{
		v_pline(vdi_handle, 3, tpts);
		for (n = 1; n < 6; n+=2) tpts[n]--;
	}

	n = LSTRLEN(pb.pb_parm); if (n > 80) n = 80;
	
	LBCOPY(ADDR(txbuf), pb.pb_parm, n);
	txbuf[79] = 0;

	v_gtext(vdi_handle, pb.pb_x + pb.pb_h + 4, pb.pb_y + gl_hchar, txbuf);
	
	return 0;
}


WORD get_type(LPTREE tree, WORD typ)
{
	WORD obj;

	for (obj = 0; !(tree[obj].ob_flags & LASTOB); obj++)
	{
		if ((tree[obj].ob_type >> 8) == typ) return ((LONG)tree[obj].ob_spec >> 24) & 0xFF;
	}
	return '?';
	
}	

VOID set_type(LPTREE tree, WORD typ, WORD val)
{
	WORD obj;
	LONG v, vl;
	GRECT r;
	
	v = val & 0xFF;
	v <<= 24;

	vl =  (mywin[typ].ob_spec & 0xFFFFFFL);
	vl |= v;
	mywin[typ].ob_spec = vl;

	for (obj = 0; !(tree[obj].ob_flags & LASTOB); obj++)
	{
		if ((tree[obj].ob_type >> 8) == typ) 
		{
			vl = (LONG)tree[obj].ob_spec;

			vl &= 0xFFFFFFL;
			vl |= v;

			tree[obj].ob_spec = (LPVOID)vl;
			
			ob_xywh(tree, obj, &r);
			objc_draw(tree, obj, 1, r.g_x, r.g_y, r.g_w, r.g_h); 
		}
		if ((tree[obj].ob_type >> 8) == typ + 80) 
		{
			LPTEDI ti = (LPTEDI)tree[obj].ob_spec;

			ti->te_ptext[0] = val & 0xFF;
			ob_xywh(tree, obj, &r);
			objc_draw(tree, obj, 1, r.g_x, r.g_y, r.g_w, r.g_h); 
		}
	}

}






VOID set_windows(LPTREE tr, WORD o)
{
	LPTREE  tree;
	GRECT	or;
	WORD	xdial, ydial, wdial, hdial, exitobj, indial;
	WORD	xo;
	WORD 	def = 0;
	
	indial  = 1;
	
	rsrc_gaddr(R_TREE, GADGETS, (LPVOID *)&tree);

	get_specs(gemwin);
	get_specs(mywin);

	if (gemwin[2].ob_flags & FLAG3D)
		 tree[WIN3D].ob_state |=  CHECKED;
	else tree[WIN3D].ob_state &= ~CHECKED;
	
	ob_xywh(tr, o, &or);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);
	form_dial(0, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	form_dial(1, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	for (xo = 0; !(tree[xo].ob_flags & LASTOB); xo++)
	{
		WORD t = tree[xo].ob_type >> 8;
		WORD c = tree[WIN3D].ob_state & CHECKED;

		if (t && t < 20)
		{
			LONG l = (LONG)tree[xo].ob_spec;
			l &= 0xFFFFFFL;
			l |= (mywin[t].ob_spec & 0xFF000000L);
			tree[xo].ob_spec = (LPVOID)l;
			if (c) tree[xo].ob_flags |= FLAG3D;
			else   tree[xo].ob_flags &= ~FLAG3D;
		}
		if (t > 80 && t < 100)
		{
			LPTEDI ti = (LPTEDI)tree[xo].ob_spec;

			ti->te_ptext[0] = ((mywin[t - 80].ob_spec) >> 24) & 0xFF;
		}
	}
	if (mywin[maybe_3d[0]].ob_flags & FLAG3D)
		 tree[WIN3D].ob_state |=  CHECKED;
	else tree[WIN3D].ob_state &= ~CHECKED;
	
	
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	while (indial)
	{
		exitobj = form_do(tree, def) & 0x7FFF;

		xo = tree[exitobj].ob_type >> 8;

		if (xo > 20 && xo < 40)
		{
			set_type(tree, xo - 20, get_type(tree, xo - 20) - 1);
		}
		if (xo > 40 && xo < 60)
		{
			set_type(tree, xo - 40, get_type(tree, xo - 40) + 1);
		}
		if (xo > 80 && xo < 100)
		{
			LPTEDI ti = (LPTEDI)tree[exitobj].ob_spec;

			set_type(tree, xo - 80, ti->te_ptext[0]);
		}
		
		switch(exitobj)
		{
			case WIN3D:
						tree[exitobj].ob_state ^= CHECKED;
						for (xo = 0; !(tree[xo].ob_flags & LASTOB); xo++)
						{
							WORD t = tree[xo].ob_type >> 8;
							WORD c = tree[exitobj].ob_state & CHECKED;

							if (t && t < 20)
							{
								if (c) tree[xo].ob_flags |= FLAG3D;
								else   tree[xo].ob_flags &= ~FLAG3D;

								objc_draw(tree, xo, 1, xdial,ydial,wdial,hdial);
							}
						}
						break;
			
			case BTNOK2:
			case BTNCAN2:
						indial = 0;		
						break;
			case BTNHLP2:
						rsrc_alert(1, HELPWIN);
						break;
		}
		tree[exitobj].ob_state &= ~SELECTED;
		objc_draw(tree, exitobj, 1, xdial, ydial, wdial, hdial);
	}

	if (exitobj == BTNOK2)
	{
		if (tree[WIN3D].ob_state & CHECKED) mywin[maybe_3d[0]].ob_flags |=  FLAG3D;
		else                                mywin[maybe_3d[0]].ob_flags &= ~FLAG3D;
		write_specs(mywin);
	}

	form_dial(2, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	form_dial(3, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	
}



