/* JCE 3-1-1998: The single 'include' file necessary for PPD to compile 
   GEM applications.
   DB 21-12-2021: Modified for Open Watcom

   This file contains prototypes for all AES and VDI functions. 
*/

#ifndef __WCCGEM_H__

#define __WCCGEM_H__ 21/12/2021

#include "portab.h"	/* BYTE, WORD, LONG etc. */
#include "machine.h"	/* WCC-specific */
#include "wccdefs.h"
#include "obdefs.h"
#include "wccgbind.h"
#include "wccgdos.h"

#include "rclib.h"	/* PTK 3.1 functions */
#include "oblib.h"
#include "xfmlib.h"
#include "emslib.h"

typedef WORD (*AESFUNC)(LPGEMBLK gb);
typedef WORD (*VDIFUNC)(LPGSXPAR pb);

/* To make an object user-draw, call this function on it */

VOID wcc_userdef(LPTREE tree, WORD nobj, LPPBLK ub);

/* These functions catch INT 0xEF, allowing your program to provide a
 * substitute AES or VDI. 
 */

VOID wcc_hookon(AESFUNC pAes, AESFUNC pAes2, VDIFUNC pVdi);
VOID wcc_hookoff(void);

/* When you have a substitute AES/VDI installed, call this function to set
 * the value it would return in AX.
 */

VOID wcc_setresult(WORD w);

/* These two functions allow GEM calls from your program to be diverted to
 * other places than INT 0xEF. 
 */

AESFUNC divert_aes(AESFUNC f);
VDIFUNC divert_vdi(VDIFUNC f);

WORD GEMAIN(WORD ARGC, BYTE *ARGV[]);
WORD gem(LPGEMBLK gb);
WORD vdi(VOID);
WORD gemcheck(VOID);
WORD aescheck(VOID);

/* Prototypes for the AES functions */

WORD appl_init   (LPXBUF lpx);	/* Pass in a NULL if you don't need the XBUF */
WORD appl_exit   (void);
WORD appl_write  (WORD  rwid,    WORD length, LPVOID pbuff);
WORD appl_read   (WORD  rwid,    WORD length, LPVOID pbuff);
WORD appl_find   (LPVOID  pname);
WORD appl_tplay  (LPVOID  tbuffer, WORD  tlength, WORD tscale);
WORD appl_trecord(LPVOID  tbuffer, WORD  tlength);
WORD appl_bvset  (UWORD bvdisk,  UWORD bvhard);
VOID appl_yield  (void);
WORD appl_xbvget  (ULONG *bvdisk,  ULONG *bvhard);
WORD appl_xbvset  (ULONG bvdisk,  ULONG bvhard);

UWORD evnt_keybd(void);
WORD  evnt_button(WORD clicks, UWORD mask, UWORD state, WORD *pmx, 
		  WORD *pmy, WORD *pmb, WORD *pks);
WORD  evnt_mouse(WORD flags, WORD x, WORD y, WORD width, WORD height,
		  WORD *pmx, WORD *pmy, WORD *pmb, WORD *pks);
WORD  evnt_mesag(LPVOID pbuff);
WORD  evnt_timer(UWORD locnt, UWORD hicnt);
WORD  evnt_multi(UWORD flags, UWORD bclk, UWORD bmsk, UWORD bst, 
		 UWORD m1flags, UWORD m1x, UWORD m1y, UWORD m1w, UWORD m1h, 
		 UWORD m2flags, UWORD m2x, UWORD m2y, UWORD m2w, UWORD m2h, 
                 LPVOID mepbuff, UWORD tlc, UWORD thc, UWORD *pmx, UWORD *pmy,
		 UWORD *pmb, UWORD *pks, UWORD *pkr, UWORD *pbr);
WORD evnt_dclick(WORD rate, WORD setit);
WORD evnt_event(MEVENT *ev);	/* Alternative to evnt_multi */
					
