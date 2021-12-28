/*------------------------------*/
/*	includes		*/
/*------------------------------*/

#include "wccgem.h"				/* portable coding conv	*/
#include <stdio.h>				/* Proper fopen() etc. */
#include <string.h>
#include "rsconv.h"
#include "rcfile.h"
#include "rclib.h"
#include	<stdlib.h>

/*------------------------------*/
/*	defines			*/
/*------------------------------*/

#define	NIL		-1
#define DESK		0
#define	ARROW		0
#define	HOUR_GLASS	2			
#define END_UPDATE	0
#define	BEG_UPDATE	1

typedef RSHDR far * LPRSHDR;
	


/*------------------------------*/
/*	Global			*/
/*------------------------------*/
GLOBAL	WORD	gl_apid;

GLOBAL	WORD	gl_wchar;		/* character width		*/
GLOBAL	WORD	gl_hchar;		/* character height		*/
GLOBAL	WORD	gl_wbox;		/* box (cell) width		*/
GLOBAL	WORD	gl_hbox;		/* box (cell) height		*/
GLOBAL	WORD	gem_handle;		/* GEM handle			*/
GLOBAL	WORD	vdi_handle;		/* VDI handle			*/
GLOBAL	WORD	work_out[57];		/* open virt workstation values	*/
GLOBAL	GRECT	scrn_area;		/* scrn area			*/
GLOBAL	MFDB	scrn_mfdb;		/* scrn memory def'n for blt	*/
GLOBAL	GRECT	full;			/* desktop size			*/

/*------------------------------*/
/*	External		*/
/*------------------------------*/

EXTERN	WORD	desel_obj();

/*------------------------------*/
/*	Local			*/
/*------------------------------*/

MLOCAL	WORD	native_in = TRUE;	/* TRUE: input .RSC is native	*/
					/* FALSE: input .RSC is foreign */
MLOCAL	WORD	conv_def = TRUE;	/* TRUE: convert .DEF also	*/
MLOCAL	WORD	new_dfn = FALSE;	/* TRUE: use new symbol fmt	*/
MLOCAL	BYTE	old_rsc[4] = "RSC";	/* new resource file extent	*/
MLOCAL	BYTE	new_rsc[4] = "RS2";	/* new resource file extent	*/
MLOCAL	BYTE	old_def[4] = "DEF";	/* new definition file extend   */
MLOCAL	BYTE	new_def[4] = "DF2";	/* new definition file extend   */
MLOCAL	BYTE	r_file[81];		/* resource file name 		*/
MLOCAL	BYTE	d_file[81];		/* definition file name 	*/
MLOCAL	BYTE	r2_file[81];		/* output resource name		*/
MLOCAL	BYTE	d2_file[81];		/* output definition name	*/

MLOCAL	FILE	*r_fp  = NULL;		/* resource file handle 	*/
MLOCAL	FILE	*d_fp  = NULL;		/* definition file handle 	*/
MLOCAL	FILE	*r2_fp = NULL;		/* output resource handle	*/
MLOCAL	FILE	*d2_fp = NULL;		/* output definition handle	*/
//MLOCAL	WORD	r_hndl = -1;		/* resource file handle 	*/
//MLOCAL	WORD	d_hndl = -1;		/* definition file handle 	*/
//MLOCAL	WORD	r2_hndl = -1;		/* output resource handle	*/
//MLOCAL	WORD	d2_hndl = -1;		/* output definition handle	*/

MLOCAL  RSHDR	header;			/* Header */
MLOCAL	BYTE	*head;			/* location of buffer		*/
MLOCAL	BYTE	buff[20];		/* def file work area		*/
MLOCAL	UWORD	img_offset, addr;	/* for image fixup		*/
MLOCAL	WORD	img_odd;		/* image fixup needed?		*/
MLOCAL	GRECT	prog_rect;		/* rectangle for prog indicator */

/*------------------------------*/
/*	swap_bytes 		*/
/*------------------------------*/
VOID swap_bytes(LPBYTE where, WORD len)
	{
	BYTE	swap;

	for (; len > 0; len -= 2)
		{
		swap = where[0];
		where[0] = where[1];
		where[1] = swap;
		where += 2;
		}
	}

