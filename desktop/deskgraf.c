/*	DESKGRAF.C	04/11/84 - 03/17/85	Lee Lorenzen		*/
/*	merge source	5/27/87			mdf			*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "wccdesk.h"

#define ORGADDR 0x0L

/*
#define vsf_interior( x )	gsx_1code(S_FILL_STYLE, x)
#define vsl_type( x )		gsx_1code(S_LINE_TYPE, x)
#define vsf_style( x )		gsx_1code(S_FILL_INDEX, x)
#define vsf_color( x )		gsx_1code(S_FILL_COLOR, x)
#define vsl_udsty( x )		gsx_1code(ST_UD_LINE_STYLE, x)

EXTERN VOID	r_get();
EXTERN WORD	vst_clip();
EXTERN VOID	r_set();
EXTERN VOID	gsx2();
EXTERN VOID	gsx_moff();
EXTERN VOID	vro_cpyfm();
EXTERN VOID	vrt_cpyfm();
EXTERN VOID	gsx_mon();
EXTERN VOID	vrn_trnfm();
EXTERN VOID	gsx_1code();

GLOBAL WORD	gl_width;
GLOBAL WORD	gl_height;


GLOBAL WORD	gl_wchar;
GLOBAL WORD	gl_hchar;

GLOBAL WORD	gl_wschar;
GLOBAL WORD	gl_hschar;

GLOBAL WORD	gl_wptschar;
GLOBAL WORD	gl_hptschar;

GLOBAL WORD	gl_wbox;
GLOBAL WORD	gl_hbox;


GLOBAL WORD 	gl_nplanes;
GLOBAL WORD 	gl_handle;


GLOBAL WS	gl_ws;
GLOBAL WORD	contrl[12];
GLOBAL WORD	intin[128];
GLOBAL WORD	ptsin[20];
GLOBAL WORD	intout[10];
GLOBAL WORD	ptsout[10];

GLOBAL LONG	ad_intin;

GLOBAL WORD	gl_mode;
GLOBAL WORD	gl_mask;
GLOBAL WORD	gl_tcolor;
GLOBAL WORD	gl_lcolor;
GLOBAL WORD	gl_fis;
GLOBAL WORD	gl_patt;
GLOBAL WORD	gl_font;

*/


/*
*	Routine to set the clip rectangle.  If the w,h of the clip
*	is 0, then no clip should be set.  Ohterwise, set the 
*	appropriate clip.
*/
WORD  gsx_sclip(GRECT *pt)
{
	WORD ptsin[4];
	
	r_get(pt, &gl_xclip, &gl_yclip, &gl_wclip, &gl_hclip);

	if ( gl_wclip && gl_hclip )
	{
	  ptsin[0] = gl_xclip;
	  ptsin[1] = gl_yclip;
	  ptsin[2] = gl_xclip + gl_wclip - 1;
	  ptsin[3] = gl_yclip + gl_hclip - 1;
	  vs_clip( gl_handle, TRUE, &ptsin[0]);
	}
	else
	  vs_clip( gl_handle, FALSE, &ptsin[0]);
	return( TRUE );
}

/*
*	Routine to get the current clip setting
*/
VOID  gsx_gclip(GRECT *pt)
{
	r_set(pt, gl_xclip, gl_yclip, gl_wclip, gl_hclip);
}


MLOCAL VOID  gsx_xline( WORD ptscount, WORD *ppoints )
{
	static	UWORD	hztltbl[2] = { 0x5555, 0xaaaa };
	static  UWORD	verttbl[4] = { 0x5555, 0xaaaa, 0xaaaa, 0x5555 };
	WORD		*linexy,i;
	WORD		st;

	for ( i = 1; i < ptscount; i++ )
	{
	  if ( *ppoints == *(ppoints + 2) )
	  {
	    st = verttbl[( (( *ppoints) & 1) | ((*(ppoints + 1) & 1 ) << 1))];
	  }	
	  else
	  {
	    if ( ppoints[0] < ppoints[2] ) linexy = ppoints;
	    else linexy = ppoints + 2;
	    st = hztltbl[( *(linexy + 1) & 1)];
	  }
	  vsl_udsty(gl_handle, st );
	  v_pline( gl_handle, 2, ppoints );
	  ppoints += 2;
	}
	vsl_udsty( gl_handle,-1);
}	

/*
*	Routine to draw a certain number of points in a polyline
*	relative to a given x,y offset.
*/
VOID  gsx_pline(WORD offx, WORD offy, WORD cnt, WORD *pts)
{
	WORD		i, j, lcnt;
	WORD 		ptsin[80];

	/* If there are more points than would fit in the buffer,
	 * then call gsx_xline() repeatedly with 40 at a time.
	 */

	do
	{
		lcnt = min(cnt, 40);
	
		for (i=0; i<cnt; i++)
		{
			j = i * 2;
	  		ptsin[j] = offx + pts[j];
	  		ptsin[j+1] = offy + pts[j+1];
		}
		gsx_xline( cnt, &ptsin[0]);
		pts += lcnt;
		cnt -= lcnt;
	} while(lcnt);
		
}

/*
*	Routine to set the text, writing mode, and color attributes.
*/
VOID  gsx_attr(UWORD text, UWORD mode, UWORD color)
{
	if (mode != gl_mode) vswr_mode(gl_handle, gl_mode = mode);
	if (text)
	{
	  if (color != gl_tcolor) vst_color(gl_handle, gl_tcolor = color);
	}	
	else
	{
	  if (color != gl_lcolor) vsl_color(gl_handle, gl_lcolor = color);
	}
}

