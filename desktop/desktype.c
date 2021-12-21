/****************************************************************************
*   Copyright 1999, Caldera Thin Client Systems, Inc.                       *
*   This software is licensed under the GNU Public License.                 *
*   See LICENSE.TXT for further information.                                *
*                                                                           *
*   Historical Copyright                                                    *
*                                                                           *
*   Copyright (c) 1985,1991,1992 Digital Research Inc.			    *
*   All rights reserved.						    *
*   The Software Code contained in this listing is proprietary to Digital   *
*   Research Inc., Monterey, California, and is covered by U.S. and other   *
*   copyright protection.  Unauthorized copying, adaption, distribution,    *
*   use or display is prohibited and may be subject to civil and criminal   *
*   penalties.  Disclosure to others is prohibited.  For the terms and      *
*   conditions of software code use, refer to the appropriate Digital       *
*   Research License Agreement.						    *
*****************************************************************************
*		      U.S. GOVERNMENT RESTRICTED RIGHTS			    *
*                    ---------------------------------                      *
*  This software product is provided with RESTRICTED RIGHTS.  Use, 	    *
*  duplication or disclosure by the Government is subject to restrictions   *
*  as set forth in FAR 52.227-19 (c) (2) (June, 1987) when applicable or    *
*  the applicable provisions of the DOD FAR supplement 252.227-7013 	    *
*  subdivision (b)(3)(ii) (May 1981) or subdivision (c)(1)(ii) (May 1987).  *
*  Contractor/manufacturer is Digital Research Inc. / 70 Garden Court /     *
*  BOX DRI / Monterey, CA 93940.					    *
*****************************************************************************
 *
 * Backported to GEM/3 desktop and Pacific C by John Elliott, 6 August 2005
 * 
 * ViewMAX history: 
 *
 * $Header: m:/davinci/users//groups/panther/dsk/rcs/desktype.c 4.6 92/04/06 09:44:01 Fontes Stable $
* $Log:	desktype.c $
 * Revision 4.6  92/04/06  09:44:01  Fontes
 * Initial cut to extract/display Windows exe embedded icons.
 * 
 * Revision 4.5  92/04/03  17:15:53  sbc
 * WNODEs and PNODEs to fars, lots of other housekeeping
 * 
 * Revision 4.4  92/03/26  14:44:50  sbc
 * WNODEs and PNODEs to far ptrs. Also merge in RSF's changes
 * 
 * Revision 4.3  92/03/13  14:42:03  sbc
 * Merge in Keiko's changes required for Double Byte Character Support
 * 
 * Revision 4.2  92/02/06  11:56:13  sbc
 * add parenthesis to WM_HSLID case to make it compile.
 * This "fix" makes no sense..... sigh....
 * 
 * Revision 4.1  91/09/17  13:57:17  system
 * Initial work for DOS window, screen saver, etc.
 * 
 * Revision 3.1  91/08/19  16:38:53  system
 * ViewMAX 2 sources
 * 
* Date    Who  SPR#	Comments
* ------- ---  ----	---------------------------------------------------
* 911122  K.H		Fix the caluclation of end_column for redrawing.
* 911112  K.H		Fix window x-coordinate was not aligned correctly.
* 911107  K.H		Convert the dbcs second byte character where a string
*			begins to space character. (#if DBCS)
* 911105  K.H		Fix vr_recfl erased the end pixel of the string.
*****************************************************************************/

#include "wccdesk.h"

		    /* Bytes per line when displaying file in hexadecimal */
#define BYTES_PER_HEX_LINE  16

#define HEX_LINE_LENGTH (6 + 2 + BYTES_PER_HEX_LINE * 3 + 1 + BYTES_PER_HEX_LINE)
                        /* Line length when displaying file in hexadecimal */
#define MAX_STR_LEN 128                 /* Maximum text string length */

/* Alignments for vst_alignment */
#define ALI_LEFT 0
#define ALI_BASE 0
#define ALI_TOP  5

#define MAXFILE 16
/* external data */

extern GLOBES G;
extern WORD DOS_ERR;
extern WORD gl_handle, gl_wchar, gl_hchar, gl_apid;
extern BYTE ILL_TYPE[];

typedef signed long SLONG;

#if (defined(__SMALL__) && defined(MULTIAPP))
WORD  do_type(WORD curr)
{
	form_alert(0, "[3][This function is not available in the |"
			"small-model desktop][ Cancel ]");
	return 0;
}

#else

/* Local data */

MLOCAL BYTE	GOOD_TYPE[] = { CLOSITEM, 0 };
MLOCAL WORD max_str_len;                /* Maximum text string length */
MLOCAL GRECT work_area;                 /* Work area of view window */
MLOCAL BYTE info_line[81];              /* Window information line */

MLOCAL WORD file_handle;                /* File handle */
MLOCAL BOOLEAN file_error;              /* True if error occurs reading file */
MLOCAL UBYTE far *file_buffer;         /* Buffer to read file into */
MLOCAL WORD file_buffer_size;           /* Size of file_buffer */
MLOCAL UBYTE far *file_buffer_end;     /* Points to byte following last byte
                                           in file_buffer */
MLOCAL BOOLEAN show_as_text;            /* True if file appears to be text */
MLOCAL BOOLEAN scanning_file;           /* True if scanning file for file size
                                           and text line length and number of
                                           text lines */
MLOCAL SLONG file_size;                 /* Number of bytes in file */
MLOCAL WORD max_text_line_length;       /* Maximum text line length in file */
MLOCAL WORD max_line_length;            /* Maximum line length in file as
                                           displayed */
