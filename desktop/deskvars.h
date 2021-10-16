
#ifdef INSTANTIATE
#define EXT
#define EQ(x) = x
#else
#define EXT extern
#define EQ(x)
#endif


EXT USERFUNC drawaddr;
EXT ICONBLK gl_icons[NUM_WOBS];
extern union	REGS DR;
#define DOS_AX DR.x.ax
EXT WORD	gl_hchar;
EXT WORD	gl_wchar;
EXT WORD	gl_hschar;
EXT WORD	gl_wschar;
EXT WORD    gl_width;
EXT WORD	gl_height;

EXT WORD	gl_wbox;
EXT WORD	gl_hbox;
EXT WORD	gl_xclip;
EXT WORD	gl_yclip;
EXT WORD	gl_wclip;
EXT WORD	gl_hclip;

EXT WORD	gl_handle;
EXT WORD	gl_wptschar, gl_hptschar;
EXT WORD	gl_nrows;
EXT WORD	gl_ncols;
EXT WORD	gl_patt, gl_font, gl_fis;

EXT WORD    gl_mode, gl_tcolor, gl_lcolor;
EXT WORD	gl_nplanes;
EXT MFDB	gl_src;
EXT MFDB	gl_dst;
EXT WORD	gl_workout[57];


EXT BYTE	gl_amstr[4];
EXT BYTE	gl_pmstr[4];
EXT WORD	gl_numics;
EXT WORD    gl_stdrv;
EXT FNODE	*ml_pfndx[NUM_FNODES];

EXT ICONBLK gl_aib;
EXT ICONBLK gl_dib;
EXT WORD gl_iconstart;	/* Was gl_pstart. Renamed so I can check all
			 * places where it's used. Anyway, it isn't a pointer,
			 * it's an offset. */
EXT BYTE gl_lngstr[256];

#if MULTIAPP
EXT BYTE	gl_bootdr;
EXT WORD	gl_untop;
EXT LONG	pr_beggem;
EXT LONG	pr_begacc;
EXT LONG	pr_begdsk;
EXT LONG	pr_topdsk;
EXT LONG	pr_topmem;
EXT LONG	pr_ssize;
EXT LONG	pr_itbl;
EXT WORD	pr_kbytes;
EXT WORD	gl_fmemflg;
EXT ACCNODE     gl_caccs[3];
EXT WORD	gl_keepac;
#endif

EXT GLOBES	G;

/* BugFix	*/
EXT WORD	ig_close;
EXT WORD	gl_apid;


// not in DESKTOP v1.2 EXT LPBYTE	ad_ptext;
// not in DESKTOP v1.2 EXT LPBYTE	ad_picon;
// not in DESKTOP v1.2 EXT GRECT	gl_savewin[NUM_WNODES];	/* preserve window x,y,w,h	*/
// not in DESKTOP v1.2 EXT GRECT	gl_normwin;		/* normal (small) window size	*/
// not in DESKTOP v1.2 EXT WORD	gl_open1st;		/* index of window to open 1st	*/
EXT BYTE	gl_defdrv;		/* letter of lowest drive	*/
/* Bring back some bits not in DESKTOP v1.2 */
EXT WORD	can_iapp;		/* TRUE if INSAPP enabled	*/
EXT WORD	can_show;		/* TRUE if SHOWITEM enabled	*/
EXT WORD	can_del;		/* TRUE if DELITEM enabled	*/
EXT WORD	can_output;		/* TRUE if OUTPITEM endabled	*/
EXT WORD	gl_whsiztop;		/* wh of window fulled		*/
EXT WORD	gl_idsiztop;		/* id of window fulled		*/

EXT GRECT	gl_rscreen;
EXT GRECT	gl_rfull;
EXT GRECT	gl_rzero;
EXT GRECT	gl_rcenter;
EXT GRECT	gl_rmenu;
EXT BYTE	gl_afile[SIZE_AFILE];
EXT BYTE	gl_buffer[SIZE_BUFF];
EXT WORD	in_type;		/* True if displaying file contents */

extern WORD global[G_SIZE];
extern WORD DOS_ERR;

EXT X_BUF_V2 gl_xbuf;