WORD menu_bar(     LPTREE tree, WORD showit);
WORD menu_icheck(  LPTREE tree, WORD itemnum,  WORD checkit);
WORD menu_ienable( LPTREE tree, WORD itemnum,  WORD enableit);
WORD menu_tnormal( LPTREE tree, WORD titlenum, WORD normalit);
WORD menu_text(    LPTREE tree, WORD inum,     LPBYTE ptext);
WORD menu_register(WORD pid, LPVOID pstr);
WORD menu_unregister(WORD mid);

WORD objc_add(LPTREE tree, WORD parent, WORD child);
WORD objc_delete(LPTREE tree, WORD delob);
WORD objc_draw(LPTREE tree, WORD drawob, WORD depth, WORD xc, WORD yc, 
                                                   WORD wc, WORD hc);
WORD objc_find(LPTREE tree, WORD startob, WORD depth, WORD mx, WORD my);
WORD objc_order(LPTREE tree, WORD mov_obj, WORD newpos);
WORD objc_offset(LPTREE tree, WORD obj, WORD *poffx, WORD *poffy);
WORD objc_edit(LPTREE tree, WORD obj, WORD inchar, WORD *idx, WORD kind);
WORD objc_change(LPTREE tree, WORD drawob, WORD depth, 
                 WORD xc, WORD yc, WORD wc, WORD hc, 
                 WORD newstate, WORD redraw);
					
WORD form_do(LPVOID form, WORD start);
WORD form_exdo(LPTREE form, WORD start); /* Implemented in the library */
WORD form_dial(WORD dtype, WORD ix, WORD iy, WORD iw, WORD ih, 
                           WORD x,  WORD y,  WORD w,  WORD h);
WORD form_alert(WORD defbut, LPBYTE astring);
WORD form_error(WORD errnum);
WORD form_center(LPTREE tree, WORD *pcx, WORD *pcy, WORD *pcw, WORD *pch);
WORD form_keybd(LPVOID form, WORD obj, WORD nxt_obj, WORD thechar, 
		WORD *pnxt_obj, WORD *pchar);
WORD form_button(LPVOID form, WORD obj, WORD clks, WORD *pnxt_obj);

WORD graf_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin, 
		 WORD *pwend, WORD *phend);
WORD graf_dragbox(WORD w,  WORD h,  WORD sx,   WORD sy, WORD xc, WORD yc, 
                  WORD wc, WORD hc, WORD *pdx, WORD *pdy);
WORD graf_mbox(WORD w, WORD h, WORD srcx, WORD srcy, WORD dstx, WORD dsty);
WORD graf_watchbox(LPTREE tree, WORD obj, UWORD instate, UWORD outstate);
WORD graf_slidebox(LPTREE tree, WORD parent, WORD obj, WORD isvert);
WORD graf_handle(WORD *pwchar, WORD *phchar, WORD *pwbox, WORD *phbox);
WORD graf_mouse(WORD m_number, LPUWORD m_addr);
VOID graf_mkstate(WORD *pmx, WORD *pmy, WORD *pmstate, WORD *pkstate);
/* Obsolete, for compatibility only */
WORD graf_growbox(WORD,WORD,WORD,WORD,WORD,WORD,WORD,WORD);
WORD graf_shrinkbox(WORD,WORD,WORD,WORD,WORD,WORD,WORD,WORD);

/* [JCE 25-3-2006] Added proc_* calls for GEM/XM */
WORD proc_create(LPVOID ibegaddr, LONG isize, WORD isswap, WORD isgem,
		WORD *onum);
WORD proc_run(WORD proc_num, WORD isgraf, WORD isover, LPBYTE pcmd,
		LPBYTE ptail);
WORD proc_delete(WORD proc_num);
WORD proc_info(WORD num, WORD *oisswap, WORD *oisgem, 
		LPVOID *obegaddr, LONG *ocsize, LPVOID *oendmem, 
		LONG *ossize, LPVOID *ointtbl);
LPVOID proc_malloc(LONG size, LONG *ret_size);
WORD proc_switch(WORD pid);
WORD proc_shrink(WORD pid);

WORD scrp_read(LPVOID pscrap);
WORD scrp_write(LPVOID pscrap);
WORD scrp_clear(void);