MLOCAL SLONG num_text_lines;            /* Number of text lines in file */
MLOCAL SLONG num_lines;                 /* Number of lines in file as
                                           displayed */
MLOCAL SLONG num_words;                 /* Number of words in file */
MLOCAL WORD scan_column;                /* Current column when scanning file */
MLOCAL BOOLEAN scan_in_word;            /* True if scan is within word */
MLOCAL SLONG window_offset;             /* File offset corresponding to start of
                                           window */
MLOCAL WORD window_column;              /* Number of first column displayed
                                           in window, 0 if at start of line
                                           in file */
MLOCAL SLONG window_row;                /* Number of first row displayed in
                                           window, 0 if first line in file */
MLOCAL BOOLEAN quit_flag;               /* Set true when quit selected */
MLOCAL WORD return_value;       /* Set with error code or TRUE to terminate program */

MLOCAL void  handle_key(UWORD key, WORD wh);

#if DBCS
extern void cvt_dbc2( UBYTE *, UBYTE * ) ;		/* deskinf.c */
#endif /* DBCS */



/*-----------------------------------------------------------------------------
Purpose : Set horizontal slider position and size
Entry   : wh    Window handle
Exit    : 
*/
MLOCAL void  set_hor_slider(WORD wh)
{
    WORD hslide, hslsize;

    if (work_area.g_w / gl_wchar >= max_line_length) {
        hslide = 1;
        hslsize = 1000;
    } else {
        hslide = 1 + (WORD) (((SLONG) window_column * 999)
            / (SLONG) (max_line_length - work_area.g_w / gl_wchar));
        hslsize = 1 + (WORD) (((SLONG) (work_area.g_w / gl_wchar) * 999)
            / (SLONG) max_line_length);
    }
    wind_set(wh, WF_HSLIDE, hslide, 0, 0, 0);
    wind_set(wh, WF_HSLSIZ, hslsize, 0, 0, 0);
} /* set_hor_slider() */

/*-----------------------------------------------------------------------------
Purpose : Set vertical slider position and size
Entry   : wh    Window handle
Exit    : 
*/
MLOCAL void  set_vert_slider(WORD wh)
{
    WORD vslide, vslsize;

    if ((SLONG) (work_area.g_h / gl_hchar) >= num_lines) {
        vslide = 1;
        vslsize = 1000;
    } else {
        vslide = 1 + (WORD) ((window_row * 999)
            / (num_lines - (SLONG) (work_area.g_h / gl_hchar)));
        vslsize = 1 + (WORD) (((SLONG) (work_area.g_h / gl_hchar) * 999)
            / num_lines);
    }
    wind_set(wh, WF_VSLIDE, vslide, 0, 0, 0);
    wind_set(wh, WF_VSLSIZ, vslsize, 0, 0, 0);
} /* set_vert_slider() */

/*-----------------------------------------------------------------------------
Purpose : Set slider positions and sizes
Entry   : wh    Window handle
Exit    : 
*/
MLOCAL void set_sliders(WORD wh)
{
    set_hor_slider(wh);
    set_vert_slider(wh);
} /* set_sliders() */

/*-----------------------------------------------------------------------------
Purpose : Seek and read from file
Entry   : 
Exit    : Returns zero if successful
*/
MLOCAL BOOLEAN  seek_read(void)
{
    if (dos_lseek(file_handle, 0, window_offset) == window_offset)
        file_buffer_end = file_buffer + 
	    dos_read( file_handle, file_buffer_size, file_buffer);
    else
        file_error = TRUE;
    return(file_error);
} /* seek_read() */

/*-----------------------------------------------------------------------------
Purpose : Search file for given row
Entry   : required_row  Required row
Exit    : 
*/
MLOCAL void seek_row(SLONG required_row)
{
    BOOLEAN reread_required;
    UBYTE far *s;

    reread_required = TRUE;
    if (show_as_text) {
        graf_mouse(HOURGLASS, 0L);

    /* Start search from beginning of file if required row nearer start than
       current row */

        if (required_row < window_row / 2) {
            window_row = 0;
            window_offset = 0;
            seek_read();
        }
        while (window_row < required_row && !file_error
                && file_buffer_end > file_buffer) {
            s = file_buffer;
            while (window_row < required_row && s < file_buffer_end - 1) {
                while (*s != '\r' && *s != '\n' && s < file_buffer_end - 1)
                    s++;
                if (s < file_buffer_end - 1) {
                    if (*s == '\r' && *(s + 1) == '\n')
                        s++;
                    s++;
                    window_row++;
                }
            }
            window_offset += (s - file_buffer);
            seek_read();
            reread_required = FALSE;
        }
        while (window_row > required_row && !file_error) {
            s = file_buffer + file_buffer_size;
            if ((window_offset -= file_buffer_size) < 0) {
                s = (UBYTE far*)((SLONG)s + window_offset);
                window_offset = 0;
            }
            if (seek_read() == 0) {
                while (window_row > required_row && s > file_buffer + 1) {
                    s--;
                    if (*s == '\n' && *(s - 1) == '\r')
                        s--;
                    if (s > file_buffer) {
                        do {
                            s--;
                        } while (*s != '\r' && *s != '\n' && s > file_buffer);
                    }
                    if (*s == '\r' || *s == '\n') {
                        s++;
                        window_row--;
                    }
                }
                if ((window_offset += (s - file_buffer)) == 0)
                    window_row = 0;
            }
        }
        graf_mouse(ARROW, 0L);
    } else {
        window_row = required_row;
        window_offset = window_row * BYTES_PER_HEX_LINE;
    }
    if (reread_required)
        seek_read();
} /* seek_row() */