/*------------------------------*/
/*	swap_words 		*/
/*------------------------------*/
VOID swap_words(LPWORD where, WORD len)
	{
	WORD	swap;

	for (; len > 0; len -= 4)
		{
		swap = where[0];
		where[0] = where[1];
		where[1] = swap;
		where += 2;
		}
	}

/*------------------------------*/
/*	swap_images 		*/
/*------------------------------*/
	VOID
swap_images()
	{
	LPBIT	where;
	LPICON  where2;
	LPBYTE	taddr;
	WORD	num;
	WORD	wb, hl;

	where = (LPBIT) (head + header.rsh_bitblk);
	num = header.rsh_nbb;
	for (; num--; where++)
		{
		taddr = where->bi_pdata;
		wb = where->bi_wb;
		hl = where->bi_hl;
		if ((LONG) taddr != -1L)
			{
			if (img_odd)
				where->bi_pdata = ++taddr;
			swap_bytes((LPBYTE)head + (LONG)taddr, wb * hl);
			}
		}

	where2 = (LPICON) (head + header.rsh_iconblk);
	num = header.rsh_nib;
	for (; num--; where2++)
		{
		wb = (where2->ib_wicon + 7) >> 3;
		hl = where2->ib_hicon;
		taddr = where2->ib_pdata;
		if ((LONG) taddr != -1L)
			{
			if (img_odd)
				where2->ib_pdata = ++taddr;
			swap_bytes((LPBYTE)head + (LONG)taddr, wb * hl);
			}
		taddr = where2->ib_pmask;
		if ((LONG) taddr != -1L)
			{
			if (img_odd)
				where2->ib_pmask = ++taddr;
			swap_bytes((LPBYTE)head + (LONG)taddr, wb * hl);
			}
		}
	}

/*------------------------------*/
/*	swap_trees 		*/
/*------------------------------*/
	VOID
swap_trees()
	{
	LPBYTE	where;
	WORD	size;

	where = head + header.rsh_trindex;
	size = header.rsh_ntree * sizeof(LONG);
	swap_bytes(where, size);
	swap_words((LPWORD) where, size);
	}

/*------------------------------*/
/*	swap_objs 		*/
/*------------------------------*/
	VOID
swap_objs()
	{
	LPTREE	where;
	WORD	num;

	where = (LPTREE) (head + (header.rsh_object));
	num = header.rsh_nobs;
	swap_bytes((LPBYTE) where, num * sizeof(OBJECT));
	for (; num--; where++)
		swap_words((LPWORD) &where->ob_spec, sizeof(LONG));
	}

/*------------------------------*/
/*	swap_teds 		*/
/*------------------------------*/
	VOID
swap_teds()
	{
	LPTEDI	where;
	WORD	num;

	where = (LPTEDI) (head + header.rsh_tedinfo);
	num = header.rsh_nted;
	swap_bytes((LPBYTE) where, num * sizeof(TEDINFO));
	for (; num--; where++)
		{
		swap_words((LPWORD) &where->te_ptext, sizeof(LONG));
		swap_words((LPWORD) &where->te_ptmplt, sizeof(LONG));
		swap_words((LPWORD) &where->te_pvalid, sizeof(LONG));
		}
	}

/*------------------------------*/
/*	swap_ibs 		*/
/*------------------------------*/
	VOID
swap_ibs()
	{
	LPICON	where;
	WORD	num;

	where = (LPICON) (head + header.rsh_iconblk);
	num = header.rsh_nib;
	swap_bytes((LPBYTE) where, num * sizeof(ICONBLK));
	for (; num--; where++)
		{
		swap_words((LPWORD) &where->ib_pdata, sizeof(LONG));
		swap_words((LPWORD) &where->ib_pmask, sizeof(LONG));
		swap_words((LPWORD) &where->ib_ptext, sizeof(LONG));
		}
	}

/*------------------------------*/
/*	swap_bbs 		*/
/*------------------------------*/
	VOID
