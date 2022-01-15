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
#include	<ctype.h>

extern WORD maybe_3d[];


VOID get_path(BYTE *tmp_path, BYTE *spec)	/* get directory path name		*/
{
	WORD	cur_drv;

	cur_drv     = dos_gdrv();
	tmp_path[0] = cur_drv + 'A';
	tmp_path[1] = ':';
	tmp_path[2] = '\\';
	dos_gdir(cur_drv+1, ADDR(&tmp_path[3]));
	if (strlen(tmp_path) > 3) strcat(tmp_path, "\\");
	else                      tmp_path[2] = '\0';

	strcat(tmp_path, spec);
}


VOID add_file_name(BYTE *dname, BYTE *fname)
{
	BYTE	c;
	WORD	ii;

	ii = strlen(dname);
	while (ii && (((c = dname[ii-1])  != '\\') && (c != ':')))
		ii--;
	dname[ii] = '\0';
	strcat(dname, fname);
}



FILE *get_file(BOOLEAN loop, BYTE *file_name, BYTE *mask)		
{
	WORD	fs_iexbutton;
	BYTE	fs_iinsel[13];
	FILE	*fp;
	
	while (TRUE)
	{
		get_path(file_name, mask);
		fs_iinsel[0] = '\0'; 

		fsel_input(ADDR(file_name), ADDR(fs_iinsel), &fs_iexbutton);
		if (fs_iexbutton)
		{
			add_file_name(file_name, fs_iinsel);
			fp = fopen(file_name, "r+");
			if (fp == NULL) rsrc_alert(1, BADOPEN);
			if (!loop || (loop && fp != NULL))
				return(fp);
		}
		else   
		{
			return((FILE *)NULL);   
		}
	}
	return ((FILE *)NULL);
} /* get_file */


FILE *get_wfile(BOOLEAN loop, BYTE *file_name, BYTE *mask)		
{
	WORD	fs_iexbutton;
	BYTE	fs_iinsel[13];
	FILE	*fp;
	
	while (TRUE)
	{
		get_path(file_name, mask);
		fs_iinsel[0] = '\0'; 

		fsel_input(ADDR(file_name), ADDR(fs_iinsel), &fs_iexbutton);
		if (fs_iexbutton)
		{
			add_file_name(file_name, fs_iinsel);
			fp = fopen(file_name, "w");
			if (!fp) rsrc_alert(1, BADOPEN);

			if (!loop || (loop && fp != NULL))
				return(fp);
		}
		else   
		{
			return((FILE *)NULL);   
		}
	}
	return ((FILE *)NULL);
} /* get_file */




MLOCAL WINOBJ gemwin[NUM_WINOBJ];
MLOCAL CLRCAT gemcc[CC_MAX];

WORD hexdgt(char **s)
{
	char c;
	
	while (**s == ' ') (*s)++;

	c = toupper(**s);
	
	if      (c >= 'A' && c <= 'F') { (*s)++; return c - 'A' + 10; }
	else if (c >= '0' && c <= '9') { (*s)++; return c - '0';      }
	else return -1;
}


VOID hex(char **s, WORD *val, WORD ndig)
{
	UWORD v, w;
	WORD n;

	for (v = n = 0; n < ndig; n++)
	{
		v = v << 4;
		w = hexdgt(s); 

		if (w == -1) return;	/* Invalid hex digit */

		v |= w;
	}
	*val = v;
}



