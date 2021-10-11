
/* deskapp.c */
ANODE *app_afind(WORD isdesk, WORD atype, WORD obid, 
				BYTE *pname, WORD *pisapp);
EXTERN VOID	app_tran(WORD bi_num);
EXTERN WORD app_save(WORD todisk);
BYTE *save_str(BYTE *pcurr, BYTE *pstr);
VOID app_detect(VOID);
WORD app_getfh(WORD openit, BYTE *pname, WORD attr);
BYTE *app_parse(BYTE *pcurr, ANODE *pa);
BYTE *scan_2(BYTE *pcurr, WORD *pwd);
BYTE *save_2(BYTE *pcurr, UWORD wd);
VOID strlcpy(char *dest, char *src, int maxlen);
VOID strlcat(char *dest, char *src, int maxlen);
VOID lstlcpy(LPBYTE dest, LPBYTE src, int maxlen);

/* desktop.c */
WORD hndl_msg(VOID);
WORD hndl_kbd(WORD key);
WORD hndl_button(WORD clicks, WORD mx, WORD my, WORD button, WORD keystate);
VOID wind_setl(WORD hw, WORD var, LPVOID data);
VOID cnx_put(VOID);

ANODE *i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp);
WORD hndl_menu(WORD title, WORD item);
WORD act_chkobj(LPTREE tree, WORD root, WORD obj, WORD mx, WORD my, WORD w, WORD h);
VOID r_set(GRECT *r, WORD x, WORD y, WORD w, WORD h);
VOID r_get(GRECT *r, WORD *x, WORD *y, WORD *w, WORD *h);
WNODE *win_ith(WORD i);
VOID men_list(LPTREE mlist, BYTE *dlist, WORD enable);

/* deskdir.c */
VOID show_hide(WORD fmd, LPTREE tree);

WORD d_doop(WORD op, LPTREE tree, WORD obj, BYTE *psrc_path, BYTE *pdst_path, 
		WORD srcpthlen, WORD dstpthlen,	
		WORD *pfcnt, WORD *pdcnt, WORD flag);

/* deskgraf.c */

WORD gsx_sclip(GRECT *pt);
VOID gsx_gclip(GRECT *pt);
VOID gsx_xline( WORD ptscount, WORD *ppoints );
VOID gsx_pline( WORD offx, WORD offy, WORD cnt, WORD *pts);
VOID gsx_attr(UWORD text, UWORD mode, UWORD color);
VOID gsx_fix(MFDB *pfd, LPVOID theaddr, WORD wb, WORD h);
VOID gsx_blt(LPVOID saddr, UWORD sx, UWORD sy, UWORD swb, 
			 LPVOID daddr, UWORD dx, UWORD dy, UWORD dwb, 
			 UWORD w, UWORD h, UWORD rule, WORD fgcolor, WORD bgcolor);
VOID bb_screen(WORD scrule, WORD scsx, WORD scsy, WORD scdx, 
		       WORD scdy, WORD scw, WORD sch);
VOID gsx_trans(LPVOID saddr, UWORD swb, LPVOID daddr, UWORD dwb, UWORD h);
VOID gsx_start(void);
VOID gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc, BYTE *text);
VOID bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, 
			 WORD hy, WORD hw, WORD hh);

/* deskapp.c */
BYTE get_defdrv(ULONG dr_exist, ULONG dr_hard);
BYTE app_blddesk();
ANODE *app_alloc(WORD tohead);
void   app_free(ANODE *pa);
BYTE *scan_str(BYTE *pcurr, BYTE **ppstr);
WORD app_start(VOID);

/* desk1.c */
WORD true_closewnd(WNODE *pw);
WORD fun_close(WNODE *pw, WORD hotclose);
VOID show_dial(LPTREE tree);
VOID snap_disk(WORD x, WORD y, LPWORD x1, LPWORD y1);
WORD desk1_drag(WORD wh, WORD dest_wh, WORD root, WORD dobj, WORD mx, WORD my);


/* deskact.c */
WORD act_chg(WORD wh, LPTREE tree, WORD root, WORD obj, GRECT *pc, 
			UWORD chgvalue, WORD dochg, WORD dodraw, WORD chkdisabled);
VOID act_bsclick(WORD wh, LPTREE tree, WORD root, WORD mx, WORD my, 
				WORD keystate, GRECT *pc, WORD dclick);
WORD act_bdown(WORD wh, LPTREE tree, WORD root, 
				WORD *in_mx, WORD *in_my, WORD keystate, 
				GRECT *pc, WORD *pdobj);
VOID act_allchg(WORD wh, LPTREE tree, WORD root, WORD ex_obj, 
			GRECT *pt, GRECT *pc, 
			WORD chgvalue, WORD dochg, WORD dodraw);

/* deskdir.c */
VOID add_fname(BYTE *path, BYTE *new_name, WORD buflen);
VOID del_fname(BYTE *pstr);
WORD d_errmsg(VOID);
WORD par_chk(BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path);
WORD dir_op(WORD op, BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path, 
			WORD *pfcnt, WORD *pdcnt, LONG *psize,
       		WORD dulx, WORD duly, WORD from_disk, WORD src_ob);