WORD fsel_input(LPVOID pipath, LPVOID pisel, WORD *pbutton);
WORD fsel_exinput(LPVOID pipath, LPVOID pisel, WORD *pbutton, LPBYTE ptitle);

WORD  wind_create(UWORD kind,   WORD wx, WORD wy, WORD ww, WORD wh);
WORD  wind_open  ( WORD handle, WORD wx, WORD wy, WORD ww, WORD wh);
WORD  wind_close ( WORD handle);
WORD  wind_delete( WORD handle);
WORD  wind_get(    WORD w_handle, WORD w_field, WORD *pw1, WORD *pw2, 
						WORD *pw3, WORD *pw4);
WORD  wind_set(    WORD w_handle, WORD w_field, WORD w2, WORD w3, 
                                                WORD w4, WORD w5);
WORD  wind_find(   WORD mx, WORD my);
WORD  wind_update( WORD beg_update);
WORD  wind_calc(   WORD wctype, UWORD kind, 
		   WORD x,   WORD y,   WORD w,   WORD h,
                   WORD *px, WORD *py, WORD *pw, WORD *ph);
					

WORD  rsrc_load(LPBYTE rsname);
WORD  rsrc_free(void);
WORD  rsrc_gaddr(WORD rstype, WORD rsid, LPVOID *paddr);
WORD  rsrc_saddr(WORD rstype, WORD rsid, LPVOID lngval);
WORD  rsrc_obfix(LPTREE tree, WORD obj);

WORD shel_read(LPVOID pcmd, LPVOID ptail);
WORD shel_write(WORD doex, WORD isgr, WORD iscr, LPVOID pcmd, LPVOID ptail);
WORD shel_find(LPVOID ppath);
WORD shel_envrn(LPVOID ppath, LPVOID psrch);
WORD shel_rdef(LPVOID lpcmd, LPVOID lpdir);
WORD shel_wdef(LPVOID lpcmd, LPVOID lpdir);

WORD xgrf_stepcalc(WORD orgw,  WORD orgh,    WORD xc,    WORD yc, 
                   WORD w,     WORD h,       WORD *pcx,  WORD *pcy, 
                   WORD *pcnt, WORD *pxstep, WORD *pystep);
WORD xgrf_2box(    WORD xc,    WORD yc,      WORD w,     WORD h,
                   WORD corners, WORD cnt,   WORD xstep, WORD ystep,
                   WORD doubled);

/* JCE 7-1-1998 ViewMax "set colours" call */

WORD xgrf_colour(WORD type, WORD fg, WORD bg, WORD style, WORD index);
#define xgrf_color xgrf_colour

/* JCE 16-5-1999 ViewMAX "set desktop image" call */

WORD xgrf_dtimage(LPMFDB deskMFDB);

WORD menu_click(WORD click, WORD setit);	/* GEM/3 only */

/* JCE 24-7-1999 FreeGEM "property" calls */

WORD prop_get(LPBYTE program, LPBYTE section, LPBYTE buf, WORD buflen, WORD options);
WORD prop_put(LPBYTE program, LPBYTE section, LPBYTE buf, WORD options);
WORD prop_del(LPBYTE program, LPBYTE section, WORD options);
WORD prop_gui_get(WORD propnum);
WORD prop_gui_set(WORD propnum, WORD value);

/* JCE 8-8-1999 "Get info" call */

WORD xapp_getinfo(WORD ap_gtype, WORD *ap_gout1, WORD *ap_gout2,
				 WORD *ap_gout3, WORD *ap_gout4);

#define appl_getinfo xapp_getinfo	/* for Atari compatibility */

/* JCE 4-12-1999 "Get/set shell" calls */

WORD xshl_getshell(LPBYTE program);
WORD xshl_setshell(LPBYTE program);


/* Prototypes for the VDI functions */

WORD vro_cpyfm( WORD handle, WORD wr_mode, WORD xy[], LPMFDB srcMFDB, LPMFDB desMFDB );
WORD vr_trnfm(  WORD handle, LPMFDB srcMFDB, LPMFDB desMFDB );