/*
*	Routine to fix up the MFDB of a particular raster form
*/
VOID  gsx_fix(MFDB *pfd, LPVOID theaddr, WORD wb, WORD h)
{
	if (theaddr == ORGADDR)
	{
	  pfd->fwp = gl_workout[0] + 1;
	  pfd->fww = pfd->fwp / 16;
	  pfd->fh  = gl_workout[1] + 1;
	  pfd->np  = gl_nplanes;
	}
	else
	{
	  pfd->fww = wb / 2;
	  pfd->fwp = wb * 8;
	  pfd->fh  = h;
	  pfd->np  = 1;
	}
	pfd->ff = FALSE;
	pfd->mp = theaddr;
}

/*
*	Routine to blit, to and from a specific area
*/
MLOCAL VOID  gsx_blt(LPVOID saddr, UWORD sx, UWORD sy, UWORD swb, 
			 LPVOID daddr, UWORD dx, UWORD dy, UWORD dwb, 
			 UWORD w, UWORD h, UWORD rule, 
			 WORD fgcolor, WORD bgcolor)
{
	WORD ptsin[8];
	WORD index[2];
	
	gsx_fix(&gl_src, saddr, swb, h);
	gsx_fix(&gl_dst, daddr, dwb, h);

	gsx_moff();
	ptsin[0] = sx;
	ptsin[1] = sy;
	ptsin[2] = sx + w - 1;
	ptsin[3] = sy + h - 1;
	ptsin[4] = dx;
	ptsin[5] = dy;
	ptsin[6] = dx + w - 1;
	ptsin[7] = dy + h - 1 ;
	if (fgcolor == -1)
	  vro_cpyfm( gl_handle, rule, &ptsin[0], &gl_src, &gl_dst);
	else
	{
		index[0] = fgcolor;
		index[1] = bgcolor;
		vrt_cpyfm( gl_handle, rule, &ptsin[0], &gl_src, &gl_dst, index);
	
	}
	gsx_mon();
}

/*
*	Routine to blit around something on the screen
*/
VOID  bb_screen(WORD scrule, WORD scsx, WORD scsy, 
				WORD scdx, WORD scdy, WORD scw, WORD sch)
{
	gsx_blt(0x0L, scsx, scsy, 0, 
		0x0L, scdx, scdy, 0,
		scw, sch, scrule, -1, -1);
}

/*
*	Routine to transform a standard form to device specific
*	form.
*/
VOID  gsx_trans(LPVOID saddr, UWORD swb, LPVOID daddr, UWORD dwb, UWORD h)
{
	gsx_fix(&gl_src, saddr, swb, h);
	gl_src.ff = TRUE;
	gl_src.np = 1;

	gsx_fix(&gl_dst, daddr, dwb, h);
	vr_trnfm( gl_handle, &gl_src, &gl_dst );
}

/*
*	Routine to initialize all the global variables dealing
*	with a particular workstation open
*/
VOID  gsx_start()
{
	WORD		char_height, nc;

	gl_xclip = 0;
	gl_yclip = 0;
	gl_width = gl_wclip  = gl_workout[0] + 1;
	gl_height = gl_hclip = gl_workout[1] + 1;

	nc = gl_workout[13];
	gl_nplanes = 0;
	while (nc != 1)
	{
	  nc >>= 1;
	  gl_nplanes++;
	}
	char_height = gl_workout[0x2E];
	vst_height( gl_handle, char_height, &gl_wptschar, &gl_hptschar, 
				&gl_wschar, &gl_hschar );
	char_height = gl_workout[0x30];
	vst_height( gl_handle, char_height, &gl_wptschar, &gl_hptschar, 
				&gl_wchar, &gl_hchar );
	gl_ncols = gl_width / gl_wchar;
	gl_nrows = gl_height / gl_hchar;
	gl_hbox = gl_hchar + 3;
	gl_wbox = (gl_hbox * gl_workout[3]) / gl_workout[4];
	vsl_type ( gl_handle, 7 );
	vsl_width( gl_handle, 1 );
	vsl_udsty( gl_handle, -1 );
	r_set(&gl_rscreen, 0, 0, gl_width, gl_height);
	r_set(&gl_rfull, 0, gl_hbox, gl_width, (gl_height - gl_hbox));
	r_set(&gl_rzero, 0, 0, 0, 0);
	r_set(&gl_rcenter, (gl_width-gl_wbox)/2, (gl_height-(2*gl_hbox))/2, 
			gl_wbox, gl_hbox);
	r_set(&gl_rmenu, 0, 0, gl_width, gl_hbox);
}

VOID  gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc, BYTE *txt)
{
	WORD		pts_height;

	if (tb_f == IBM)
	{
	  if (tb_f != gl_font)
	  {
	    pts_height = gl_workout[0x30];
	    vst_height( gl_handle, pts_height, &gl_wptschar, &gl_hptschar, 
				&gl_wchar, &gl_hchar );
	    gl_font = tb_f;
	  }
	  y += gl_hptschar;
	}

	v_gtext(gl_handle, x, y, txt);
}

/*
*	Routine to do a filled bit blit, (a rectangle).
*/
VOID  bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, WORD hy, WORD hw, WORD hh)
{
	WORD ptsin[4];
	
	gsx_fix(&gl_dst, 0x0L, 0, 0);
	ptsin[0] = hx;
	ptsin[1] = hy;
	ptsin[2] = hx + hw - 1;
	ptsin[3] = hy + hh - 1;

	gsx_attr(TRUE, mode, gl_tcolor);
	if (fis != gl_fis)
	{
	  vsf_interior( gl_handle, fis);
	  gl_fis = fis;
	}
	if (patt != gl_patt)
	{
	  vsf_style( gl_handle, patt );
	  gl_patt = patt;
	}
	vr_recfl( gl_handle, &ptsin[0]/* why?, &gl_dst*/ );
}