swap_bbs()
	{
	LPBIT	where;
	WORD	num;

	where = (LPBIT) (head + header.rsh_bitblk);
	num = header.rsh_nbb;
	swap_bytes((LPBYTE) where, num * sizeof(BITBLK));
	for (; num--; where++)
		swap_words((LPWORD) &where->bi_pdata, sizeof(LONG));
	}

/*------------------------------*/
/*	swap_fstr 		*/
/*------------------------------*/
	VOID
swap_fstr()
	{
	LPBYTE	where;
	WORD	size;

	where = head + header.rsh_frstr;
	size = header.rsh_nstring * sizeof(LONG);
	swap_bytes(where, size);
	swap_words((LPWORD) where, size);
	}

/*------------------------------*/
/*	swap_fimg 		*/
/*------------------------------*/
	VOID
swap_fimg()
	{
	LPBYTE	where;
	WORD	size;

	where = head + header.rsh_frimg;
	size = header.rsh_nimages * sizeof(LONG);
	swap_bytes(where, size);
	swap_words((LPWORD) where, size);
	}

/*------------------------------*/
/*	close_files 		*/
/*------------------------------*/
	VOID
close_files()
	{
	if (r_fp)  fclose(r_fp);
	if (d_fp)  fclose(d_fp);
	if (r2_fp) fclose(r2_fp);
	if (d2_fp) fclose(d2_fp);
	r_fp = d_fp = r2_fp = d2_fp = NULL;
	}
	
/*------------------------------*/
/*	do_conv 		*/
/*------------------------------*/
	WORD