WORD v_opnwk( WORD work_in[], WORD *handle, WORD work_out[] );
WORD v_clswk( WORD handle );
WORD v_clrwk( WORD handle );
WORD v_updwk( WORD handle );
WORD vq_chcells(  WORD handle, WORD *rows, WORD *columns );
WORD v_exit_cur(  WORD handle );
WORD v_enter_cur( WORD handle );
WORD v_curup(     WORD handle );
WORD v_curdown(   WORD handle );
WORD v_curright(  WORD handle );
WORD v_curleft(   WORD handle );
WORD v_curhome(   WORD handle );
WORD v_eeos(      WORD handle );
WORD v_eeol(      WORD handle );
WORD vs_curaddress( WORD handle, WORD row, WORD column );
WORD v_curtext(     WORD handle, BYTE *string );
WORD v_rvon(        WORD handle );
WORD v_rvoff(       WORD handle );
WORD vq_curaddress( WORD handle, WORD *row, WORD *column );
WORD vq_tabstatus(  WORD handle );
WORD v_hardcopy(    WORD handle );
WORD v_dspcur(      WORD handle, WORD x, WORD y );
WORD v_rmcur(       WORD handle );
WORD v_form_adv(    WORD handle );
WORD v_output_window( WORD handle, WORD xy[] );
WORD v_clear_disp_list( WORD handle );
WORD v_bit_image( WORD handle,  BYTE *filename, WORD aspect, 
                  WORD x_scale, WORD y_scale,   WORD h_align, WORD v_align, 
		  WORD xy[] );
WORD vq_scan(     WORD handle, WORD *g_height, WORD *g_slice, 
                               WORD *a_height, WORD *a_slice, WORD *factor );
WORD v_alpha_text( WORD handle, BYTE *string );
WORD vs_palette( WORD handle, WORD palette );
WORD v_sound( WORD handle, WORD frequency, WORD duration );
WORD vs_mute( WORD handle, WORD action );
WORD vsp_film( WORD handle, WORD index, WORD lightness );
BOOLEAN vqp_filmname( WORD handle, WORD index, BYTE *name );
WORD vsc_expose( WORD handle, WORD state );
WORD v_meta_extents( WORD handle, WORD min_x, WORD min_y, 
                                  WORD max_x, WORD max_y );
WORD v_write_meta( WORD handle, WORD num_ints, WORD ints[], 
                                WORD num_pts,  WORD pts[] );
WORD vm_filename( WORD handle, BYTE *filename );
WORD vm_pagesize( WORD handle, WORD pgwidth, WORD pgheight );
WORD vm_coords( WORD handle, WORD llx, WORD lly, WORD urx, WORD ury );
WORD vt_resolution ( WORD handle, WORD xres, WORD yres, WORD *xset, WORD *yset );
WORD vt_axis ( WORD handle, WORD xres, WORD yres, WORD *xset, WORD *yset );
WORD vt_origin ( WORD handle, WORD xorigin, WORD yorigin );
WORD vq_tdimensions( WORD handle, WORD *xdimension, WORD *ydimension );
WORD vt_alignment( WORD handle, WORD dx, WORD dy );
WORD v_pline(   WORD handle, WORD count, WORD xy[] );
WORD v_pmarker( WORD handle, WORD count, WORD xy[] );
WORD v_gtext(   WORD handle, WORD x, WORD y, BYTE *string);
WORD v_fillarea( WORD handle, WORD count, WORD xy[]);
WORD v_cellarray( WORD handle, WORD xy[4], WORD row_length, 
                  WORD el_per_row, WORD num_rows, WORD wr_mode, WORD *colors );
WORD v_bar( WORD handle, WORD xy[] );
WORD v_arc( WORD handle, WORD xc, WORD yc, WORD rad, WORD sang, WORD eang );
WORD v_pieslice( WORD handle, WORD xc, WORD yc, WORD rad, WORD sang, WORD eang );
WORD v_circle(  WORD handle, WORD xc, WORD yc, WORD rad );
WORD v_ellipse( WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad );
WORD v_ellarc(  WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad, 
                WORD sang,   WORD eang );
