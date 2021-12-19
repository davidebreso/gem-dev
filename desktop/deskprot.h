
/* deskapp.c */
ANODE * __near app_afind(WORD isdesk, WORD atype, WORD obid, 
				BYTE *pname, WORD *pisapp);
EXTERN VOID __near app_tran(WORD bi_num);
EXTERN WORD __near app_save(WORD todisk);
BYTE * __near save_str(BYTE *pcurr, BYTE *pstr);
VOID __near app_detect(VOID);
WORD __near app_getfh(WORD openit, BYTE *pname, WORD attr);
BYTE * __near app_parse(BYTE *pcurr, ANODE *pa);
BYTE * __near scan_2(BYTE *pcurr, WORD *pwd);
BYTE * __near save_2(BYTE *pcurr, UWORD wd);
BYTE * __near save_memsz(BYTE *pcurr, UWORD wd);
BYTE * __near scan_memsz(BYTE *pcurr, UWORD *pwd);
// VOID strlcpy(char *dest, char *src, int maxlen);
// VOID strlcat(char *dest, char *src, int maxlen);
VOID __near lstlcpy(LPBYTE dest, LPBYTE src, int maxlen);

/* desktop.c */
WORD __near hndl_msg(VOID);
WORD __near hndl_kbd(WORD key);
WORD __near hndl_button(WORD clicks, WORD mx, WORD my, WORD button, WORD keystate);
VOID __near wind_setl(WORD hw, WORD var, LPVOID data);
VOID __near cnx_put(VOID);

ANODE * __near i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp);
WORD __near hndl_menu(WORD title, WORD item);
WORD __near act_chkobj(LPTREE tree, WORD root, WORD obj, WORD mx, WORD my, WORD w, WORD h);
VOID __near r_set(GRECT *r, WORD x, WORD y, WORD w, WORD h);
VOID __near r_get(GRECT *r, WORD *x, WORD *y, WORD *w, WORD *h);
WNODE * __near win_ith(WORD i);
VOID __near men_list(LPTREE mlist, BYTE *dlist, WORD enable);

/* deskdir.c */
VOID __near show_hide(WORD fmd, LPTREE tree);

WORD __near d_doop(WORD op, LPTREE tree, WORD obj, BYTE *psrc_path, BYTE *pdst_path, 
		WORD srcpthlen, WORD dstpthlen,	
		WORD *pfcnt, WORD *pdcnt, WORD flag);

/* deskgraf.c */

WORD __near gsx_sclip(GRECT *pt);
VOID __near gsx_gclip(GRECT *pt);
VOID __near gsx_xline( WORD ptscount, WORD *ppoints );
VOID __near gsx_pline( WORD offx, WORD offy, WORD cnt, WORD *pts);
VOID __near gsx_attr(UWORD text, UWORD mode, UWORD color);
VOID __near gsx_fix(MFDB *pfd, LPVOID theaddr, WORD wb, WORD h);
VOID __near gsx_blt(LPVOID saddr, UWORD sx, UWORD sy, UWORD swb, 
			 LPVOID daddr, UWORD dx, UWORD dy, UWORD dwb, 
			 UWORD w, UWORD h, UWORD rule, WORD fgcolor, WORD bgcolor);
VOID __near bb_screen(WORD scrule, WORD scsx, WORD scsy, WORD scdx, 
		       WORD scdy, WORD scw, WORD sch);
VOID __near gsx_trans(LPVOID saddr, UWORD swb, LPVOID daddr, UWORD dwb, UWORD h);
VOID __near gsx_start(void);
VOID __near gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc, BYTE *text);
VOID __near bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, 
			 WORD hy, WORD hw, WORD hh);

/* deskapp.c */
BYTE __near get_defdrv(ULONG dr_exist, ULONG dr_hard);
BYTE __near app_blddesk();
ANODE * __near app_alloc(WORD tohead);
void  __near app_free(ANODE *pa);
BYTE * __near scan_str(BYTE *pcurr, BYTE **ppstr);
WORD __near app_start(VOID);
WORD __near hex_dig(BYTE achar);

/* desk1.c */
WORD __near true_closewnd(WNODE *pw);
WORD __near fun_close(WNODE *pw, WORD hotclose);
VOID __near show_dial(LPTREE tree);
VOID __near snap_disk(WORD x, WORD y, LPWORD x1, LPWORD y1);
WORD __near desk1_drag(WORD wh, WORD dest_wh, WORD root, WORD dobj, WORD mx, WORD my);