do_conv()
	{
	LPBYTE	str; 
	LONG	size;
	WORD	reply, nsym;
	int	fwret;

	r_file[0] = '\0';

	if (!get_file(old_rsc, r_file))
		return;
	if ((r_fp = fopen(r_file, "rb")) == NULL)
		return;
	if (conv_def)
		{
		new_ext(r_file, d_file, old_def);
		FOREVER {
			d_fp = fopen(d_file, "rb");
			if (d_fp) break;
			rsrc_gaddr(R_STRING, NODEF, (LPVOID *)&str);
			reply = form_alert(1, str);
			if (reply == 3)
				{
				close_files();
				return;
				}
			if (reply == 1)
				{
				conv_def = FALSE;
				break;
				}
						/* if (reply == 2) */
			if (!get_file(old_def, d_file))
				break;
			}
		}

	graf_mouse(HOUR_GLASS, 0x0L);
	beg_prog(&prog_rect);
	set_prog(RSCREAD);

	if (fread(&header, 1, sizeof(RSHDR), r_fp) < (int)sizeof(RSHDR))
		{
		dos_error(errno);
		return;
		}

	if (!native_in)
		swap_bytes((LPBYTE)&header, sizeof(RSHDR));

	head = malloc(header.rsh_rssize);
		
	size = header.rsh_rssize;

	if (!head)
		{
		graf_mouse(ARROW, 0x0L);		/* Before alert */
		rsrc_gaddr(R_STRING, NOMEM, (LPVOID *)&str);
		form_alert(1, str);
		close_files();
		free(head);
		return;
		}
	memcpy(head, &header, sizeof(RSHDR));

	if (fread(head + sizeof(RSHDR), 1, size - sizeof(RSHDR), r_fp) < size - sizeof(RSHDR))
		{
		dos_error(errno);
		free(head);
		return;
		}

	img_offset = ((LPRSHDR) head)->rsh_imdata;
	if ( (img_odd = img_offset & 0x0001) )
		{
		set_prog(IMGALIGN);
		((LPRSHDR) head)->rsh_rssize = img_offset + 1;
		for (addr = ((LPRSHDR) head)->rsh_frstr; --addr > img_offset; )
			*(head + addr) = *(head + addr - 1);
		}

	set_prog(BYTESWAP);
	if (native_in) swap_images();

	swap_trees();
	swap_objs();
	swap_teds();
	swap_ibs();
	swap_bbs();
	swap_fstr();
	swap_fimg();

	if (native_in)  swap_bytes(head, sizeof(RSHDR));
	else		    swap_images();

	set_prog(RSCWRITE);
	new_ext(r_file, r2_file, new_rsc);
	if ( !(r2_fp = create_file(r2_file)))
		{
		close_files();
		free(head);
		return;
		}
	graf_mouse(HOUR_GLASS, 0x0L);		/* Create_file could reset */

	if (fwrite(head, 1, size, r2_fp) < size)
		{
		dos_error(errno);
		free(head);
		return;
		}

	if (!conv_def)
		{
		close_files();
		end_prog(&prog_rect);
		free(head);
		return;
		}

	set_prog(DEFREAD);
	new_ext(r_file, d2_file, new_def);

	if ( !(d2_fp = create_file(d2_file)))
		{
		close_files();
		free(head);
		return;
		}

	if (fread(&nsym, 1, sizeof(WORD), d_fp) < (int)sizeof(WORD))
		{
		dos_error(errno);
		free(head);
		return;
		}

	set_prog(BYTESWAP);
	reply = nsym;
	swap_bytes((LPBYTE)&reply, 2);

	fwret = fwrite( (new_dfn ? &nsym : &reply), 1, sizeof(WORD), d2_fp);
	if (fwret < (int)sizeof(WORD))
		{
		dos_error(errno);
		free(head);
		return;
		}

	if (!(native_in || new_dfn))
		nsym = reply;

	for (; nsym--; )
		{
		if (!new_dfn)			/* Spare words only if */
						/* using old format    */
		if (native_in)
			{			/* insert spare word */
			reply = 0;
			fwret = fwrite(&reply, 1, sizeof(WORD), d2_fp);
			if (fwret < (int)sizeof(WORD))
				{
				dos_error(errno);
				free(head);
				return;
				}
			}
		else
			{			/* delete extra word */ 
			fwret = fread(buff, 1, sizeof(WORD), d_fp);
			if (fwret < (int)sizeof(WORD))
				{
				dos_error(errno);
				free(head);
				return;
				}
			}
		fwret = fread(buff, 1, sizeof(WORD), d_fp);
		if (fwret < (int)sizeof(WORD))
			{
			dos_error(errno);
			free(head);
			return;
			}

		if (!new_dfn)			/* Just copy for new fmt */
			swap_bytes(buff, 2);

		fwret = fwrite(buff, 1, sizeof(WORD), d2_fp);
		if (fwret < (int)sizeof(WORD))
			{
			dos_error(errno);
			free(head);
			return;
			}
		fwret = fread(buff, 1, 10 + sizeof(WORD), d_fp);
		if (fwret < 10)	/* BUGFIX: Last read is 10 bytes instead of 12 */
			{
			dos_error(errno);
			free(head);
			return;
			}

		if (!new_dfn)
			swap_bytes(buff, 2);	/* only swap value */

		fwret = fwrite(buff, 1, 10 + sizeof(WORD), d2_fp);
		if (fwret < (10 + (int)sizeof(WORD)))
			{
			dos_error(errno);
			free(head);
			return;
			}
		}

	set_prog(DEFWRITE);
	close_files();
	end_prog(&prog_rect);
	free(head);
	}

/*------------------------------*/
/*	do_help 		*/
/*------------------------------*/
	VOID
do_help()
	{
	LPTREE	tree;
	WORD	obj;

	rsrc_gaddr(R_TREE, HELP, (LPVOID *)&tree);
	obj = hndl_dial(tree, 0, 0, 0, 0, 0);
	desel_obj(tree, obj);
	}



	
/*------------------------------*/
/*	do_mode 		*/
/*------------------------------*/
	WORD