/*----------------------------------------------------------------------------
Purpose : Set information line
Entry   : wh    Window handle
Exit    : 
*/
MLOCAL void  set_info_line(WORD wh)
{
    if (scanning_file) {
        strlcpy( info_line, ini_str(STFSCAN), sizeof(info_line) );
	
    } else {
	if ( show_as_text ) {
	    sprintf( info_line, ini_str( STFWORDS ),
				file_size, num_text_lines, num_words ) ;
	   }
	else {
	    sprintf( info_line, ini_str( STFBYTES ), file_size ) ;
	   }
    }
    wind_setl(wh, WF_INFO, info_line);
    
} /* set_info_line() */

/*-----------------------------------------------------------------------------
Purpose : Redraw all or part of window
Entry   : wh            Window handle
          area          Area to be redrawn
Exit    : 
*/
MLOCAL void do_redraw(WORD wh, GRECT *area)
{
    BOOLEAN end_of_line;
    UBYTE c;
    WORD i, j;
    WORD column, row;
    WORD start_column, start_row, end_column, end_row;
    WORD string_length;
    WORD hor_alignment, vert_alignment;
    UBYTE *s, *t;
    UBYTE far *buffer_ptr;
    UBYTE text_buffer[MAX_STR_LEN + 1];
    WORD xy_array[4];
    GRECT box, dirty_dest;
#if DBCS
    UBYTE far *slptr, far *p;
    BYTE dbc1;
#endif /* DBCS */

    graf_mouse(M_OFF, 0L);

    wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
    while (box.g_w != 0 && box.g_h != 0) {
        if (rc_intersect(area, &box)) {
            rc_copy(&box, &dirty_dest);
            if (rc_intersect(&work_area, &dirty_dest)) {

            /* Calculate dirty source in text units */

                start_column = (dirty_dest.g_x - work_area.g_x) / gl_wchar
                    + window_column;
                start_row = (dirty_dest.g_y - work_area.g_y) / gl_hchar;
                end_column
                    = (dirty_dest.g_x - work_area.g_x + dirty_dest.g_w - 1
                        + 1 + gl_wchar - 1) / gl_wchar + window_column;
                end_row = (dirty_dest.g_y - work_area.g_y + dirty_dest.g_h - 1
                        + 1 + gl_hchar - 1) / gl_hchar;

            /* Set attributes */

                vst_alignment(gl_handle, ALI_LEFT, ALI_TOP, &hor_alignment,
                    &vert_alignment);   /* Set text alignment to top left */
                vsf_interior(gl_handle, FIS_HOLLOW);	/* Set interior pattern */
		vswr_mode( gl_handle, MD_REPLACE );	/* Set Replace-mode	*/
		
            /* Set clipping rectangle */

                xy_array[0] = dirty_dest.g_x;
                xy_array[1] = dirty_dest.g_y;
                xy_array[2] = dirty_dest.g_x + dirty_dest.g_w - 1;
                xy_array[3] = dirty_dest.g_y + dirty_dest.g_h - 1;
                vs_clip(gl_handle, 1, xy_array);    /* Enable clipping */

                buffer_ptr = file_buffer;
                row = 0;
                while (row < end_row) {

                    end_of_line = FALSE;

                /* Point to start of start row */

                    if (show_as_text) {
                        while (row < start_row
                                && buffer_ptr < file_buffer_end) {
                            while (*buffer_ptr != '\r' && *buffer_ptr != '\n')
                                buffer_ptr++;
                            if (buffer_ptr < file_buffer_end) {
                                buffer_ptr++;
                                if (*(buffer_ptr - 1) == '\r'
                                        && *buffer_ptr == '\n'
                                        && buffer_ptr < file_buffer_end)
                                    buffer_ptr++;
                                row++;
                            }
                        }
                        row = start_row;    /* In case at file_buffer end */

                    /* Point to start column */

#if DBCS
			slptr = buffer_ptr;	/* save start pointer */
#endif /* DBCS */
                        column = 0;
                        while (!end_of_line && column < start_column
                                && buffer_ptr < file_buffer_end) {
                            if (buffer_ptr >= file_buffer_end
                                    || *buffer_ptr == '\r'
                                    || *buffer_ptr == '\n') {
                                end_of_line = TRUE;
                            } else {
                                if (*buffer_ptr == '\t') {
                                    if ((column = (column / 8 + 1) * 8)
                                            > start_column)
                                        column = start_column;
                                    else
                                        buffer_ptr++;
                                } else {
                                    column++;
                                    buffer_ptr++;
                                }
                            }
                        }
                        s = text_buffer;
                    } else {
                        row = start_row;
                        if ((buffer_ptr
                                    = file_buffer + row * BYTES_PER_HEX_LINE)
                                >= file_buffer_end) {
                            buffer_ptr = file_buffer_end;
                            column = start_column;
                            end_of_line = TRUE;
                        } else {

                        /* Create hex display in text_buffer */
			    t = text_buffer ;
                            memset(text_buffer, ' ', HEX_LINE_LENGTH);
			    t += sprintf( (BYTE *)t, "%06lX  ", 
				window_offset + row * BYTES_PER_HEX_LINE ) ;
                            i = (WORD)(file_buffer_end - buffer_ptr) ;
                            if ( i > BYTES_PER_HEX_LINE)
                                i = BYTES_PER_HEX_LINE;
                            for (j = 0; j < i; j++) {
				t += sprintf( (BYTE *)t, "%02X ", 
						0xff & (*(buffer_ptr + j)) );
                            }
			    *t = ' ' ;
                            text_buffer[6 + 2 + (BYTES_PER_HEX_LINE / 2) * 3
                                - 1] = '-';
                            t = text_buffer + HEX_LINE_LENGTH
                                - BYTES_PER_HEX_LINE;
                            while (i > 0) {
				*t = (*buffer_ptr == '\0') ? '.' : *buffer_ptr;
                                t++;
                                buffer_ptr++;
                                i--;
                            }
                            if (start_column < HEX_LINE_LENGTH) {
                                column = start_column;
                                s = text_buffer + start_column;
                            } else {
                                column = HEX_LINE_LENGTH;
                                end_of_line = TRUE;
                            }
                        }
                    }

                /* Output text */

                    while (!end_of_line && column < end_column) {

                        xy_array[0] = work_area.g_x
                            + (column - window_column) * gl_wchar;

                        string_length = 0;
                        if (show_as_text) {

                        /* Transfer text to text_buffer, translating characters
                           */

                            t = s = text_buffer;    /* Start of text */
                            while (!end_of_line && column < end_column
                                    && string_length < max_str_len) {
                                if (buffer_ptr >= file_buffer_end
                                        || *buffer_ptr == '\r'
                                        || *buffer_ptr == '\n') {
                                    end_of_line = TRUE;
                                } else {
                                    if (*buffer_ptr == '\t') {
                                        i = (column / 8 + 1) * 8;
                                        while (column < i && column < end_column
                                                && string_length
                                                    < max_str_len) {
                                            *s++ = ' ';
                                            column++;
                                            string_length++;
                                        }
                                        if (column == i)
                                            buffer_ptr++;
                                        else
                                            string_length = max_str_len;
                                    } else {
                                        if (*buffer_ptr == '\0')
                                            *s = '.';
                                        else
                                            *s = *buffer_ptr;
#if DBCS
					if ( dbcs_expected() && (s == t) &&
					    (buffer_ptr > slptr) )
					{
					    dbc1 = 0;
					    for(p=buffer_ptr-1; p>=slptr; p--)
						if (dbcs_lead(*p))
						    dbc1++;
						else
						    break;
					    if (dbc1 & 1)
						*s = ' ';
					}
#endif /* DBCS */
                                        s++;
                                        column++;
                                        string_length++;
                                        buffer_ptr++;
                                    }
                                }
                            }
                        } else {
                            t = s;      /* Start of text */
#if DBCS
			    if (dbcs_expected())
				cvt_dbc2(text_buffer, t);
#endif /* DBCS */
                            while (!end_of_line && column < end_column
                                    && string_length < max_str_len) {
                                if (s == text_buffer + HEX_LINE_LENGTH) {
                                    end_of_line = TRUE;
                                } else {
                                    s++;
                                    column++;
                                    string_length++;
                                }
                            }
                        }

                        c = *s;		/* Save character at end of string */
                        *s = '\0';      /* Mark end of string */
                        v_gtext(gl_handle, xy_array[0], work_area.g_y + row * gl_hchar, 
				(BYTE*)t);
                        *s = c;     /* Restore character overwritten with NUL */
                    }

                /* Clear after end of line if necessary */

                    if (column < end_column) {
                        xy_array[0] = work_area.g_x
                            + (column - window_column) * gl_wchar;
                        xy_array[1] = work_area.g_y + row * gl_hchar;
                        xy_array[2] = dirty_dest.g_x + dirty_dest.g_w;
                        xy_array[3] = work_area.g_y + (row + 1) * gl_hchar;
                        vr_recfl(gl_handle, xy_array);   /* NULL not used */
                    }

                    start_row++;        /* Next required row */
                }

                vs_clip(gl_handle, 0, NULL);       /* Disable clipping */
                vst_alignment(gl_handle, ALI_LEFT, ALI_BASE, &hor_alignment,
                    &vert_alignment);   /* Set text alignment to top left */
            }
        }
        wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w,
            &box.g_h);
    }

    graf_mouse(M_ON, 0L);
} /* do_redraw() */

