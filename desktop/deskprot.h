
/* deskapp.c */
ANODE *  app_afind(WORD isdesk, WORD atype, WORD obid, 
 				BYTE *pname, WORD *pisapp);
EXTERN VOID  app_tran(WORD bi_num);
EXTERN WORD  app_save(WORD todisk);
VOID  app_detect(VOID);
BYTE  app_blddesk();
ANODE *  app_alloc(WORD tohead);
VOID app_free(ANODE *pa);
BYTE *  scan_str(BYTE *pcurr, BYTE **ppstr);
WORD  app_start(VOID);

/* desktop.c */
WORD  hndl_msg(VOID);
WORD  hndl_button(WORD clicks, WORD mx, WORD my, WORD button, WORD keystate);
WORD  hndl_kbd(WORD thechar);
WORD  hndl_menu(WORD title, WORD item);
VOID  wind_setl(WORD hw, WORD var, LPVOID data);
ANODE *  i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp);
VOID  men_list(LPTREE mlist, BYTE *dlist, WORD enable);

/* deskdir.c */
VOID  show_hide(WORD fmd, LPTREE tree);

WORD  d_doop(WORD op, LPTREE tree, WORD obj, BYTE *psrc_path, BYTE *pdst_path, 
		WORD srcpthlen, WORD dstpthlen,	
		WORD *pfcnt, WORD *pdcnt, WORD flag);
VOID  add_fname(BYTE *path, BYTE *new_name, WORD buflen);
VOID  del_fname(BYTE *pstr);
WORD  d_errmsg(VOID);
WORD  par_chk(BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path);
WORD  dir_op(WORD op, BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path, 
			WORD *pfcnt, WORD *pdcnt, LONG *psize,
       		WORD dulx, WORD duly, WORD from_disk, WORD src_ob);
BYTE *  scan_slsh(BYTE *path);
VOID  draw_fld(LPTREE tree, WORD obj);

/* deskgraf.c */

WORD  gsx_sclip(GRECT *pt);
VOID  gsx_gclip(GRECT *pt);
// VOID  gsx_xline( WORD ptscount, WORD *ppoints );
VOID  gsx_pline( WORD offx, WORD offy, WORD cnt, WORD *pts);
VOID  gsx_attr(UWORD text, UWORD mode, UWORD color);
VOID  gsx_fix(MFDB *pfd, LPVOID theaddr, WORD wb, WORD h);
// VOID  gsx_blt(LPVOID saddr, UWORD sx, UWORD sy, UWORD swb, 
// 			 LPVOID daddr, UWORD dx, UWORD dy, UWORD dwb, 
// 			 UWORD w, UWORD h, UWORD rule, WORD fgcolor, WORD bgcolor);
VOID  bb_screen(WORD scrule, WORD scsx, WORD scsy, WORD scdx, 
		       WORD scdy, WORD scw, WORD sch);
VOID  gsx_trans(LPVOID saddr, UWORD swb, LPVOID daddr, UWORD dwb, UWORD h);
VOID  gsx_start(void);
VOID  gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc, BYTE *text);
VOID  bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, 
			 WORD hy, WORD hw, WORD hh);

/* desk1.c */
WORD  true_closewnd(WNODE *pw);
WORD  fun_close(WNODE *pw, WORD hotclose);
VOID  snap_disk(WORD x, WORD y, LPWORD x1, LPWORD y1);
WORD  desk1_drag(WORD wh, WORD dest_wh, WORD root, WORD dobj, WORD mx, WORD my);
WNODE *  win_ontop(VOID);


/* deskact.c */
WORD  act_chg(WORD wh, LPTREE tree, WORD root, WORD obj, GRECT *pc, 
			UWORD chgvalue, WORD dochg, WORD dodraw, WORD chkdisabled);
VOID  act_bsclick(WORD wh, LPTREE tree, WORD root, WORD mx, WORD my, 
				WORD keystate, GRECT *pc, WORD dclick);
WORD  act_bdown(WORD wh, LPTREE tree, WORD root, 
				WORD *in_mx, WORD *in_my, WORD keystate, 
				GRECT *pc, WORD *pdobj);
VOID  act_allchg(WORD wh, LPTREE tree, WORD root, WORD ex_obj, 
			GRECT *pt, GRECT *pc, 
			WORD chgvalue, WORD dochg, WORD dodraw);

/* deskfpd.c */
VOID  fpd_start(VOID);
WORD  fpd_bldspec(WORD drive, BYTE *ppath, BYTE *pname, 
				 BYTE *pext, BYTE *pspec);
VOID  fpd_parse(BYTE *pspec, WORD *pdrv, BYTE *ppath, 
				BYTE *pname, BYTE *pext);
