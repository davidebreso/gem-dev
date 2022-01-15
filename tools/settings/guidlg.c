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

#define PROP_GUI_RSRC 5

MLOCAL VOID set_pair(LPTREE tr, WORD o1, WORD o2, int which)
{
	if (which)
	{
		tr[o2].ob_state |=  SELECTED;
		tr[o1].ob_state &= ~SELECTED;
	}
	else
	{
		tr[o1].ob_state |=  SELECTED;
		tr[o2].ob_state &= ~SELECTED;
	}
}


VOID set_options(LPTREE tr, WORD o)
{
	LPTREE  tree;
	GRECT	or;
	WORD	xdial, ydial, wdial, hdial, exitobj, indial;
	char	val[5];
	
	indial  = 1;
	
	rsrc_gaddr(R_TREE, GUIOPTS, (LPVOID *)&tree);

	set_pair(tree, OPT1V0, OPT1V1, prop_gui_get(PROP_GUI_SQUARE3D));
	set_pair(tree, OPT2V0, OPT2V1, prop_gui_get(PROP_GUI_FRAME3D));
	set_pair(tree, OPT4V0, OPT4V1, prop_gui_get(PROP_GUI_MENU) & 1);
	set_pair(tree, OPT4V2, OPT4V3, prop_gui_get(PROP_GUI_MENU) & 2);
	set_pair(tree, OPT5V0, OPT5V1, prop_gui_get(PROP_GUI_RSRC));

	ob_xywh(tr, o, &or);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);
	form_dial(0, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	form_dial(1, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	while (indial)
	{
		exitobj = form_do(tree, 0) & 0x7FFF;

		switch(exitobj)
		{
			case GUICAN:	indial = 0; break;
			case GUIOK:		indial = 0; break;
			case GUIHLP:	rsrc_alert(1, HELPOPTS); break;
		}
		tree[exitobj].ob_state &= ~SELECTED;
		objc_draw(tree, exitobj, 1, xdial, ydial, wdial, hdial);
		
	}

	if (exitobj == GUIOK)
	{
		WORD opt4;
		
		if (tree[OPT1V0].ob_state & SELECTED) prop_gui_set(PROP_GUI_SQUARE3D, 0);
		else								  prop_gui_set(PROP_GUI_SQUARE3D, 1);

		if (tree[OPT2V0].ob_state & SELECTED) prop_gui_set(PROP_GUI_FRAME3D, 0);
		else								  prop_gui_set(PROP_GUI_FRAME3D, 1);

		if (tree[OPT4V0].ob_state & SELECTED) opt4  = 0;
		else								  opt4  = 1;
		if (tree[OPT4V3].ob_state & SELECTED) opt4 |= 2;
		
		prop_gui_set(PROP_GUI_MENU, opt4);

		if (tree[OPT5V0].ob_state & SELECTED) prop_gui_set(PROP_GUI_RSRC, 0);
		else				      prop_gui_set(PROP_GUI_RSRC, 1);

		sprintf(val, "%04x", prop_gui_get(PROP_GUI_SQUARE3D)); 
		prop_put(ADDR("GEM.AES"), ADDR("Option.1"), ADDR(val), 0);
		sprintf(val, "%04x", prop_gui_get(PROP_GUI_FRAME3D)); 
		prop_put(ADDR("GEM.AES"), ADDR("Option.2"), ADDR(val), 0);
		sprintf(val, "%04x", prop_gui_get(PROP_GUI_MENU)); 
		prop_put(ADDR("GEM.AES"), ADDR("Option.4"), ADDR(val), 0);
		sprintf(val, "%04x", prop_gui_get(PROP_GUI_RSRC)); 
		prop_put(ADDR("GEM.AES"), ADDR("Option.5"), ADDR(val), 0);

	}


	form_dial(2, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);
	form_dial(3, or.g_x, or.g_y, or.g_w, or.g_h, xdial, ydial, wdial, hdial);

	
}



