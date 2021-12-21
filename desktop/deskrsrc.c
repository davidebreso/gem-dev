/*	DESKRSRC.C	05/04/84 - 06/10/85	Lee Lorenzen		*/
/*	for 3.0		4/25/86			MDF			*/
/*	merge source	5/27/87			mdf			*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "wccdesk.h"

#define R_STRING 5

/* The adaptive resource beautifier (which makes checkboxes look right on
 * all GEM versions) is largely nicked from the FreeGEM RCS */
typedef VOID ( *MAPROUTINE)(LPTREE tr, WORD obj);

MLOCAL WORD ini_tree(WORD which);
MLOCAL VOID map_tree(LPTREE tree, WORD first, WORD last, MAPROUTINE routine);
MLOCAL VOID menu_cleanup(LPTREE tr, WORD obj);

VOID  rsrc_init(VOID)
{
	WORD n;
	LPTREE tree;
	
	rsrc_load(ADDR("DESKTOP.RSC"));

	for (n = ADFILEIN; n <= ADINSACC; n++) ini_tree(n);

	if (!(gl_xbuf.abilities & ABLE_X3D)) 
	{
		rsrc_gaddr(R_TREE, ADMENU, (LPVOID *)&tree) ;
		if (tree) map_tree(tree, ROOT, NIL, menu_cleanup);
	}
}


BYTE *  ini_str(WORD stnum)
{
	LPBYTE	lstr;

	rsrc_gaddr(R_STRING, stnum, (LPVOID *)&lstr);
	lstlcpy(ADDR(&gl_lngstr[0]), lstr, sizeof(gl_lngstr));
	return(&gl_lngstr[0]);
}


/*  If the AES supports checkboxes and radio buttons, implement them. 
 * 
 *  This is done simply by changing buttons with the DRAW3D and WHITEBAK
 *  flags to strings. That way, the controls look like buttons in older
 *  GEMs and like checkboxes in new ones. 
 * 
 */
	
#define EXT3D (DRAW3D | WHITEBAK)

/* The FreeGEM checkboxes and radio buttons are based on those in GEM/5.
 * This program will detect GEM/5 or FreeGEM and act accordingly.
 */

	
MLOCAL WORD gem5 = -1;
	
VOID  make_cbox(LPTREE tr, WORD obj)
{	
	if ((tr[obj].ob_state & EXT3D) == EXT3D)
	{
		if (tr[obj].ob_type == G_BUTTON) 
		{
			tr[obj].ob_type = G_STRING;
		}

		/* The little pseudo-buttons on the "file info" page are not 
		 * buttons, but boxes. So rather than make them strings, just
		 * take their borders and 3D away. */
		if (tr[obj].ob_flags & FLAG3D)
		{
			tr[obj].ob_flags &= ~FLAG3D;
			tr[obj].ob_flags &= ~USECOLORCAT;
			tr[obj].ob_spec = (LPVOID) (((LONG)tr[obj].ob_spec) & 0xFF00FFFFL);
			if (gem5) 
			{
				tr[obj].ob_state |= CROSSED;
				tr[obj].ob_type   = G_STRING;
				tr[obj].ob_spec   = ADDR(" ");
			}
		}
	}
	else if (tr[obj].ob_type == G_BUTTON)
	{
		tr[obj].ob_state |= DRAW3D;
	}


	/* Do the GEM/5 3D bits */
	if (gem5) 
	{
		if (tr[obj].ob_flags & FLAG3D)
		{
			tr[obj].ob_state |= CROSSED;
	
			if (tr[obj].ob_type == G_BOX  || tr[obj].ob_type == G_BOXCHAR)
			{
				tr[obj].ob_spec = (LPVOID)((((LONG)tr[obj].ob_spec) & 0xFFFFFF80L) | 0x7E);
				tr[obj].ob_flags |= OUTLINED;
			}
		}
		if (tr[obj].ob_state & EXT3D)
		{
			tr[obj].ob_state |= CROSSED;
		}

		if (tr[obj].ob_type == G_BOX && tr[obj].ob_state & SHADOWED)
		{
			tr[obj].ob_state |= CROSSED | CHECKED;
			tr[obj].ob_state &= ~SHADOWED;	
			tr[obj].ob_spec = (LPVOID)((((LONG)tr[obj].ob_spec) & 0xFFFFFF80L) | 0x7E);
		}

		if (tr[obj].ob_type == G_FTEXT || tr[obj].ob_type == G_FBOXTEXT)
		{
			tr[obj].ob_type = G_FBOXTEXT;
			tr[obj].ob_state |= CROSSED | OUTLINED | CHECKED;
		}
/* Title bars */
		if (tr[obj].ob_type == G_BOXTEXT && ((LPTEDI)tr[obj].ob_spec)->te_color == 0x1191  )
		{
		 ((LPTEDI)tr[obj].ob_spec)->te_color = 0x1A7B;
			
		}
/* Entry fields */
		if (tr[obj].ob_type == G_TEXT || tr[obj].ob_type == G_BOXTEXT)
		{
			tr[obj].ob_type = G_BOXTEXT;
			tr[obj].ob_state |= CROSSED | OUTLINED | CHECKED;
		}
	}
	
}


