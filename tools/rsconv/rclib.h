
typedef WORD (*SELRTN)(LPTREE tree, WORD w);


VOID do_obj  (LPTREE tree, WORD which, WORD bit);	/* set specified bit in object state	*/
VOID undo_obj(LPTREE tree, WORD which, WORD bit);
WORD sel_obj (LPTREE tree, WORD which);
WORD desel_obj(LPTREE tree, WORD which);
WORD disab_obj(LPTREE tree, WORD which);
VOID objc_xywh(LPTREE tree, WORD obj, GRECT *p);
VOID disp_obj(LPTREE tree, WORD obj);
WORD selected(LPTREE tree, WORD obj);
VOID map_tree(LPTREE tree, WORD this, WORD last, SELRTN routine);
WORD hndl_dial(LPTREE tree, WORD def, WORD x, WORD y, WORD w, WORD h);
VOID set_text(LPTREE tree, WORD obj, LPBYTE str, WORD len);
VOID beg_prog(LPGRECT rect);
VOID end_prog(LPGRECT rect);
VOID set_prog(UWORD strno);