WORD v_ellpie( WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad, 
	       WORD sang,   WORD eang);
WORD v_rbox(   WORD handle, WORD xy[] );
WORD v_rfbox(  WORD handle, WORD xy[] );
WORD v_justified( WORD handle, WORD x, WORD y, BYTE string[], WORD length, 
                  WORD word_space, WORD char_space);
WORD vqt_justified( WORD handle, WORD x, WORD y, BYTE string[], 
		    WORD length, WORD word_space, WORD char_space, 
                    WORD offsets[]);
WORD vst_height( WORD handle, WORD height, 
                 WORD *char_width, WORD *char_height, 
                 WORD *cell_width, WORD *cell_height );
WORD vst_rotation( WORD handle, WORD angle );
WORD vs_color(   WORD handle, WORD index, WORD *rgb );
WORD vsl_type(   WORD handle, WORD style );
WORD vsl_width(  WORD handle, WORD width );
WORD vsl_color(  WORD handle, WORD index );
WORD vsm_type(   WORD handle, WORD symbol );
WORD vsm_height( WORD handle, WORD height );
WORD vsm_color(  WORD handle, WORD index );
WORD vst_font(   WORD handle, WORD font );
WORD vst_color(  WORD handle, WORD index );
WORD vsf_interior( WORD handle, WORD style );
WORD vsf_style(   WORD handle, WORD index );
WORD vsf_color(   WORD handle, WORD index );
WORD vq_color( WORD handle, WORD index, WORD set_flag, WORD rgb[] );
WORD vq_cellarray( WORD handle, WORD xy[], WORD row_len, WORD num_rows, 
                   WORD *el_used, WORD *rows_used, WORD *stat, WORD colors[] );
WORD vrq_locator( WORD handle, WORD initx, WORD inity, 
                  WORD *xout, WORD *yout, WORD *term );
WORD vsm_locator( WORD handle, WORD initx, WORD inity, 
                  WORD *xout, WORD *yout, WORD *term );
WORD vrq_valuator( WORD handle, WORD val_in, WORD *val_out, WORD *term );
WORD vsm_valuator( WORD handle, WORD val_in, WORD *val_out, WORD *term, 
                   WORD *status );
WORD vrq_choice( WORD handle, WORD in_choice, WORD *out_choice );
WORD vsm_choice( WORD handle, WORD *choice );
WORD vrq_string( WORD handle, WORD length, WORD echo_mode, WORD echo_xy[],
                 BYTE *string);
WORD vsm_string( WORD handle, WORD length, WORD echo_mode, WORD echo_xy[],
                 BYTE *string );
WORD vswr_mode( WORD handle, WORD mode );
WORD vsin_mode( WORD handle, WORD dev_type, WORD mode );
WORD vql_attributes( WORD handle, WORD attributes[] );
WORD vqm_attributes( WORD handle, WORD attributes[] );
WORD vqf_attributes( WORD handle, WORD attributes[] );
BOOLEAN vqt_attributes( WORD handle, WORD attributes[] );
WORD vst_alignment( WORD handle, WORD hor_in,   WORD vert_in, 
                                 WORD *hor_out, WORD *vert_out );
WORD v_opnvwk( WORD work_in[], WORD *handle, WORD work_out[] );
WORD v_clsvwk( WORD handle );
WORD vq_extnd( WORD handle, WORD owflag, WORD work_out[] );
WORD v_contourfill( WORD handle, WORD x, WORD y, WORD index );
WORD vsf_perimeter( WORD handle, WORD per_vis );
WORD v_get_pixel( WORD handle, WORD x, WORD y, WORD *pel, WORD *index );
WORD vst_effects( WORD handle, WORD effect );
WORD vst_point( WORD handle, WORD point, WORD *char_width, WORD *char_height, 
                WORD *cell_width, WORD *cell_height );