/*-----------------------------------------------------------------------------
Purpose : Toggle window between full and previous size
Entry   : wh    Window handle
Exit    : 
*/
MLOCAL void  do_full(WORD wh)
{
    GRECT	curr, full;

    graf_mouse(M_OFF, 0L);
    wind_get(wh, WF_CXYWH, &curr.g_x, &curr.g_y, &curr.g_w, &curr.g_h);
    wind_get(wh, WF_FXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);
    if ( curr.g_h > full.g_h/2 )
	full.g_h/=2;
    wind_set(wh, WF_CXYWH, full.g_x, full.g_y, full.g_w, full.g_h);
    wind_get(wh, WF_WXYWH, &work_area.g_x, &work_area.g_y,
        &work_area.g_w, &work_area.g_h);
    graf_mouse(M_ON, 0L);
} /* do_full() */

/*-----------------------------------------------------------------------------
Purpose : Handle message event
Entry   : message       Message buffer
          wh            Window handle
Exit    : 
*/
/* (hca: not MLOCAL, used in deskinf.c) */
void  handle_message(WORD *message, WORD wh)
{
    BOOLEAN redraw_required;            /* True if work area needs to updated */
    SLONG required_row;
    GRECT work;
    WORD on_top,junk;
    
    wind_get( 0,WF_TOP,&on_top, &junk, &junk, &junk );

    redraw_required = FALSE;

    switch (message[0]) {

    case MN_SELECTED:   /* Handle a menu event */
      if( on_top == wh ){
        switch( message[3] ){
          case VIEWMENU:
            switch( message[4] ){
	      case CLOSITEM:
                quit_flag = TRUE;
                return_value = FALSE;   /* Force termination (JFL) */
                break;
	      case TEXTITEM:
                handle_key( 'T'-'@',wh );	/* Toggle text mode */
	        break;
/*	      case ZOOMITEM: XXX ViewMAX
                do_full( wh );
		break;		*/		/* Must be full box then */
	     }
	     break;
	   default:
             quit_flag = return_value = hndl_menu( message[3], message[4] );
	     break;
	  }
        }
        else
	  quit_flag = return_value = hndl_menu( message[3], message[4] );
        break;
    case WM_REDRAW:
	if( quit_flag )
	  break;
	if (message[3] == wh) 
	{
            work.g_x = message[4];
            work.g_y = message[5];
            work.g_w = message[6];
            work.g_h = message[7];
            do_redraw(message[3], &work);
        } 
	else 
	{
            fun_msg(WM_REDRAW, message[3], message[4], message[5], message[6],
                message[7]);
        }
        break;

    case WM_TOPPED:
        if (message[3] == wh) 
	{
            wind_set(message[3], WF_TOP, message[3], 0, 0, 0);
	    men_list( G.a_trees[ADMENU], GOOD_TYPE, TRUE );
  	    men_list( G.a_trees[ADMENU], ILL_TYPE, FALSE );
	}
        else
	{
            fun_msg(WM_TOPPED, message[3], message[4], message[5], message[6],
		    message[7] );
	}
    break;

    case WM_CLOSED:
        if (message[3] == wh){
            quit_flag = TRUE;
            return_value = FALSE;
            on_top = FALSE;
	}
        else
	{
            fun_msg(WM_CLOSED, message[3], message[4], message[5], message[6],
		    message[7] );
	}
        break;

    case WM_FULLED:
        if (message[3] == wh) 
	{
            do_full(message[3]);
        }
        else
            fun_msg(WM_FULLED, message[3], message[4], message[5], message[6],
		    message[7] );
        break;

    case WM_ARROWED:
        if (message[3] == wh) 
	{
            switch (message[4]) {       /* Requested action */

            case WA_UPPAGE:
                if (window_row > 0) {
                    if ((required_row
                                = window_row - work_area.g_h / gl_hchar + 1)
                            < 0)
                        required_row = 0;
                    seek_row(required_row);
                    redraw_required = TRUE;
                }
                break;

            case WA_DNPAGE:
                if (window_row < num_lines - work_area.g_h / gl_hchar) {
                    if ((required_row
                                = window_row + work_area.g_h / gl_hchar - 1)
                            > num_lines - work_area.g_h / gl_hchar)
                        required_row = num_lines - work_area.g_h / gl_hchar;
                    seek_row(required_row);
                    redraw_required = TRUE;
                }
                break;

            case WA_UPLINE:
                if (window_row > 0) {
                    seek_row(window_row - 1);
                    redraw_required = TRUE;
                }
                break;

            case WA_DNLINE:
                if (window_row < num_lines - work_area.g_h / gl_hchar) {
                    seek_row(window_row + 1);
                    redraw_required = TRUE;
                }
                break;

            case WA_LFPAGE:
                if (window_column > 0) {
                    if ((window_column -= work_area.g_w / gl_wchar - 1) < 0)
                        window_column = 0;
                    redraw_required = TRUE;
                }
                break;

            case WA_RTPAGE:
                if (window_column < max_line_length - work_area.g_w / gl_wchar) {
                    if ((window_column += work_area.g_w / gl_wchar - 1)
                            > max_line_length - work_area.g_w / gl_wchar)
                        window_column
                            = max_line_length - work_area.g_w / gl_wchar;
                    redraw_required = TRUE;
                }
                break;

            case WA_LFLINE:
                if (window_column > 0) {
                    window_column--;
                    redraw_required = TRUE;
                }
                break;

            case WA_RTLINE:
                if (window_column
                        < max_line_length - work_area.g_w / gl_wchar) {
                    window_column++;
                    redraw_required = TRUE;
                }
                break;
            } /* end switch */
        } 
	else
	{
            fun_msg(WM_ARROWED, message[3], message[4], message[5], 
		    message[6], message[7] );
	}
	break;

    case WM_HSLID:
        if (message[3] == wh) {
            if (work_area.g_w / gl_wchar < max_line_length) {
                if( message[4]<1 ) message[4]=1;
                window_column = (WORD) (((SLONG) (message[4] - 1)
                        * (SLONG) (max_line_length - (work_area.g_w / gl_wchar)))
                        / 999);
                redraw_required = TRUE;
            }
        }
        break;

    case WM_VSLID:
        if (message[3] == wh) {
            if (!scanning_file && (SLONG) (work_area.g_h / gl_hchar)
                    < num_lines) {
                if( message[4]<1 ) message[4]=1;
		seek_row(((SLONG) (message[4]-1)
                    * (num_lines - (SLONG) (work_area.g_h / gl_hchar))) / 999);
                redraw_required = TRUE;
            }
        }
	else
            fun_msg(WM_VSLID, message[3], message[4], message[5], message[6],
		    message[7] );
        break;

    case WM_SIZED:
    case WM_MOVED:
        if (message[3] == wh) {
            wind_calc( WC_WORK, WKIND_SHOW, message[4], message[5], message[6],
                message[7], &work.g_x, &work.g_y, &work.g_w, &work.g_h);
            work.g_x = WORD_ALIGN( work.g_x );
            work.g_w = (work.g_w / gl_wchar) * gl_wchar;
            work.g_h = (work.g_h / gl_hchar) * gl_hchar;
            wind_calc(WC_BORDER, WKIND_SHOW, work.g_x, work.g_y, work.g_w,
                work.g_h, &message[4], &message[5], &message[6], &message[7]);
            message[4] = WORD_ALIGN( message[4] );
            wind_set(message[3], WF_CXYWH, message[4], message[5], message[6],
                message[7]);
            wind_get(message[3], WF_WXYWH, &work_area.g_x, &work_area.g_y,
                &work_area.g_w, &work_area.g_h);
            if (window_column > max_line_length - work.g_w / gl_wchar) {
                if ((window_column = max_line_length - work.g_w / gl_wchar) < 0)
                    window_column = 0;
            }
            redraw_required = TRUE;
            break;
        }
    else
            fun_msg(message[0], message[3], message[4], message[5], 
		    message[6], message[7] );
    }

    if (redraw_required) 
    {
        do_redraw(message[3], &work_area);
        set_sliders(message[3]);
    }
} /* handle_message() */