/* deskact.c */
WORD __near act_chg(WORD wh, LPTREE tree, WORD root, WORD obj, GRECT *pc, 
			UWORD chgvalue, WORD dochg, WORD dodraw, WORD chkdisabled);
VOID __near act_bsclick(WORD wh, LPTREE tree, WORD root, WORD mx, WORD my, 
				WORD keystate, GRECT *pc, WORD dclick);
WORD __near act_bdown(WORD wh, LPTREE tree, WORD root, 
				WORD *in_mx, WORD *in_my, WORD keystate, 
				GRECT *pc, WORD *pdobj);
VOID __near act_allchg(WORD wh, LPTREE tree, WORD root, WORD ex_obj, 
			GRECT *pt, GRECT *pc, 
			WORD chgvalue, WORD dochg, WORD dodraw);

/* deskdir.c */
VOID __near add_fname(BYTE *path, BYTE *new_name, WORD buflen);
VOID __near del_fname(BYTE *pstr);
WORD __near d_errmsg(VOID);
WORD __near par_chk(BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path);
WORD __near dir_op(WORD op, BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path, 
			WORD *pfcnt, WORD *pdcnt, LONG *psize,
       		WORD dulx, WORD duly, WORD from_disk, WORD src_ob);
BYTE * __near scan_slsh(BYTE *path);

/* deskdraw.c */
VOID __near draw_fld(LPTREE tree, WORD obj);


/* deskfpd.c */
VOID __near fn_init(VOID);
VOID __near pn_init(VOID);
VOID __near fpd_start(VOID);
WORD __near fpd_bldspec(WORD drive, BYTE *ppath, BYTE *pname, 
				BYTE *pext, BYTE *pspec);
VOID __near fpd_parse(BYTE *pspec, WORD *pdrv, BYTE *ppath, 
				BYTE *pname, BYTE *pext);
FNODE * __near fpd_ofind(FNODE *pf, WORD obj);
BYTE  * __near fpd_elist(FNODE *pfpd, FNODE *pstop);
VOID __near fn_free(FNODE *thefile);
VOID __near fl_free(FNODE *pflist);
FNODE * __near fn_alloc(VOID);
PNODE * __near pn_alloc(VOID);
VOID __near pn_free(PNODE *thepath);
VOID __near pn_close(PNODE *thepath);
PNODE * __near pn_open(WORD drive, BYTE *path, BYTE *name, BYTE *ext, 
				WORD attr);
WORD __near pn_fcomp(FNODE *pf1, FNODE *pf2, WORD which);
WORD __near pn_comp(FNODE *pf1, FNODE *pf2);
FNODE * __near pn_sort(WORD lstcnt, FNODE *pflist);
WORD __near pn_folder(PNODE *thepath);
WORD __near pn_desktop(PNODE *thepath);
WORD __near pn_active(PNODE *thepath);

/* deskfun.c */
WORD __near fun_alert(WORD defbut, WORD stnum, ...);
VOID __near fun_msg(WORD type, WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
VOID __near fun_rebld(WNODE *pwin);
WORD __near fun_mkdir(WNODE *pw_node);
WORD __near fun_op(WORD op, PNODE *pspath, BYTE *pdest, 
			WORD dulx, WORD duly, WORD from_disk, WORD src_ob);
WORD __near fun_wdst(PNODE *pspath, BYTE *pdspec, WORD datype, FNODE *pdf, 
			  WORD dulx, WORD duly, WORD from_disk, WORD src_ob, 
			  WORD *pdo_both);
WORD __near fun_disk(WORD src_ob, WNODE *pdw, WORD datype, FNODE *pdf, 
			  WORD dulx, WORD duly);
VOID __near fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly);
VOID __near fun_del(WNODE *pdw);
WORD __near cmp_names(BYTE *psrc, BYTE *pdst);


/* deskgsx.c */
VOID __near gsx_vopen(VOID);
VOID __near gsx_vclose(VOID);

/* deskif.c */
VOID __near gsx_moff(VOID);
VOID __near gsx_mon(VOID);

