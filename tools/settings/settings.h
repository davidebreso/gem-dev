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

#include "ppdgem.h"
#include "gemset.h"			// Resources
#include "popmenu.h"
#include "popcolou.h"
#include <dos.h>
#include <string.h>

/* settings.c */
extern WORD work_out[];

#define CC_MAX 24


/* clrdlg.c */
VOID use_colset(CLRCAT *cc);
VOID set_colours(LPTREE tr, WORD o);
VOID write_colours(CLRCAT *cc);

/* guidlg.c */
VOID set_options(LPTREE tr, WORD o);

/* windlg.c */

typedef struct winobj
{
	LONG ob_spec;
	WORD ob_flags;
	WORD ob_type;
} WINOBJ;

#define NUM_WINOBJ 19

VOID set_windows(LPTREE tr, WORD o);
VOID get_specs(WINOBJ *o);
VOID set_specs(WINOBJ *o);
VOID write_specs(WINOBJ *o);

/* theme.c */

VOID load_theme(VOID);
VOID save_theme(VOID);

/* setutil.c */

VOID filled_rect(GRECT *area, WORD fg, WORD bg, WORD style, WORD index);
VOID draw_rect(GRECT *area);	/* Draw a rectangle round an area */
WORD contrast(WORD col);
WORD rsrc_alert(WORD defbtn, WORD res);
VOID send_redraw(WORD wh, GRECT *p);


extern X_BUF_V2 gl_xbuf;
extern WORD vdi_handle;