FNODE *  fpd_ofind(FNODE *pf, WORD obj);
VOID  pn_close(PNODE *thepath);
PNODE *  pn_open(WORD drive, BYTE *path, BYTE *name, BYTE *ext, 
				WORD attr);
FNODE *  pn_sort(WORD lstcnt, FNODE *pflist);
WORD  pn_active(PNODE *thepath);

/* deskfun.c */
WORD  fun_alert(WORD defbut, WORD stnum, ...);
VOID  fun_msg(WORD type, WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
VOID  fun_rebld(WNODE *pwin);
WORD  fun_mkdir(WNODE *pw_node);
WORD  fun_op(WORD op, PNODE *pspath, BYTE *pdest, 
			WORD dulx, WORD duly, WORD from_disk, WORD src_ob);
// WORD  fun_wdst(PNODE *pspath, BYTE *pdspec, WORD datype, FNODE *pdf, 
// 			  WORD dulx, WORD duly, WORD from_disk, WORD src_ob, 
// 			  WORD *pdo_both);
// WORD  fun_disk(WORD src_ob, WNODE *pdw, WORD datype, FNODE *pdf, 
// 			  WORD dulx, WORD duly);
VOID  fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly);
// VOID  fun_del(WNODE *pdw);
// WORD  cmp_names(BYTE *psrc, BYTE *pdst);


/* deskgsx.c */
VOID  gsx_vopen(VOID);
// VOID  gsx_vclose(VOID);
VOID  gsx_moff(VOID);
VOID  gsx_mon(VOID);

/* deskinf.c */
VOID  inf_start(LPTREE tree);
VOID  inf_end(LPTREE tree);
WORD  inf_show(LPTREE tree, WORD start);
WORD  inf_file(BYTE *ppath, FNODE *pfnode);
WORD  inf_folder(BYTE *ppath, FNODE *pf);
WORD  inf_disk(BYTE dr_id);
WORD dr_code(LPPARM pparms);
WORD  inf_pref(VOID);
WORD  opn_appl(BYTE *papname, BYTE *papparms, BYTE *pcmd, BYTE *ptail,
		WORD taillen);


/* deskins.c */
WORD  is_installed(ANODE *pa);
WORD  ins_disk(ANODE *pa);
WORD  ins_app(BYTE *pfname, ANODE *pa);
VOID  ins_posdisk(WORD dx, WORD dy, WORD *pdx, WORD *pdy);
VOID  ob_actxywh();
VOID  ob_relxywh();

/* deskobj.c */
VOID  obj_init(VOID);
VOID  obj_wfree(WORD obj, WORD x, WORD y, WORD w, WORD h);
WORD  obj_walloc(WORD x, WORD y, WORD w, WORD h);
WORD  obj_ialloc(WORD wparent, WORD x, WORD y, WORD w, WORD h);

/* deskpro.c */

WORD  pro_chdir(WORD drv, BYTE *ppath);
WORD  pro_cmd(BYTE *psubcmd, BYTE *psubtail, WORD exitflag);
WORD  pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr);
WORD  pro_exit(LPBYTE pcmd, LPBYTE ptail);

/* deskrsrc.c */
VOID rsrc_init(VOID);
BYTE * ini_str(WORD stnum);

/* desksupp.c */
WORD  do_diropen(WNODE *pw, WORD new_win, WORD curr_icon, WORD drv, 
				BYTE *ppath, BYTE *pname, BYTE *pext, GRECT *pt,
				WORD redraw);
WORD  alert_s(WORD defbut, WORD alert_num, BYTE *s);
WORD  menu_item_to_alert_s( WORD def_but, WORD alert_num, WORD item );
VOID  desk_clear(WORD wh);
VOID  desk_verify(WORD wh, WORD changed);
WORD  do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc);
VOID  get_xywh(OBJECT *olist, WORD obj, WORD *px, WORD *py, WORD *pw, WORD *ph);
LPICON  get_spec(OBJECT *olist, WORD obj);
VOID  do_xyfix(WORD *px, WORD *py);
VOID  do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h);
VOID  do_wfull(WORD wh);
VOID  do_fopen(WNODE *pw, WORD curr, WORD drv, BYTE *ppath, 
			  BYTE *pname, BYTE *pext, WORD chkall, WORD redraw);
WORD  do_open(WORD curr);
WORD  do_info(WORD curr);
VOID  do_format(WORD curr);
VOID  do_chkall(WORD redraw);

/* desktype.c */
WORD  do_type(WORD curr);

