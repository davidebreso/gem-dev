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

extern WORD gl_apid;







WORD rsrc_alert(WORD defbtn, WORD res)
{
	LPBYTE pa;
	
	rsrc_gaddr(R_STRING, res, (LPVOID *)&pa);

	return form_alert(defbtn, pa);
}


VOID send_redraw(WORD wh, GRECT *p)
	{
	WORD	msg[8];

	msg[0] = WM_REDRAW;		/* Defined in GEMBIND.H	    */
	msg[1] = gl_apid;		/* As returned by appl_init */
	msg[2] = 0;
	msg[3] = wh;			/* Handle of window to redraw */
	msg[4] = p->g_x;
	msg[5] = p->g_y;
	msg[6] = p->g_w;
	msg[7] = p->g_h;
	appl_write(gl_apid, 16, ADDR(msg));	/* Use ADDR(msg) for portability */
	}