BYTE *scan_slsh(BYTE *path);

/* deskdraw.c */
VOID draw_fld(LPTREE tree, WORD obj);


/* deskfpd.c */
VOID fn_init(VOID);
VOID pn_init(VOID);
VOID fpd_start(VOID);
WORD fpd_bldspec(WORD drive, BYTE *ppath, BYTE *pname, 
				BYTE *pext, BYTE *pspec);
VOID fpd_parse(BYTE *pspec, WORD *pdrv, BYTE *ppath, 
				BYTE *pname, BYTE *pext);
FNODE *fpd_ofind(FNODE *pf, WORD obj);
BYTE  *fpd_elist(FNODE *pfpd, FNODE *pstop);
VOID fn_free(FNODE *thefile);
VOID fl_free(FNODE *pflist);
FNODE *fn_alloc(VOID);
PNODE *pn_alloc(VOID);
VOID pn_free(PNODE *thepath);
VOID pn_close(PNODE *thepath);
PNODE *pn_open(WORD drive, BYTE *path, BYTE *name, BYTE *ext, 
				WORD attr);
WORD pn_fcomp(FNODE *pf1, FNODE *pf2, WORD which);
WORD pn_comp(FNODE *pf1, FNODE *pf2);
FNODE *pn_sort(WORD lstcnt, FNODE *pflist);
WORD pn_folder(PNODE *thepath);
WORD pn_desktop(PNODE *thepath);
WORD pn_active(PNODE *thepath);

/* deskfun.c */
WORD fun_alert(WORD defbut, WORD stnum, ...);
VOID fun_msg(WORD type, WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
VOID fun_rebld(WNODE *pwin);
WORD fun_mkdir(WNODE *pw_node);
WORD fun_op(WORD op, PNODE *pspath, BYTE *pdest, 
			WORD dulx, WORD duly, WORD from_disk, WORD src_ob);
WORD fun_wdst(PNODE *pspath, BYTE *pdspec, WORD datype, FNODE *pdf, 
			  WORD dulx, WORD duly, WORD from_disk, WORD src_ob, 
			  WORD *pdo_both);
WORD fun_disk(WORD src_ob, WNODE *pdw, WORD datype, FNODE *pdf, 
			  WORD dulx, WORD duly);
VOID fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly);
VOID fun_del(WNODE *pdw);
WORD cmp_names(BYTE *psrc, BYTE *pdst);


/* deskgsx.c */
VOID gsx_vopen(VOID);
VOID gsx_vclose(VOID);

/* deskif.c */
VOID gsx_moff(VOID);
VOID gsx_mon(VOID);

/* deskinf.c */
WORD inf_show(LPTREE tree, WORD start);
WORD inf_file(BYTE *ppath, FNODE *pfnode);
WORD inf_folder(BYTE *ppath, FNODE *pf);
WORD inf_disk(BYTE dr_id);
WORD dr_code(LPPARM pparms);
WORD inf_pref(VOID);
WORD opn_appl(BYTE *papname, BYTE *papparms, BYTE *pcmd, BYTE *ptail,
		WORD taillen);


/* deskins.c */
WORD is_installed(ANODE *pa);
WORD ins_disk(ANODE *pa);
VOID ob_relxywh(LPTREE tree, WORD obj, GRECT *prect);
VOID ob_actxywh(LPTREE tree, WORD obj, GRECT *p);
WORD ins_app(BYTE *pfname, ANODE *pa);
VOID ins_posdisk(WORD dx, WORD dy, WORD *pdx, WORD *pdy);

/* deskobj.c */
VOID obj_init(VOID);
VOID obj_wfree(WORD obj, WORD x, WORD y, WORD w, WORD h);
WORD obj_walloc(WORD x, WORD y, WORD w, WORD h);
WORD obj_ialloc(WORD wparent, WORD x, WORD y, WORD w, WORD h);

/* deskpro.c */

VOID pro_chcalc(LONG appsize, LONG *begaddr, LONG *chsize);
WORD pro_chdir(WORD drv, BYTE *ppath);
WORD pro_cmd(BYTE *psubcmd, BYTE *psubtail, WORD exitflag);
WORD pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr);
WORD pro_exec(WORD isgraf, WORD isover, LPBYTE pcmd, LPBYTE ptail);
WORD pro_exit(LPBYTE pcmd, LPBYTE ptail);

/* deskrsrc.c */
VOID rsrc_init(VOID);
BYTE *ini_str(WORD stnum);

/* desksupp.c */
LPICON get_spec(OBJECT *olist, WORD obj);
VOID do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, 
			  WORD w, WORD h);
WORD do_diropen(WNODE *pw, WORD new_win, WORD curr_icon, WORD drv, 
				BYTE *ppath, BYTE *pname, BYTE *pext, GRECT *pt,
				WORD redraw);
WORD do_info(WORD curr);
WORD alert_s(WORD defbut, WORD alert_num, BYTE *s);
WORD menu_item_to_alert_s( WORD def_but, WORD alert_num, WORD item );