/*-----------------------------------------------------------------------------
Purpose : Handle keyboard event
Entry   : key           VDI keyboard code
          wh            Window handle
Exit    : 
*/
MLOCAL void  handle_key(UWORD key, WORD wh)
{
    SLONG old_offset, current_offset;
    UBYTE far *s;
    WORD on_top,junk;
    
    wind_get( 0,WF_TOP,&on_top, &junk, &junk, &junk );
    if( wh!=on_top ){
      quit_flag = return_value = hndl_kbd( key );
      return;
    }
    
    if ((key & 0xff) == 'T' - '@') {    /* Ctrl T - toggle text/hex display */
        graf_mouse(HOURGLASS, 0L);
        if (show_as_text) {
            show_as_text = FALSE;
            max_line_length = HEX_LINE_LENGTH;
            num_lines
                = (file_size + BYTES_PER_HEX_LINE - 1) / BYTES_PER_HEX_LINE;
            window_row = window_offset / BYTES_PER_HEX_LINE;
            window_offset = (window_offset / BYTES_PER_HEX_LINE)
                * BYTES_PER_HEX_LINE;
        } else {
            show_as_text = TRUE;
            max_line_length = max_text_line_length;
            num_lines = num_text_lines;

    /* Find row and offset for start of text line */

            old_offset = window_offset;
            window_offset = current_offset = 0;
            window_row = 0;
            while (current_offset < old_offset && !file_error) {
                if (dos_lseek(file_handle, 0, current_offset)
                        == current_offset) {
                    file_buffer_end = file_buffer + dos_read(file_handle,
                        file_buffer_size, file_buffer );
                    s = file_buffer;
                    while (current_offset + (s - file_buffer) < old_offset
                            && s < file_buffer_end - 1) {
                        if (*s == '\r' || *s == '\n') {
                            if (*s == '\r' && *(s + 1) == '\n')
                                s++;
                            window_row++;
                            window_offset
                                = current_offset + (s + 1 - file_buffer);
                        }
                        s++;
                    }
                    current_offset += (s - file_buffer);
                } else {
                    file_error = TRUE;
                }
            }
        }
        seek_read();
        do_redraw(wh, &work_area);
        set_sliders(wh);
        graf_mouse(ARROW, 0L);
    }
} /* handle_key() */