/* deskinf.c */
WORD __near inf_show(LPTREE tree, WORD start);
WORD __near inf_file(BYTE *ppath, FNODE *pfnode);
WORD __near inf_folder(BYTE *ppath, FNODE *pf);
WORD __near inf_disk(BYTE dr_id);
WORD dr_code(LPPARM pparms);
WORD __near inf_pref(VOID);
WORD __near opn_appl(BYTE *papname, BYTE *papparms, BYTE *pcmd, BYTE *ptail,
		WORD taillen);


/* deskins.c */
WORD __near is_installed(ANODE *pa);
WORD __near ins_disk(ANODE *pa);
VOID __near ob_relxywh(LPTREE tree, WORD obj, GRECT *prect);
VOID __near ob_actxywh(LPTREE tree, WORD obj, GRECT *p);
WORD __near ins_app(BYTE *pfname, ANODE *pa);
VOID __near ins_posdisk(WORD dx, WORD dy, WORD *pdx, WORD *pdy);

/* deskobj.c */
VOID __near obj_init(VOID);
VOID __near obj_wfree(WORD obj, WORD x, WORD y, WORD w, WORD h);
WORD __near obj_walloc(WORD x, WORD y, WORD w, WORD h);
WORD __near obj_ialloc(WORD wparent, WORD x, WORD y, WORD w, WORD h);

/* deskpro.c */

VOID __near pro_chcalc(LONG appsize, LPBYTE *begaddr, LONG *chsize);
WORD __near pro_chdir(WORD drv, BYTE *ppath);
WORD __near pro_cmd(BYTE *psubcmd, BYTE *psubtail, WORD exitflag);
WORD __near pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr);
WORD __near pro_exec(WORD isgraf, WORD isover, LPBYTE pcmd, LPBYTE ptail);
WORD __near pro_exit(LPBYTE pcmd, LPBYTE ptail);

/* deskrsrc.c */
VOID __near rsrc_init(VOID);
BYTE * __near ini_str(WORD stnum);

/* desksupp.c */
LPICON __near get_spec(OBJECT *olist, WORD obj);
VOID __near do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, 
			  WORD w, WORD h);
WORD __near do_diropen(WNODE *pw, WORD new_win, WORD curr_icon, WORD drv, 
				BYTE *ppath, BYTE *pname, BYTE *pext, GRECT *pt,
				WORD redraw);
WORD __near do_info(WORD curr);
WORD __near alert_s(WORD defbut, WORD alert_num, BYTE *s);
WORD __near menu_item_to_alert_s( WORD def_but, WORD alert_num, WORD item );

VOID __near desk_clear(WORD wh);
VOID __near desk_verify(WORD wh, WORD changed);
WORD __near do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc);
VOID __near get_xywh(OBJECT *olist, WORD obj, WORD *px, WORD *py, WORD *pw, WORD *ph);
LPICON __near get_spec(OBJECT *olist, WORD obj);
VOID __near do_xyfix(WORD *px, WORD *py);
VOID __near do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h);
VOID __near do_wfull(WORD wh);
WORD __near do_aopen(ANODE *pa, WORD isapp, WORD curr, WORD drv, 
			  BYTE *ppath, BYTE *pname);
WORD __near do_dopen(WORD curr);
VOID __near do_fopen(WNODE *pw, WORD curr, WORD drv, BYTE *ppath, 
			  BYTE *pname, BYTE *pext, WORD chkall, WORD redraw);
WORD __near do_open(WORD curr);
WORD __near do_info(WORD curr);
VOID __near romerr(WORD curr);
VOID __near do_format(WORD curr);
VOID __near do_chkall(WORD redraw);

/* desktype.c */
WORD __near do_type(WORD curr);

/* deskwin.c */
LPBYTE __near win_iname(WORD curr);
VOID __near win_sname(WNODE *pw);
VOID __near win_bldview(WNODE *pwin, WORD x, WORD y, WORD w, WORD h);
WORD __near do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc);
WORD __near create_window( WORD wkind, BYTE * title, BYTE * info, GRECT *work );