VOID load_theme(VOID)
{
	FILE	*fp;
	char	filename[160];
	char    buf[160], *psrc;
	WORD 	idx;
	WORD 	win2d = 0;
	GRECT 	gr_full;
	WORD	opts[5];

	for (idx = 0; idx < 5; idx++) opts[idx] = prop_gui_get(idx);
	
	get_specs(gemwin);
	LBCOPY(ADDR(gemcc), ADDR(gl_xbuf.cc), CC_MAX * sizeof(CLRCAT));

	fp = get_file(TRUE, filename, "*.CFG"); //"*.GTM");

	if (!fp) return;

	while (fgets(buf, 159, fp))
	{
		if (!strnicmp("GEM.AES.Colour.", buf, 15))
		{
			/* 0-15 = "GEM.AES.Colour." 16=no. 17=colon 18=space */
			psrc = buf + 15;

			if      (psrc[0] <= '9' && psrc[0] >= '0') idx = psrc[0] - '0';
			else if (psrc[0] <= 'N' && psrc[0] >= 'A') idx = psrc[0] - 'A' + 10;
			else if (psrc[0] <= 'n' && psrc[0] >= 'a') idx = psrc[0] - 'a' + 10;

			++psrc;
			if (idx >= 0 && idx < CC_MAX && psrc[0] == ':')
			{
				++psrc;	/* Past the colon */
				hex(&psrc, &gemcc[idx].cc_foreground, 4);				
				hex(&psrc, &gemcc[idx].cc_background, 4);				
				hex(&psrc, &gemcc[idx].cc_style, 4);				
				hex(&psrc, &gemcc[idx].cc_pattern, 4);				
			}
		}

		if (!strnicmp("GEM.AES.Button.", buf, 15))
		{
			/* 0-15 = "GEM.AES.Button." 16=no. 17=colon 18=space */
			psrc = buf + 15;

			if      (psrc[0] <= '9' && psrc[0] >= '0') idx = psrc[0] - '0';
			else if (psrc[0] <= 'G' && psrc[0] >= 'A') idx = psrc[0] - 'A' + 10;
			else if (psrc[0] <= 'g' && psrc[0] >= 'a') idx = psrc[0] - 'a' + 10;
			else idx = -1;
			++psrc;

			if (idx >= 0 && psrc[0] == ':')
			{
				WORD wVal = (gemwin[idx].ob_spec >> 24) & 0xFF;
				LONG lVal = gemwin[idx].ob_spec & 0xFFFFFFL;
				LONG lNew;
				
				++psrc;	/* Past the colon */
				hex(&psrc, &wVal, 2);
				lNew = wVal;
				lNew = (lNew << 24) | lVal;

				gemwin[idx].ob_spec = lNew;
			}
		}
		if (!strnicmp("GEM.AES.Win2D",    buf, 13)) win2d = 1;
		/* Back compatibility */
		if (!strnicmp("GEM.AES.Square3D", buf, 16)) opts[PROP_GUI_SQUARE3D] = 0;
		if (!strnicmp("GEM.AES.Option.",  buf, 15))
		{
			psrc = buf + 15;

			if      (psrc[0] <= '9' && psrc[0] >= '0') idx = psrc[0] - '0';
			else idx = -1;
			++psrc;
			
			if (idx >= 0 && psrc[0] == ':')
			{
				WORD wVal;
				
				++psrc;	/* Past the colon */
				hex(&psrc, &wVal, 4);

				opts[idx] = wVal;
			}

		}
	}
	fclose(fp);

	if (win2d) gemwin[maybe_3d[0]].ob_flags &= ~FLAG3D;
	else       gemwin[maybe_3d[0]].ob_flags |= FLAG3D;

	write_specs(gemwin);
	write_colours(gemcc);

	gr_full.g_x = 0;
	gr_full.g_y = 0;
	gr_full.g_w = work_out[0];
	gr_full.g_h = work_out[1];

	prop_gui_set(PROP_GUI_SQUARE3D, opts[PROP_GUI_SQUARE3D]);
	prop_gui_set(PROP_GUI_FRAME3D,  opts[PROP_GUI_FRAME3D]);
	prop_gui_set(PROP_GUI_MENU,     opts[PROP_GUI_MENU]);

	/* Write option settings to GEM.CFG */
	sprintf(buf, "%04x", prop_gui_get(PROP_GUI_SQUARE3D)); 
	prop_put(ADDR("GEM.AES"), ADDR("Option.1"), ADDR(buf), 0);
	sprintf(buf, "%04x", prop_gui_get(PROP_GUI_FRAME3D)); 
	prop_put(ADDR("GEM.AES"), ADDR("Option.2"), ADDR(buf), 0);
	sprintf(buf, "%04x", prop_gui_get(PROP_GUI_MENU)); 
	prop_put(ADDR("GEM.AES"), ADDR("Option.4"), ADDR(buf), 0);


	
	send_redraw(0, &gr_full);		
	
	
}


VOID save_theme(VOID)
{
	WORD win2d, round;
	FILE	*fp;
	char	filename[160], cidx;
	WORD	idx, xo;
	char 	buf[2];
	
	if (prop_get(ADDR("GEM.AES"), ADDR("Square3D"), ADDR(buf), 2, 0)) round = 1;
	else round = 0;
	
	get_specs(gemwin);
	LBCOPY(ADDR(gemcc), ADDR(gl_xbuf.cc), CC_MAX * sizeof(CLRCAT));

	if (gemwin[2].ob_flags & FLAG3D) win2d = 0; else win2d = 1;

	fp = get_wfile(TRUE, filename, "*.CFG");

	if (!fp) return;
	
	for (idx = CC_NAME; idx < CC_MAX; idx++) if (idx != 15)
	{
		if (idx < 10) cidx = idx + '0'; else cidx = idx + 'A' - 10;
		
		fprintf(fp,  "GEM.AES.Colour.%c: %04x%04x%04x%04x\n", 
						 cidx,
		                 gemcc[idx].cc_foreground,
		                 gemcc[idx].cc_background,
		                 gemcc[idx].cc_style,
		                 gemcc[idx].cc_pattern);
	}
	for (xo = 0; xo < NUM_WINOBJ; xo++) if (gemwin[xo].ob_type == G_BOXCHAR)
	{
		fprintf(fp, "GEM.AES.Button.%c: ", (xo < 10) ? (xo + '0') : (xo + '7'));
		fprintf(fp, "%02x\n", (WORD)(((gemwin[xo].ob_spec) >> 24) & 0xFF));
	}		
	fprintf(fp, "GEM.AES.Option.1: %04x\n", prop_gui_get(PROP_GUI_SQUARE3D));
	fprintf(fp, "GEM.AES.Option.2: %04x\n", prop_gui_get(PROP_GUI_FRAME3D));
	fprintf(fp, "GEM.AES.Option.4: %04x\n", prop_gui_get(PROP_GUI_MENU));
	
	if (win2d)  fprintf(fp, "GEM.AES.Win2D: 1\n");
	fclose(fp);
}