/*-----------------------------------------------------------------------------
Purpose : Scan part file to accumulate maximum text line length and number of
          text lines
Entry   : wh    Window handle
Exit    : 
*/
MLOCAL void  scan_file(WORD wh)
{
    WORD ii;
    WORD old_max_line_length;
    SLONG old_num_lines;
    UBYTE *s;
    UBYTE buffer[513];

    if (dos_lseek(file_handle, 0, file_size) == file_size) 
    {
        ii = dos_read( file_handle, sizeof(buffer), (UBYTE far *)buffer );
        if ( ii == sizeof(buffer))
            ii = sizeof(buffer) - 1;
        else
            scanning_file = FALSE;
        old_max_line_length = max_text_line_length;
        old_num_lines = num_text_lines;
        s = buffer;
        while (s < buffer + ii) 
	{
            if (*s == '\r' || *s == '\n') 
	    {
                if (*s == '\r' && *(s + 1) == '\n')
                    s++;
                scan_column = 0;
                num_text_lines++;
            } 
	    else 
	    {
                if ((*s >= '0' && *s <= '9') || (*s >= 'A' && *s <= 'Z')
                        || (*s >= 'a' && *s <= 'z')) 
		{
                    if (!scan_in_word) 
		    {
                        num_words++;
                        scan_in_word = TRUE;
                    }
                } 
		else 
		{
                    scan_in_word = FALSE;
                }
                if (++scan_column > max_text_line_length)
                    max_text_line_length = scan_column;
            }
            s++;
        }
        file_size += (s - buffer);
        if (show_as_text) 
	{
            if (max_text_line_length > old_max_line_length
                    || num_text_lines > old_num_lines) 
	    {
                max_line_length = max_text_line_length;
                num_lines = num_text_lines;
                set_sliders(wh);
            }
        } 
	else 
	{
            num_lines
                = (file_size + BYTES_PER_HEX_LINE - 1) / BYTES_PER_HEX_LINE;
            set_sliders(wh);
        }
        if (!scanning_file) 
	{
            set_info_line(wh);
        }
    }
    else {
        file_error = TRUE;
        scanning_file = FALSE;
    }
} /* scan_file() */