/* deskwin.c */
LPBYTE  win_iname(WORD curr);
VOID  win_sname(WNODE *pw);
VOID  win_bldview(WNODE *pwin, WORD x, WORD y, WORD w, WORD h);
WORD  create_window( WORD wkind, BYTE * title, BYTE * info, GRECT *work );
VOID  win_view(WORD vtype, WORD isort);
VOID  win_start(VOID);
VOID  win_free(WNODE *thewin);
WNODE *  win_alloc(WORD obid);
WNODE *  win_find(WORD wh);
VOID  win_top(WNODE *thewin);
// WORD  win_cnt(WORD level);
WNODE *  win_ith(WORD level);
// VOID  win_ocalc(WNODE *pwin, WORD wfit, WORD hfit, FNODE **ppstart);
// VOID  win_icalc(FNODE *pfnode);
// VOID  win_blt(WNODE *pw, BOOLEAN vertical, WORD newcv);
//VOID win_slide(WORD wh, WORD sl_value); // DESKTOP v1.2 has 2 sliders...
VOID  win_slide(WORD wh, WORD sl_value, WORD vertical);
VOID  win_arrow(WORD wh, WORD arrow_type);
VOID  win_srtall(VOID);
VOID  win_bdall(VOID);
VOID  win_shwall(VOID);
WORD  win_isel(OBJECT *olist, WORD root, WORD curr);
// LPBYTE  win_iname(WORD curr);
VOID  win_sname(WNODE *pw);
VOID  win_sinfo(WNODE *pw);

/* optimize.c */

/* use v_sound() WORD sound(WORD isfreq, WORD freq, WORD dura); */
// WORD  bit_num(UWORD flag);
VOID  rc_constrain(LPGRECT pc, LPGRECT pt);
// WORD rc_intersect(LPGRECT p1, LPGRECT p2);
// WORD  mid(WORD lo, WORD val, WORD hi);
// BYTE *  strscn(BYTE *ps, BYTE *pd, BYTE stop);
VOID  fmt_str(BYTE *instr, BYTE *outstr);
VOID  unfmt_str(BYTE *instr, BYTE *outstr);
// VOID  fs_sset(LPTREE tree, WORD obj, LPBYTE pstr, 
// 			 LPBYTE *ptext, WORD *ptxtlen);
VOID  inf_sset(LPTREE tree, WORD obj, BYTE *pstr);
// VOID  fs_sget(LPTREE tree, WORD obj, LPBYTE pstr, WORD maxlen);
VOID  inf_sget(LPTREE tree, WORD obj, BYTE *pstr, WORD maxlen);
VOID  inf_fldset(LPTREE tree, WORD obj, 
		UWORD testfld, UWORD testbit, UWORD truestate, UWORD falsestate);
WORD  inf_gindex(LPTREE tree, WORD baseobj, WORD numobj);
WORD  inf_what(LPTREE tree, WORD ok, WORD cncl);
WORD  wildcmp(BYTE *pwld, BYTE *ptst);
// VOID  ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len);
// 

/* deskosif.asm */
VOID  takedos(VOID);
VOID  takekey(VOID);
VOID  takevid(VOID);

VOID  givedos(VOID);
VOID  givekey(VOID);
VOID  givevid(VOID);

/* deskcds.c */
BOOLEAN  cd_isdrvcd( BYTE drvno );

/* deskwcc.c */
WORD  desk_sound(WORD isfreq, WORD freq, WORD duration);
char   *scasb(char *s, char b);
VOID  dos_label(BYTE drive, BYTE *plabel);
WORD  dos_dtype(WORD drive);
VOID  r_set(GRECT *r, WORD x, WORD y, WORD w, WORD h);
VOID  r_get(GRECT *r, WORD *x, WORD *y, WORD *w, WORD *h);
VOID  lstlcpy(LPBYTE dest, LPBYTE src, int maxlen);
UWORD  inside(WORD x, WORD y, GRECT *pt);
VOID  rc_union    (LPGRECT p1, LPGRECT p2);

#if DEBUG
WORD  form_valert(WORD button, BYTE *str, ...);
WORD  dbg(BYTE *str, ...);
VOID  crashdump(BYTE ch);
VOID  debugbreak(VOID);
#endif


#if 1
VOID  iac_init(VOID);
VOID  iac_strcop(LPTREE tree, WORD obj, LPBYTE src);
VOID  iac_schar(LPTREE tree, WORD obj, BYTE ch);
VOID  iac_redrw(LPTREE tree, WORD obj, WORD state, WORD depth);
WORD  iac_isnam(LPBYTE lst);
VOID  ins_acc(VOID);
WORD  iac_dial(LPTREE tree);
VOID  iac_save(LPTREE tree);
WORD  iac_names(LPTREE tree);
VOID  iac_elev(LPTREE tree, WORD currtop, WORD count);
// WORD  iac_comp(VOID);
VOID  iac_mvnames(LPTREE tree, WORD start, WORD num);
#endif