VOID desk_clear(WORD wh);
VOID desk_verify(WORD wh, WORD changed);
WORD do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc);
VOID get_xywh(OBJECT *olist, WORD obj, WORD *px, WORD *py, WORD *pw, WORD *ph);
LPICON get_spec(OBJECT *olist, WORD obj);
VOID do_xyfix(WORD *px, WORD *py);
VOID do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h);
VOID do_wfull(WORD wh);
WORD do_aopen(ANODE *pa, WORD isapp, WORD curr, WORD drv, 
			  BYTE *ppath, BYTE *pname);
WORD do_dopen(WORD curr);
VOID do_fopen(WNODE *pw, WORD curr, WORD drv, BYTE *ppath, 
			  BYTE *pname, BYTE *pext, WORD chkall, WORD redraw);
WORD do_open(WORD curr);
WORD do_info(WORD curr);
VOID romerr(WORD curr);
VOID do_format(WORD curr);
VOID do_chkall(WORD redraw);

/* desktype.c */
WORD do_type(WORD curr);

/* deskwin.c */
LPBYTE win_iname(WORD curr);
VOID win_sname(WNODE *pw);
VOID win_bldview(WNODE *pwin, WORD x, WORD y, WORD w, WORD h);
WORD do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc);
WORD create_window( WORD wkind, BYTE * title, BYTE * info, GRECT *work );


VOID win_view(WORD vtype, WORD isort);
VOID win_start(VOID);
VOID win_free(WNODE *thewin);
WNODE *win_alloc(WORD obid);
WNODE *win_find(WORD wh);
VOID win_top(WNODE *thewin);
WNODE *win_ontop(VOID);
WORD win_cnt(WORD level);
WNODE *win_ith(WORD level);
VOID win_ocalc(WNODE *pwin, WORD wfit, WORD hfit, FNODE **ppstart);
VOID win_icalc(FNODE *pfnode);
VOID win_blt(WNODE *pw, BOOLEAN vertical, WORD newcv);
//VOID win_slide(WORD wh, WORD sl_value); // DESKTOP v1.2 has 2 sliders...
VOID win_slide(WORD wh, WORD sl_value, WORD vertical);
VOID win_arrow(WORD wh, WORD arrow_type);
VOID win_srtall(VOID);
VOID win_bdall(VOID);
VOID win_shwall(VOID);
WORD win_isel(OBJECT *olist, WORD root, WORD curr);
LPBYTE win_iname(WORD curr);
VOID win_sname(WNODE *pw);
VOID win_sinfo(WNODE *pw);

/* optimize.c */

/* use v_sound() WORD sound(WORD isfreq, WORD freq, WORD dura); */
WORD bit_num(UWORD flag);
VOID rc_union    (LPGRECT p1, LPGRECT p2);
VOID rc_constrain(LPGRECT pc, LPGRECT pt);
WORD rc_intersect(LPGRECT p1, LPGRECT p2);
WORD mid(WORD lo, WORD val, WORD hi);
BYTE *strscn(BYTE *ps, BYTE *pd, BYTE stop);
VOID fmt_str(BYTE *instr, BYTE *outstr);
VOID unfmt_str(BYTE *instr, BYTE *outstr);
VOID fs_sset(LPTREE tree, WORD obj, LPBYTE pstr, 
			 LPBYTE *ptext, WORD *ptxtlen);
VOID inf_sset(LPTREE tree, WORD obj, BYTE *pstr);
VOID fs_sget(LPTREE tree, WORD obj, LPBYTE pstr, WORD maxlen);
VOID inf_sget(LPTREE tree, WORD obj, BYTE *pstr, WORD maxlen);
VOID inf_fldset(LPTREE tree, WORD obj, 
		UWORD testfld, UWORD testbit, UWORD truestate, UWORD falsestate);
WORD inf_gindex(LPTREE tree, WORD baseobj, WORD numobj);
WORD inf_what(LPTREE tree, WORD ok, WORD cncl);
VOID merge_v(BYTE *pdst, BYTE *ptmp, va_list ap);
VOID merge_str(BYTE *pdst, BYTE *ptmp, ...);
WORD wildcmp(BYTE *pwld, BYTE *ptst);
VOID ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len);

/* optimopt.a86 */
UWORD inside(WORD x, WORD y, GRECT *pt);

WORD wmax(WORD a, WORD b);
WORD wmin(WORD a, WORD b);

/* deskosif.a86 */
VOID takedos(VOID);
VOID takekey(VOID);
VOID takevid(VOID);

VOID givedos(VOID);
VOID givekey(VOID);
VOID givevid(VOID);

WORD desk_sound(WORD isfreq, WORD freq, WORD duration);

char   *scasb(char *s, char b);
VOID dos_label(BYTE drive, BYTE *plabel);
WORD dos_dtype(WORD drive);
BOOLEAN cd_isdrvcd (BYTE drive);
#if DEBUG
WORD form_valert(WORD button, BYTE *str, ...);
WORD dbg(BYTE *str, ...);
VOID crashdump(BYTE ch);
VOID debugbreak(VOID);
#endif