VOID __near win_view(WORD vtype, WORD isort);
VOID __near win_start(VOID);
VOID __near win_free(WNODE *thewin);
WNODE * __near win_alloc(WORD obid);
WNODE * __near win_find(WORD wh);
VOID __near win_top(WNODE *thewin);
WNODE * __near win_ontop(VOID);
WORD __near win_cnt(WORD level);
WNODE * __near win_ith(WORD level);
VOID __near win_ocalc(WNODE *pwin, WORD wfit, WORD hfit, FNODE **ppstart);
VOID __near win_icalc(FNODE *pfnode);
VOID __near win_blt(WNODE *pw, BOOLEAN vertical, WORD newcv);
//VOID win_slide(WORD wh, WORD sl_value); // DESKTOP v1.2 has 2 sliders...
VOID __near win_slide(WORD wh, WORD sl_value, WORD vertical);
VOID __near win_arrow(WORD wh, WORD arrow_type);
VOID __near win_srtall(VOID);
VOID __near win_bdall(VOID);
VOID __near win_shwall(VOID);
WORD __near win_isel(OBJECT *olist, WORD root, WORD curr);
LPBYTE __near win_iname(WORD curr);
VOID __near win_sname(WNODE *pw);
VOID __near win_sinfo(WNODE *pw);

/* optimize.c */

/* use v_sound() WORD sound(WORD isfreq, WORD freq, WORD dura); */
WORD __near bit_num(UWORD flag);
VOID __near rc_union    (LPGRECT p1, LPGRECT p2);
VOID __near rc_constrain(LPGRECT pc, LPGRECT pt);
WORD rc_intersect(LPGRECT p1, LPGRECT p2);
WORD __near mid(WORD lo, WORD val, WORD hi);
BYTE * __near strscn(BYTE *ps, BYTE *pd, BYTE stop);
VOID __near fmt_str(BYTE *instr, BYTE *outstr);
VOID __near unfmt_str(BYTE *instr, BYTE *outstr);
VOID __near fs_sset(LPTREE tree, WORD obj, LPBYTE pstr, 
			 LPBYTE *ptext, WORD *ptxtlen);
VOID __near inf_sset(LPTREE tree, WORD obj, BYTE *pstr);
VOID __near fs_sget(LPTREE tree, WORD obj, LPBYTE pstr, WORD maxlen);
VOID __near inf_sget(LPTREE tree, WORD obj, BYTE *pstr, WORD maxlen);
VOID __near inf_fldset(LPTREE tree, WORD obj, 
		UWORD testfld, UWORD testbit, UWORD truestate, UWORD falsestate);
WORD __near inf_gindex(LPTREE tree, WORD baseobj, WORD numobj);
WORD __near inf_what(LPTREE tree, WORD ok, WORD cncl);
VOID __near merge_v(BYTE *pdst, BYTE *ptmp, va_list ap);
VOID __near merge_str(BYTE *pdst, BYTE *ptmp, ...);
WORD __near wildcmp(BYTE *pwld, BYTE *ptst);
VOID __near ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len);

/* optimopt.a86 */
UWORD __near inside(WORD x, WORD y, GRECT *pt);

WORD __near wmax(WORD a, WORD b);
WORD __near wmin(WORD a, WORD b);

/* deskosif.a86 */
VOID __near takedos(VOID);
VOID __near takekey(VOID);
VOID __near takevid(VOID);

VOID __near givedos(VOID);
VOID __near givekey(VOID);
VOID __near givevid(VOID);

WORD __near desk_sound(WORD isfreq, WORD freq, WORD duration);

char   *scasb(char *s, char b);
VOID __near dos_label(BYTE drive, BYTE *plabel);
WORD __near dos_dtype(WORD drive);
BOOLEAN __near cd_isdrvcd (BYTE drive);
#if DEBUG
WORD __near form_valert(WORD button, BYTE *str, ...);
WORD __near dbg(BYTE *str, ...);
VOID __near crashdump(BYTE ch);
VOID __near debugbreak(VOID);
#endif


#if MULTIAPP
VOID __near iac_init(VOID);
VOID __near iac_strcop(LPTREE tree, WORD obj, LPBYTE src);
VOID __near iac_schar(LPTREE tree, WORD obj, BYTE ch);
VOID __near iac_redrw(LPTREE tree, WORD obj, WORD state, WORD depth);
WORD __near iac_isnam(LPBYTE lst);
VOID __near ins_acc(VOID);
WORD __near iac_dial(LPTREE tree);
VOID __near iac_save(LPTREE tree);
WORD __near iac_names(LPTREE tree);
VOID __near iac_elev(LPTREE tree, WORD currtop, WORD count);
WORD __near iac_comp(VOID);
VOID __near iac_mvnames(LPTREE tree, WORD start, WORD num);
#endif