/*-----------------------------------------------------------------------------
Purpose : Read and scan start of file to accumulate maximum text line length and
          number of text lines and decide whether text file
Entry   : wh    Window handle
Exit    : 
*/
MLOCAL void  start_scan_file(WORD wh)
{
    WORD i;
    WORD num_text_chars;
    UBYTE far *s;
#if DBCS
    WORD type = CT_ADE;
#endif /* DBCS */

    file_error = FALSE;
    window_offset = 0;
    if ((i = dos_read(file_handle, file_buffer_size, file_buffer ))
            == 0) {
	rsrc_gaddr(R_STRING, STEMPTY, (LPVOID *)&G.a_alert);
	form_alert(1, G.a_alert);	
        /* alert( 0x0101, EREMPTY ); */
        quit_flag = TRUE;
    } else {
        file_buffer_end = file_buffer + i;
        scanning_file = (i == file_buffer_size);
        max_text_line_length = 0;
        num_text_lines = num_words = 0;
        scan_column = 0;
        scan_in_word = FALSE;
        num_text_chars = 0;
        s = file_buffer;
        while (s < file_buffer_end - 1) {
#if DBCS
	    type = chkctype(*s, type);
	    if (type == CT_DBC1 || type == CT_DBC2)
                num_text_chars++;
	    else
#endif /* DBCS */
            if ((*s >= ' ' && *s <= 0x7e) || (*s >= 0x09 && *s <= 0x0d))
                num_text_chars++;
            if (*s == '\r' || *s == '\n') {
                if (*s == '\r' && *(s + 1) == '\n')
                    s++;
                scan_column = 0;
                num_text_lines++;
            } else {
                if ((*s >= '0' && *s <= '9') || (*s >= 'A' && *s <= 'Z')
                        || (*s >= 'a' && *s <= 'z')) {
                    if (!scan_in_word) {
                        num_words++;
                        scan_in_word = TRUE;
                    }
                } else {
                    scan_in_word = FALSE;
                }
                if (++scan_column > max_text_line_length)
                    max_text_line_length = scan_column;
            }
            s++;
        }
        file_size = i = (WORD)(s - file_buffer);

    /* Assume non-text file if less than 70% text */

        show_as_text = (((SLONG) num_text_chars * 10) / i >= 7);
        if (max_text_line_length == 0)
            max_text_line_length = 1;
        if (num_text_lines == 0)
            num_text_lines = 1;
        if (show_as_text) {
            max_line_length = max_text_line_length;
            num_lines = num_text_lines;
        } else {
            max_line_length = HEX_LINE_LENGTH;
            num_lines
                = (file_size + BYTES_PER_HEX_LINE - 1) / BYTES_PER_HEX_LINE;
        }
        window_column = 0;
        window_row = 0;
        quit_flag = FALSE;
        set_sliders(wh);
        set_info_line(wh);
    }
} /* start_scan_file() */

/*-----------------------------------------------------------------------------
Purpose : Get and handle events
Entry   : wh            Window handle
Exit    : 
*/
GLOBAL void  get_handle_events(WORD wh)
{
    WORD ev_which;
    UWORD mousex, mousey, bstate, kbd_state, kbd_code, bclicks;
    WORD message[8];                    /* Message buffer */
    WORD on_top,junk;
    
    wind_get( 0,WF_TOP,&on_top, &junk, &junk, &junk );

    ev_which = MU_MESAG | MU_KEYBD | MU_BUTTON;
    if( scanning_file && wh==on_top )
        ev_which |= MU_TIMER;
	
    ev_which = evnt_multi( ev_which,
        2, 1, 1,                        /* Button requirements */
        0, 0, 0, 0, 0,                  /* Mouse rectangle 1 */
        0, 0, 0, 0, 0,                  /* Mouse rectangle 2 */
        (LPWORD)(ADDR(message)),	/* Message buffer */
        10, 0,                          /* Timer counts */
        &mousex, &mousey, &bstate,
        &kbd_state, &kbd_code,
        &bclicks);                      /* Return values */

    wind_update( BEG_UPDATE );
    
  /* XXX ViewMAX   hilite_obj( FALSE ); */

    if (ev_which & MU_BUTTON && (wind_find(mousex,mousey)!=wh) )
	quit_flag = return_value = hndl_button(bclicks, mousex, mousey, bstate, kbd_state);
    
    if (ev_which & MU_MESAG)
        handle_message(message, wh);
    
    if (ev_which & MU_KEYBD)
        handle_key(kbd_code, wh);
    
    if ( scanning_file && ev_which == MU_TIMER) 
        scan_file(wh);

  /* XXX ViewMAX  hilite_obj( TRUE ); */

    wind_update( END_UPDATE );
    
} /* get_handle_events() */



