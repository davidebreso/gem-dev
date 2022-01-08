/************************************************************************

    EDICON 2.00 - icon editor for GEM

    Copyright (C) 1998,1999,2000  John Elliott <jce@seasip.demon.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*************************************************************************/


#include "ppdgem.h"
#include "edicon.h"
#include "ediconf.h"

#include <conio.h>



WORD newfile_dlg(VOID)
	{
	LPTREE	tree;
	WORD	xdial, ydial, wdial, hdial, exitobj;
	WORD	xtype;
	WORD	rv = 1;
	
	GRECT	box;

	objc_xywh(gl_menu, FILEMENU, &box);
	rsrc_gaddr(R_TREE, NEWFILE, (LPVOID *)&tree);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/
	
	form_dial(0, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/
	
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	FOREVER
	{
		exitobj = form_do(tree, 0) & 0x7FFF;
		xtype = tree[exitobj].ob_type & 0xFF00;	
		if (!xtype)  /* is not extended type */
			break;
	}

	form_dial(2, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* draws a	*/
	/**/				/* shrinking box 		*/
  
	form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* free 	*/
	/**/				/* screen space, causes redraw	*/


	if (tree[DESKHI].ob_state  & SELECTED) rv =  1;
	if (tree[DESKLO].ob_state  & SELECTED) rv =  2;
	if (tree[WINHI].ob_state   & SELECTED) rv =  3;
	if (tree[WINLO].ob_state   & SELECTED) rv =  4;
	if (tree[WINDBL].ob_state  & SELECTED) rv =  5;
	if (tree[BTNCAN4].ob_state & SELECTED) rv = -1;
	
	tree[BTNOK4] .ob_state &= ~SELECTED; /* deselect OK button	*/
	tree[BTNCAN4].ob_state &= ~SELECTED; /* Deselect Cancel button */
	
	return rv;
}



WORD saveas_dlg(VOID)
	{
	LPTREE	tree;
	WORD	xdial, ydial, wdial, hdial, exitobj;
	WORD	xtype;
	WORD	rv = 1;
	
	GRECT	box;

	objc_xywh(gl_menu, FILEMENU, &box);
	rsrc_gaddr(R_TREE, SAVEFMT, (LPVOID *)&tree);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/
	
	form_dial(0, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/
	
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	FOREVER
	{
		exitobj = form_do(tree, 0) & 0x7FFF;
		xtype = tree[exitobj].ob_type & 0xFF00;	
		if (!xtype)  /* is not extended type */
			break;
	}

	form_dial(2, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* draws a	*/
	/**/				/* shrinking box 		*/
  
	form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* free 	*/
	/**/				/* screen space, causes redraw	*/


	if (tree[GEMICN].ob_state  & SELECTED)  rv =  1;
	if (tree[WINICO].ob_state  & SELECTED)  rv =  2;
	if (tree[GEMRCS].ob_state  & SELECTED)  rv =  3;
	if (tree[BTNCAN5].ob_state & SELECTED)  rv = -1;
	
	tree[BTNOK5] .ob_state &= ~SELECTED; /* deselect OK button	*/
	tree[BTNCAN5].ob_state &= ~SELECTED; /* Deselect Cancel button */
	
	return rv;
}


