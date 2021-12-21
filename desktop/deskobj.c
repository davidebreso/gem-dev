/*	DESKOBJ.C	06/11/84 - 02/08/85		Lee Lorenzen	*/
/*	merge source	5/27/87				mdf		*/

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

#include "ppddesk.h"

#if 0
EXTERN VOID	movs();
EXTERN VOID	r_set();
EXTERN WORD	objc_add();

EXTERN WORD	gl_width;
EXTERN WORD	gl_height;
#endif

GLOBAL OBJECT	gl_sampob[2] =
{
	NIL, NIL, NIL, G_IBOX, NONE, NORMAL, 0x0L, 0, 0, 0, 0,
	NIL, NIL, NIL, G_BOX,  NONE, NORMAL, (LPVOID)0x00001100L, 0, 0, 0, 0
};


/*
*	Initialize all objects as children of the 0th root which is
*	the parent of unused objects.
*/
VOID  obj_init(VOID)
{
	WORD		ii;
	LPTREE		tree;

	tree = G.a_screen = ADDR(&G.g_screen[0]);
	for (ii = 0; ii < NUM_SOBS; ii++)
	{
	  G.g_screen[ii].ob_head = NIL;
	  G.g_screen[ii].ob_next = NIL;
	  G.g_screen[ii].ob_tail = NIL;
	}
	memcpy(&G.g_screen[ROOT], &gl_sampob[0], sizeof(OBJECT));
	r_set((GRECT *)&G.g_screen[ROOT].ob_x, 0, 0, gl_width, gl_height);
	for (ii = 0; ii < (NUM_WNODES+1); ii++)
	{
	  memcpy(&G.g_screen[DROOT+ii], &gl_sampob[1], sizeof(OBJECT));
	  objc_add(tree, ROOT, DROOT+ii);
	} /* for */
} /* obj_init */

/*
*	Allocate a window object from the screen tree by looking for 
*	the child of the parent with no size
*/
	WORD 
obj_walloc(x, y, w, h)
	WORD		x, y, w, h;
{
	WORD		ii;

	for (ii = DROOT; ii < (NUM_WNODES+1); ii++)
	{
	  if ( !(G.g_screen[ii+1].ob_width && G.g_screen[ii+1].ob_height) )
	    break;
	}
	if ( ii < (NUM_WNODES+1) )
	{
	  r_set((GRECT *)&G.g_screen[ii+1].ob_x, x, y, w, h);
	  return(ii+1);
	}
	else
	  return(0);
} /* obj_walloc */

/*
*	Free a window object by changing its size to zero and
*	NILing out all its children.
*/
	VOID 
obj_wfree(obj, x, y, w, h)
	WORD		obj;
{
	WORD		ii, nxtob;

	r_set((GRECT *)&G.g_screen[obj].ob_x, x, y, w, h);
	for (ii = G.g_screen[obj].ob_head; ii > obj; ii = nxtob)
	{
	  nxtob = G.g_screen[ii].ob_next;
	  G.g_screen[ii].ob_next = NIL;
	}
	G.g_screen[obj].ob_head = G.g_screen[obj].ob_tail = NIL;
} /* obj_wfree */

/*
*	Routine to find and allocate a particular item object.  The
*	next free object is found by looking for any object that
*	is available (i.e., a next pointer of NIL).
*/
	WORD 
obj_ialloc(wparent, x, y, w, h)
	WORD		wparent;
	WORD		x, y, w, h;
{
	WORD		ii;

	for (ii = NUM_WNODES+2; ii < NUM_SOBS; ii++)
	{
	  if (G.g_screen[ii].ob_next == NIL)
	    break;
	}
	if (ii < NUM_SOBS)
	{
	  G.g_screen[ii].ob_head = G.g_screen[ii].ob_tail = NIL;
	  objc_add(G.a_screen, wparent, ii);
	  r_set((GRECT *)&G.g_screen[ii].ob_x, x, y, w, h);
	  return(ii);
	}
	else
	  return(0);
} /* obj_ialloc */