/*-----------------------------------------------------------------------------
Purpose : Type file
Entry   : curr  Current item
Exit    : Returns zero if successful
	  -4 if can't create window
	  -5 if can't alloc enough memory for one line
	  -6 if DOS_ERR on open file
Caller	: do_filemenu() in desktop.c
*/
WORD  do_type(WORD curr)
{
    WORD	i;
    WORD	wh;			/* View window handle */
    WORD	drive;
    LONG	mem_avail;
    ANODE	*panode;
    WNODE *	pwnode;
    FNODE *	pfnode;
    BYTE 	name[LEN_ZFNAME];
    BYTE	*ptr1;
    BYTE	*ptr2;
    BYTE	path_name[LEN_ZPATH], file_stem[MAXFILE] ;
    BYTE	path_spec[LEN_ZPATH], title[LEN_ZPATH + 2];
    GRECT	d,c;

    return_value = 0;

    panode = i_find(G.g_cwin, curr, &pfnode, &i);
    pwnode = win_find(G.g_cwin);

    if (panode) {
        if (panode->a_type == AT_ISFILE || panode->a_type == AT_ISWIND) 
	{
        /* Parse path name into drive, path, file name stem and extension */

            fpd_parse(pwnode->w_path->p_spec, &drive, path_name, file_stem,
						(BYTE *)NULL ) ;

        /* Split file name into stem and extension */
	    lstlcpy((BYTE far *)name, pfnode->f_name, LEN_ZFNAME);
	    strlcpy( file_stem, name, MAXFILE ) ;
	    ptr2 = strrchr( file_stem, '.' ) ;
	    if ( ptr2 ) {
		ptr1 = ptr2 + 1 ;	/* ptr1 points to extension */
		*ptr2 = '\0' ;	/* ptr2 points to end of root file name */
	    }
	    else
		ptr1 = name + strlen( name ) ; /* no extension found */

        /* Build complete path/file spec */

            fpd_bldspec(drive, path_name, file_stem, ptr1, path_spec);
	 
        /* Copy path/file spec without passwords to title */

	    sprintf( title, " %s ", path_spec ) ;
	    ptr1 = title ;
            while ( ptr1 ) {
		ptr1 = strchr( title, ';' ) ;	/* find a password */
		if ( ptr1 ) {			
		    *ptr1 = '\0' ;
		    ptr2 = strchr( ptr1+1, '\\' ) ;	/* find another slash */
		    if ( ptr2 )			/* shift string to left */
			memmove( (void *)ptr1, (void *)ptr2, 1 + strlen( ptr2 ) ) ;
		}
	    }

            graf_mouse(HOURGLASS, 0L);

        /* Get maximum string length */

/* Not used to save space, assumes maximum string length of MAX_STR_LEN
            vq_extnd(gl_handle, 1, work_out);
            if ((max_str_len = work_out[15]) > MAX_STR_LEN)
*/
                max_str_len = MAX_STR_LEN;

            info_line[0] = '\0';

/* Do zooming box effect */
	    get_xywh(G.g_screen, curr,      &d.g_x, &d.g_y, &d.g_w, &d.g_h);
	    get_xywh(G.g_screen, G.g_croot, &c.g_x, &c.g_y, &c.g_w, &c.g_h);
	    d.g_x += c.g_x;
	    d.g_y += c.g_y;
	    graf_growbox(d.g_x, d.g_y, d.g_w, d.g_h, G.g_xdesk, 
			    G.g_ydesk, G.g_wdesk, G.g_hdesk);

	    wh = create_window( WKIND_SHOW, title, info_line, &work_area ) ;
	    if ( wh == -1 )
		return_value = -4 ;

	    else {
                graf_mouse(ARROW, 0L);
                wind_update(END_UPDATE);

            /* Allocate memory */

                file_buffer_size = (work_area.g_w / gl_wchar + 2)
                    * (work_area.g_h / gl_hchar) + 1;
#if 1
		mem_avail = dos_avail();
/* XXX We have no find yet
 		if ( (LONG)file_buffer_size > mem_avail )
		{
			// See if there's memory being held by Find	
		    release_last_find();
		    mem_avail = dos_avail();
		}
*/
	        if ( (LONG)file_buffer_size > mem_avail )
		{
			/* Resize to fit avail space, preserving	*/
			/*	divisibility. We know mem_avail fits	*/
			/*	in a word because it's < size		*/
			file_buffer_size = (WORD)mem_avail - 
			    ((WORD)mem_avail % (work_area.g_x/ gl_wchar+2));
		}
		file_buffer = (UBYTE far *) dos_alloc(file_buffer_size);
#else /* 1 */
                do {
                    if ((file_buffer
                            = (UBYTE far *) dos_alloc(file_buffer_size)) == 0)
                        file_buffer_size -= work_area.g_x / gl_wchar + 2;
                } while (file_buffer == NULL && file_buffer_size > 0);
#endif /* 1 */
                if (file_buffer == NULL) {
                    menu_item_to_alert_s( /*0x0101*/1, STNOMEM, TYPITEM );
                    return_value = -5;
                } else {

                /* Open file */

                    file_handle = dos_open( (BYTE far *)path_spec, 0);

                    if (DOS_ERR) {
			if( DOS_AX != E_PASSWFAIL )
			{
                            alert_s( /*0x0101*/1, STBADOPN, path_spec );
                            return_value = -6;
			} /* endif E_PASSWFAIL */
                    } else {

                    /* Read start of file and investigate */

                        start_scan_file(wh);

	                fun_msg(WM_REDRAW, wh, work_area.g_x, work_area.g_y,
                   		work_area.g_w, work_area.g_h);

                    /* Main loop */

                        while (!quit_flag)
                            get_handle_events(wh);

                    /* Normal termination */

                        dos_close(file_handle);
                    } /* endif DOS_ERR */

                /* Finalisation */

                    dos_free( file_buffer );
                } /* endif NULL file_buffer */
   /* reset text alignment (JFL) */
                vst_alignment(gl_handle, ALI_LEFT, ALI_BASE, &i, &i);
                wind_update(BEG_UPDATE);
		/* close_window( wh ) ; */
		wind_get(wh, WF_WXYWH, &c.g_x, &c.g_y, &c.g_w, &c.g_h);
		wind_close( wh );
		wind_delete( wh );
	        graf_shrinkbox(d.g_x, d.g_y, d.g_w, d.g_h, c.g_x,
			    c.g_y, c.g_w, c.g_h);
            } /* endif wind_create */
            graf_mouse(ARROW, 0L);
        } /* endif pa_node ISFILE */
    } /* endif pa_node non NULL */

    return(return_value);
} /* do_type() */

#endif
/*
 *	EOF:	desktype.c
 */