WORD vsl_ends( WORD handle, WORD beg_style, WORD end_style);
WORD vsc_form( WORD handle, WORD *cur_form );
WORD vsf_udpat( WORD handle, WORD fill_pat[], WORD planes );
WORD vsl_udsty( WORD handle, WORD pattern );
WORD vr_recfl( WORD handle, WORD *xy );
WORD vqin_mode( WORD handle, WORD dev_type, WORD *mode );
WORD vqt_extent( WORD handle, BYTE string[], WORD extent[] );
WORD vqt_width( WORD handle, BYTE character, WORD *cell_width, 
                WORD *left_delta, WORD *right_delta );
WORD vex_timv( WORD handle, LPVOID tim_addr, LPVOID *old_addr, WORD *scale );
WORD vst_load_fonts( WORD handle, WORD select );
WORD vst_unload_fonts( WORD handle, WORD select );
WORD vrt_cpyfm( WORD handle, WORD wr_mode, WORD xy[],
                LPMFDB srcMFDB, LPMFDB desMFDB, WORD *index );
WORD v_show_c( WORD handle, WORD reset );
WORD v_hide_c( WORD handle );
WORD vq_mouse( WORD handle, WORD *status, WORD *px, WORD *py );
WORD vex_butv( WORD handle, LPVOID usercode, LPVOID *savecode );
WORD vex_motv( WORD handle, LPVOID usercode, LPVOID *savecode );
WORD vex_curv( WORD handle, LPVOID usercode, LPVOID *savecode );
WORD vq_key_s( WORD handle, WORD *status );
WORD vs_clip( WORD handle, WORD clip_flag, WORD xy[] );
WORD vqt_name( WORD handle, WORD element_num, BYTE name[] );
WORD vqt_font_info( WORD handle, WORD *minADE, WORD *maxADE, 
                    WORD distances[], WORD *maxwidth, WORD effects[] );

/* The following functions are GEM/3 only*/

VOID v_copies( WORD handle, WORD count);
VOID v_etext(  WORD handle, WORD x, WORD y, UBYTE string[], WORD offsets[]);
VOID v_orient( WORD handle, WORD orientation );
VOID v_tray(   WORD handle, WORD tray );

WORD v_xbit_image( WORD handle,     BYTE *filename,  WORD aspect, WORD x_scale, 
                   WORD y_scale,    WORD h_align,    WORD v_align, WORD rotate, 
                   WORD background, WORD foreground, WORD xy[] );
WORD vst_ex_load_fonts( WORD handle, WORD select, WORD font_max, WORD font_free );

/* These appeared in the GEM PTK v3.1 */

VOID v_ps_halftone(WORD handle, WORD index, WORD angle, WORD frequency);
WORD shel_get(BYTE FAR *pbuffer, WORD len);
WORD shel_put(BYTE FAR *pbuffer, WORD len);
VOID v_setrgbi(WORD handle, WORD primtype, WORD r, WORD g, WORD b, WORD i);
VOID v_topbot(WORD handle, WORD height, WORD FAR *char_width, 
                 WORD FAR *char_height, WORD FAR *cell_width, 
                 WORD FAR *cell_height);
VOID vs_bkcolor(WORD handle, WORD color);
VOID v_set_app_buff(LPVOID address, WORD nparagraphs);
WORD v_bez_on(WORD handle);
WORD v_bez_off(WORD handle);
WORD v_bez_qual(WORD handle, WORD prcnt);
VOID v_pat_rotate(WORD handle, WORD angle);
VOID vs_grayoverride(WORD handle, WORD grayval);
VOID v_bez(WORD handle, WORD count, LPWORD xyarr, UBYTE FAR *bezarr, 
           LPWORD minmax, LPWORD npts, LPWORD nmove);
VOID v_bezfill(WORD handle, WORD count, LPWORD xyarr, UBYTE FAR *bezarr, 
               LPWORD minmax, LPWORD npts, LPWORD nmove);

GLOBAL VOID fix_icon       (WORD vdi_handle, LPTREE tree);
GLOBAL VOID trans_gimage   (WORD vdi_handle, LPTREE tree, WORD obj);


#endif /* ndef __WCCGEM_H__ */