do_mode()
	{
	LPTREE	tree;
	WORD	obj;
	WORD	xdial, ydial, wdial, hdial;
	GRECT   R;

	rsrc_gaddr(R_TREE, MODE, (LPVOID *)&tree);

	sel_obj(tree, native_in? N2F: F2N);
	if (!conv_def)
		sel_obj(tree, DEFNO);
	else if (!new_dfn)
		sel_obj(tree, DEFYES);
	else
		sel_obj(tree, DFNYES);
	set_text(tree, RSC1, old_rsc, 4);
	set_text(tree, RSC2, new_rsc, 4);
	set_text(tree, DEF1, old_def, 4);
	set_text(tree, DEF2, new_def, 4);

	form_center(tree, &xdial, &ydial, &wdial, &hdial);
	form_dial(0, 0, 0, 0, 0, xdial, ydial, wdial, hdial);
	form_dial(1, 0, 0, 0, 0, xdial, ydial, wdial, hdial);
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	R.g_x = xdial;
	R.g_y = ydial;
	R.g_w = wdial;
	R.g_h = hdial;

	while (TRUE)
		{
		obj = form_do(tree, 0) & 0x7FFF;
		if (obj == DEFYES)
			{
			if (strcmp(old_def, "DFN") == 0)
				{
				strcpy(old_def, "DEF");
				disp_obj(tree, DEF1);
				}
			}
		else if (obj == DFNYES)
			{
			if (strcmp(old_def, "DEF") == 0)
				{
				strcpy(old_def, "DFN");
				disp_obj(tree, DEF1);
				}
			}
		else
			break;
		}

	form_dial(2, 0, 0, 0, 0, xdial, ydial, wdial, hdial);
	form_dial(3, 0, 0, 0, 0, xdial, ydial, wdial, hdial);

	native_in = selected(tree, N2F);
	conv_def = !selected(tree, DEFNO);
	new_dfn = selected(tree, DFNYES);

	map_tree(tree, ROOT, NIL, desel_obj);
	return (obj);
	}

/*------------------------------*/
/*	rscv_run		*/
/*------------------------------*/
	WORD
rscv_run()
	{
	WORD	obj;

	FOREVER {
		obj = do_mode();
		if (obj == HELPMODE)
			do_help();
		else if (obj == CONVMODE)
			{
			do_conv();
			graf_mouse(ARROW, 0x0L);
			}
		else 
			break;
		}
	}

/*------------------------------*/
/*	rscv_term		*/
/*------------------------------*/
	VOID
rscv_term(term_type)
	WORD	term_type;
	{
	switch (term_type)	/* NOTE: all cases fall through		*/
		{
		case (0 /* normal termination */):
		case (2):
			v_clsvwk( vdi_handle );
		case (3):
			rsrc_free();
		case (4):
			if (term_type)
				wind_update(END_UPDATE);
			appl_exit();
		case (5):
			break;
		}
	}

/*------------------------------*/
/*	rscv_init		*/
/*------------------------------*/
	WORD
rscv_init()
	{
	WORD	i;
	WORD	work_in[11];

	appl_init(NULL);				/* initialize libraries	*/
	if (gl_apid == -1)
		return (5);			/* No handles ! */
	graf_mouse(HOUR_GLASS, 0x0L);
	wind_update(BEG_UPDATE);
	if (!rsrc_load( "RSCONV.RSC" ))
		{
		graf_mouse(ARROW, 0x0L);
		form_alert(1,
		"[3][Fatal Error !|RSCONV.RSC|File Not Found][ Abort ]" );
		return(4);	/* Can't find resource */
		}

	for (i=0; i<10; i++)
		{
		work_in[i]=1;
		}
	work_in[10]=2;
	gem_handle = graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	vdi_handle = gem_handle;
	v_opnvwk(work_in,&vdi_handle,work_out);	/* open virtual work stn*/
	if (vdi_handle == 0)
		return (3);

	vqt_attributes (vdi_handle, work_in);

	scrn_area.g_x = 0;
	scrn_area.g_y = 0;
	scrn_area.g_w = work_out[0] + 1;
	scrn_area.g_h = work_out[1] + 1;

	scrn_mfdb.np = work_out[4];
	scrn_mfdb.mp = 0x0L;

	wind_get(DESK, WF_WXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);

	vst_height(vdi_handle, work_in[7],
		&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);

	graf_mouse(ARROW,0x0L);
	wind_update(END_UPDATE);
	return(0);
	}

/*------------------------------*/
/*	main			*/
/*------------------------------*/
WORD GEMAIN(WORD argc, BYTE *argv[])
	{
	WORD	rscv_code;
	
	if ( !(rscv_code = rscv_init()) )	/* initialization	*/
		rscv_run();
	rscv_term(rscv_code);			/* termination		*/
	}