/* Remove underlines from a string, moving all the other characters up
 * and putting spaces at the end */
MLOCAL VOID chop_underline(LPBYTE txt)
{
	LPBYTE txt2;
	while (*txt)
	{
		if (*txt == '_')
		{
			txt2 = txt;
			while (txt2[1])
			{
				txt2[0] = txt2[1];
				++txt2;	
			}
			txt2[0] = ' ';
		}
		++txt;
	}
}



MLOCAL VOID gem3_cleanup(LPTREE tr, WORD obj)
{	
	if (tr[obj].ob_type == G_BUTTON)
	{
		chop_underline(tr[obj].ob_spec);		
	}

	/* Shrink "extended 3D" buttons (checkboxes / radio buttons) a bit */
	if ((tr[obj].ob_state & EXT3D) == EXT3D)
	{
		tr[obj].ob_y += 2;
		tr[obj].ob_x += 2;
		tr[obj].ob_height -= 4;
		tr[obj].ob_width  -= 4;
	}
}


MLOCAL VOID menu_cleanup(LPTREE tr, WORD obj)
{	
	if (tr[obj].ob_type == G_STRING || tr[obj].ob_type == G_TITLE)
	{
		chop_underline(tr[obj].ob_spec);		
	}
}


	

	
MLOCAL WORD ini_tree(WORD which)		/* find tree address */
{
	WORD w;
	LPTREE tree;

	w = rsrc_gaddr(R_TREE, which, (LPVOID *)&tree) ;
	
	if (!tree) return w;

	if (gem5 == -1)
	{
		char name[40];
		WORD attrib[10];

		/* First check for the ViewMAX/2 driver, which also fails on 
		 * vqt_name() */
		if (vqt_attributes(gl_handle, attrib))
		{	
			vqt_name(gl_handle, 1, name);
			if (name[0]) gem5 = 0; else gem5 = 1;
		}
		else gem5 = 0;
	}

	/* Remove underlines from buttons if not in FreeGEM */
	if (!(gl_xbuf.abilities & ABLE_X3D)) 
	{
		map_tree(tree, ROOT, NIL, gem3_cleanup);
	}
	if ((gl_xbuf.abilities & ABLE_X3D) || gem5)
	{
		map_tree(tree, ROOT, NIL, make_cbox);
	}
	return w;
}


	

	
MLOCAL VOID map_tree(LPTREE tree, WORD first, WORD last, MAPROUTINE routine)
{
	REG WORD tmp1;
	REG WORD this = first;
	
						/* non-recursive tree	*/
						/*   traversal		*/
child:
						/* see if we need to	*/
						/*   to stop		*/
	if ( this == last)
	  return;

						/* do this object	*/
	(*routine)(tree, this);
						/* if this guy has kids	*/
						/*   then do them	*/
	tmp1 = tree[this].ob_head;
	
	if ( tmp1 != NIL )
	{
	  this = tmp1;
	  goto child;
	}
sibling:
						/* if this obj. has a	*/
						/*   sibling that is not*/
						/*   his parent, then	*/
						/*   move to him and do	*/
						/*   him and his kids	*/
	tmp1 = tree[this].ob_next;
	if ( (tmp1 == last) ||
	     (tmp1 == NIL) )
	  return;

	if ( tree[tmp1].ob_tail != this )
	{
	  this = tmp1;
	  goto child;
	}
						/* if this is the root	*/
						/*   which has no parent*/
						/*   then stop else 	*/
						/*   move up to the	*/
						/*   parent and finish	*/
						/*   off his siblings	*/ 
	this = tmp1;
	goto sibling;
}



