
/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*              Historical Copyright                             
*	-------------------------------------------------------------
*	GEM Desktop					  Version 3.0
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1986			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "wccdesk.h"

MLOCAL WORD	iac_chkd;

/*

EXTERN ACCNODE gl_caccs[];


EXTERN LONG	pr_begacc;
EXTERN LONG	pr_begdsk;
EXTERN WORD	gl_hbox;
EXTERN BYTE	gl_bootdr;
#define BEG_UPDATE 1
#define END_UPDATE 0
*/
#define NUM_AFILES 20
#define NUM_FSNAME  8
#define LEN_FSNAME 16

GLOBAL BYTE	*g_fslist[NUM_AFILES];
GLOBAL BYTE	g_fsnames[LEN_FSNAME * NUM_AFILES];
MLOCAL WORD	fcount;
MLOCAL WORD	fcurrtop;

#if !MULTIAPP
	GLOBAL BYTE *g_inslist[3];
#endif

/*
 * Forward declarations
 */
VOID  iac_elev(LPTREE tree, WORD currtop, WORD count);
VOID  iac_mvnames(LPTREE tree, WORD start, WORD num);
VOID  iac_redrw(LPTREE tree, WORD obj, WORD state, WORD depth);

#if 0
	VOID
printstr(lst)
	LONG		lst;
{
	BYTE		ch;
	WORD		i;
	
	i = 0;
	while ((ch = LBGET(lst + i++)) != 0)
	  dbg("%c", ch);
}
#endif

WORD  iac_isnam(LPBYTE lst)
{
	BYTE		ch;
	
	ch = LBGET(lst);
	return((ch>='A') && (ch<='Z'));
}

/*
 * Routine that merge concatenate a custom string with the GEM error message 
 * for a given error number. The resultant alert is then displayed.
 */
WORD iac_error(BYTE *dir, BYTE *acc, WORD errnum)
{
	WORD string;

	switch (errnum)
	{
	  case 2:
	  case 18:	
	  case 3:
		string = ACC18ERR;
		break;
	  case 4:
		string = ACC04ERR;
		break;
	  case 5:
		string = ACC05ERR;
		break;
	  case 8:
	  case 10:
	  case 11:
		string = ACC08ERR;
		break;
	  default:
		string = ACCXXERR;
		break;
	}

    return fun_alert(1, string, dir, acc, errnum);
}



#if MULTIAPP
VOID  iac_init()
{
	WORD		i, j;
	BYTE		*npt;
	BYTE		accstr[9];

	gl_keepac = FALSE;
	for (i=0; i<3; i++)
	{
	  npt = &gl_caccs[i].acc_name[0];
	  if (iac_isnam(ADDR(npt)))
	  {
	    strcpy(&accstr[0], "        ");	/* eight blanks		*/
	    for (j=0; (j<8) && npt[j] && (npt[j] != '.'); j++)
	      accstr[j] = npt[j];
	    if (appl_find(ADDR(&accstr[0])) < 0)
	      npt[0] = '\0';
	    else				/* check for no full step */
	      gl_keepac |= (gl_caccs[i].acc_swap == 'N');
	  }
	}
}

#define ACCMIN	0x2000

WORD  iac_save(LPTREE tree)
{
	WORD		i;
	WORD		chnum;
	WORD		isswap;
	LPBYTE		lst;
	LONG		begaddr;
	LONG		csize;
	LONG		junk;
	BYTE		didalert;
	WORD 		didrun;

	wind_update(END_UPDATE);
	strcpy(":\\GEMBOOT", &G.g_cmd[1]);
	G.g_cmd[0] = gl_bootdr;			/* get correct drive	*/
	dos_sdrv(gl_bootdr - 'A');		/* set drive & directory */
	dos_chdir(G.a_cmd);

	begaddr = pr_begacc;
	didalert = FALSE;
	gl_keepac = FALSE;
	proc_delete(-1);		/* delete all accessories	*/
	for (i=0; i<3; i++)
	{
	  LPTEDI tedi = (LPTEDI)(tree[ACC1NAME + i].ob_spec);
	  lst = tedi->te_ptext;
	  if (iac_isnam(lst))
	  {
            _fstrcpy(ADDR(&(gl_caccs[i].acc_name[0])), lst);

	    junk = (LONG)(tree[ACC1FMEM+i].ob_spec);
	    gl_caccs[i].acc_swap = LHIBT(LHIWD(junk));
	    isswap = (gl_caccs[i].acc_swap == 'N');
	    gl_keepac |= isswap;	/* if TRUE, no full step	*/
	    if (gl_keepac && gl_fmemflg)
	    {
	      if (!didalert)
	      {
		rsrc_gaddr(R_STRING, STACFMEM, (LPVOID *)&G.a_alert);
		form_alert(1, G.a_alert);
		didalert = TRUE;
	      }
	      gl_keepac = FALSE;	/* this guy won't run		*/
	      begaddr = pr_begdsk;	/* quit trying, but get names	*/
	    }
	    if  ((begaddr + ACCMIN <= pr_begdsk) &&
		(proc_create((LPBYTE)LSEGOFF(begaddr), pr_begdsk-begaddr,
/*				 isswap, 1, &chnum)))			*/
/* ISSWAP = 0 FOR GDOS */	 0     , 1, &chnum)))
	    {
	      didrun = proc_run(chnum, 1, 3, lst, ADDR("\0"));
	      if (didrun)
	      { 
	        proc_info(chnum, (WORD *)&junk, (WORD *)&junk,
				(LPVOID *)&junk, &csize,
				(LPVOID *)&junk,&junk,
				(LPVOID *)&junk);
	        begaddr += csize;
	      }
	    }
	    else
	      didrun = FALSE;
	    if ((!didalert) && (!didrun))
	    {
	      rsrc_gaddr(R_STRING, STACCMEM, (LPVOID *)&G.a_alert);
	      form_alert(1, G.a_alert);
	      didalert = TRUE;
	    }
	  }
	  else
	    gl_caccs[i].acc_name[0] = '\0';
        }
	wind_update(BEG_UPDATE);
	
	return 0;
}

VOID  iac_schar(LPTREE tree, WORD obj, BYTE ch)
{
	LONG		longch;
	LONG		spec;

	spec = (LONG)(tree[obj].ob_spec);
 	longch = ch;
	longch = longch << 24;
	spec = (spec & 0xFFFFFFl) | longch;
	tree[obj].ob_spec = (LPBYTE)spec;
	// [JCE] To make these controls behave like checkboxes
	if (ch == 'Y')
		tree[obj].ob_state |= SELECTED;
	else	tree[obj].ob_state &= ~SELECTED;
}
	

#else

VOID iac_swap(LPTREE tree, WORD left, WORD right)
{
	WORD i;
	WORD thefile, theacc;
	BYTE *temp;

	// fprintf(logfile, "iac_swap(tree, %d, %d)\n", left, right);
	if(left < 0)
	{
		/* If left is negative, remove the ACC without swapping */
		/* Add an empty slot at the end of g_fslist */
		thefile = fcount;
		g_fslist[thefile] = NULL;
		fcount++;
	} else {
		thefile = fcurrtop + left - ACA1NAME;
	}
	theacc = right - ACC1NAME;
	// fprintf(logfile, "thefile=%d, g_fslist=%s\n", thefile, g_fslist[thefile]);
	// fprintf(logfile, "theacc=%d, chspec=%s\n", theacc, g_inslist[theacc]);
	/* swap installed and available ACC */
	temp = g_fslist[thefile];
	g_fslist[thefile] = g_inslist[theacc];
	g_inslist[theacc] = temp;

	if (g_fslist[thefile] == NULL)
	{
		/* remove empty entry from g_fslist */
		fcount--;
		for(i = thefile; i < fcount; i++)
		{
			g_fslist[i] = g_fslist[i+1];
		}
		g_fslist[fcount] = NULL;
		// fprintf(logfile, "fcurrtop = %d, fcount = %d\n", fcurrtop, fcount);
		if(fcurrtop > max(0, fcount - NUM_FSNAME))
		{
			/* scroll down one line */
			fcurrtop--;
		}
	}
	/* Update and redraw accessory lists and scroll bar */
	iac_elev(tree, fcurrtop, fcount);
	iac_mvnames(tree, fcurrtop, min(fcount - fcurrtop, NUM_FSNAME));
	iac_redrw(tree, ACSCROLL, NORMAL, 2);
	iac_redrw(tree, ACNAMBOX, NORMAL, 1);
	iac_redrw(tree, right, CHECKED, 0);
}

WORD iac_save(LPTREE tree)
{
	WORD i, ret;
	BOOLEAN keep;

    graf_mouse(HOURGLASS,NULL);    /* say we're busy */

	strcpy(&G.g_srcpth[1], ":\\GEMAPPS\\GEMBOOT\\");
	G.g_srcpth[0] = gl_bootdr;
	strcpy(&G.g_dstpth[1], ":\\GEMAPPS\\GEMSYS\\");
	G.g_dstpth[0] = gl_bootdr;
	dos_sdta(G.a_wdta);
	strcpy(&G.g_cmd[1], ":\\GEMAPPS\\GEMSYS\\*.ACC");
	G.g_cmd[0] = gl_bootdr;
	// fprintf(logfile, "G.g_cmd=%s\n", G.g_cmd);
	ret = dos_sfirst(G.a_cmd, 0x16);
	// fprintf(logfile, "dos_sfirst: %d\n", ret);
	while(ret)
	{
		strcpy(&G.g_dstpth[18], &G.g_wdta[30]);
		// fprintf(logfile, "Checking installed ACC %s\n", &G.g_dstpth[18]);
		keep = FALSE;
		for(i = 0; i < 3; i++)
		{
			if(strcmp(&G.g_dstpth[18], g_inslist[i]) == 0)
			{
				// fprintf(logfile, "ACC should be kept installed\n");
				keep = TRUE;
				/* Set to NULL, so that next phase will skip it */
				g_inslist[i] = NULL;
				break;
			}
		}
		if(!keep)
		{
			strcpy(&G.g_srcpth[19], &G.g_wdta[30]);
			// fprintf(logfile, "Move %s to %s\n", G.g_dstpth, G.g_srcpth);
			ret = dos_rename(G.g_dstpth, G.g_srcpth);
			// fprintf(logfile, "dos_rename returned %d\n", ret);
			if(ret) {
				graf_mouse(ARROW,NULL);
				iac_error("remove", &G.g_srcpth[19], ret);
				return 0;
			}
		} 
		ret = dos_snext();
	}

	/* Move newly installed ACCs to GEMSYS */
	for(i = 0; i < 3; i++)
	{
		if(g_inslist[i])
		{
			strcpy(&G.g_srcpth[19], g_inslist[i]);
			strcpy(&G.g_dstpth[18], g_inslist[i]);
			// fprintf(logfile, "Move %s to %s\n", G.g_srcpth, G.g_dstpth);
			ret = dos_rename(G.g_srcpth, G.g_dstpth);
			// fprintf(logfile, "dos_rename returned %d\n", ret);
			if(ret) {
				graf_mouse(ARROW,NULL);
				iac_error("install", g_inslist[i], ret);
				return 0;
			}
		}
	}

	graf_mouse(ARROW,NULL);
	
	return fun_alert(1, STACINST, NULL);
}

#endif


VOID  iac_strcop(LPTREE tree, WORD obj, LPBYTE src)
{
	LPBYTE	dst;
	LPTEDI tedi;

	switch (tree[obj].ob_type)
	{
	  case G_BUTTON:
	  case G_STRING:
	    dst = tree[obj].ob_spec;
	    break;
	  default:
	    tedi = (LPTEDI)(tree[obj].ob_spec);
	    dst = tedi->te_ptext;
	    break;
	}
	_fstrcpy(dst, src);
}

	    
/*
 * Change `state` and redraw tree[obj] with given `depth`
 * Bugfix: add `trim` parameter to trim the right edge and redraw
 * overlapped objects correctly
 */
VOID  iac_redrw(LPTREE tree, WORD obj, WORD state, WORD depth)
{
	WORD		x, y, w, h;

	objc_offset(tree, obj, &x, &y);
	w = tree[obj].ob_width;
	h = tree[obj].ob_height;
	tree[obj].ob_state = state;
	objc_draw(tree, obj, depth, x, y, w, h);
}

VOID  iac_elev(LPTREE tree, WORD currtop, WORD count)
{
	WORD		h,y,th;

	y = 0;
	th = h = tree[ACFSVSLI].ob_height;
	if ( count > NUM_FSNAME)
	{
	  h = x_mul_div(NUM_FSNAME, h, count);
	  h = max(gl_hbox/2, h);		/* min size elevator	*/
	  y = x_mul_div(currtop, th-h, count-NUM_FSNAME);
	}
	tree[ACFSELEV].ob_y = y;
	tree[ACFSELEV].ob_height = h;
}


/* 
WORD  iac_comp(VOID)
{
	WORD		chk;

	if ( (gl_tmp1[0] == ' ') &&
	     (gl_tmp2[0] == ' ') )
	{
	  chk = strcmp( scasb(&gl_tmp1[0], '.'), 
			scasb(&gl_tmp2[0], '.') );
	  if ( chk )
	    return( chk );
	}
	return ( strcmp(&gl_tmp1[0], &gl_tmp2[0]) );
}
 */

VOID iac_padname(BYTE *fname)
{
	WORD j,k, len;
	
	len = 0;
	while (fname[len] != '.')
	    len++;
	if (len < 8)				/* blank pad in middle	*/
	{
		for (j=11, k=len+3; j > 7; j--, k--)
	    	fname[j] = fname[k];
	    for (j=len; j < 8; j++)
	      fname[j] = ' ';
		/* BIGFIX: set zero terminator */
		fname[12]='\0';
	}
}

VOID  iac_mvnames(LPTREE tree, WORD start, WORD num)
{
	WORD		i, j, k;
	WORD		len;

	// fprintf(logfile, "iac_mvnames\n");
	for (i=0; i<num; i++)
	{
		strcpy(G.g_cmd, g_fslist[i+start]);
		// fprintf(logfile, "file name: %s\n", G.g_cmd);
		iac_padname(G.g_cmd);
		// fprintf(logfile, "padded name: %s\n", G.g_cmd);
		iac_strcop(tree, ACA1NAME+i, G.a_cmd);
	}
	/* Cleanup empty items at the bottom, if any */
	for( ; i < NUM_FSNAME; i++)
	{
		iac_strcop(tree, ACA1NAME+i, "________.___");
	}
#if !MULTIAPP
	/* Update names of installed ACCs */
	for(i = 0; i < 3; i++)
	{
		if(g_inslist[i])
		{
			strcpy(G.g_cmd, g_inslist[i]);
			iac_padname(G.g_cmd);
			iac_strcop(tree, ACC1NAME+i, G.a_cmd);
		} else {
			iac_strcop(tree, ACC1NAME+i, "unused");
		}
	}
	
#endif
}


WORD  iac_names(LPTREE tree)
{
	WORD		ret;
	WORD		len;
	WORD		i, j, gap;
	WORD		thefile;
	BYTE		*ptr, *temp;
	
	// fprintf(logfile, "iac_names()\n");

					/* find all .acc files in GEMBOOT */
					/* stuff first 8 in object	*/
					/* adjust elevator size to number */
	thefile = 0;
	ptr = &g_fsnames[0];
	dos_sdta(G.a_wdta);
#if MULTIAPP
	strcpy(&G.g_cmd[1], ":\\GEMBOOT\\*.ACC");
#else
	strcpy(&G.g_cmd[1], ":\\GEMAPPS\\GEMBOOT\\*.ACC");
#endif
	G.g_cmd[0] = gl_bootdr;
	// fprintf(logfile, "G.g_cmd=%s\n", G.g_cmd);
	ret = dos_sfirst(G.a_cmd, 0x16);
	// fprintf(logfile, "dos_sfirst: %d\n", ret);
	while ( ret )
	{
	  strcpy((g_fslist[thefile] = ptr), &G.g_wdta[30]);
	  len = strlen(ptr);
	  ptr += len+1;
	  // fprintf(logfile, "filename: %s\n", g_fslist[thefile]);
	  ret = dos_snext();
      // fprintf(logfile, "dos_snext: %d\n", ret);
      /* leave at least 3 slots free for the installed ACCs */
	  if (thefile++ >= NUM_AFILES - 3) 
	  {
	    ret = FALSE;
	    desk_sound(TRUE, 660, 4);
	  }
	}
	fcount = thefile;


#if MULTIAPP
	/* Order the accessory names only in the MULTIAPP */
	for(gap = thefile/2; gap > 0; gap /= 2)
	{
	  for(i = gap; i < thefile; i++)
	  {
	    for (j = i-gap; j >= 0; j -= gap)
	    {
	      if ( strcmp(g_fslist[j], g_fslist[j+gap]) <= 0 )
		break;
	      temp = g_fslist[j];
	      g_fslist[j] = g_fslist[j+gap];
	      g_fslist[j+gap] = temp;
	    }
	  }
	}
#else
	/* Add the installed accessory names after the available ACCs */
	strcpy(&G.g_cmd[1], ":\\GEMAPPS\\GEMSYS\\*.ACC");
	// fprintf(logfile, "G.g_cmd=%s\n", G.g_cmd);
	ret = dos_sfirst(G.a_cmd, 0x16);
	// fprintf(logfile, "dos_sfirst: %d\n", ret);
	thefile = 0;
	while ( ret )
	{
	  _fstrcpy(ADDR(g_inslist[thefile] = ptr), G.a_wdta+30);
	  len = _fstrlen(ptr);
	  ptr += len+1;
	  // fprintf(logfile, "filename: %s\n", g_inslist[thefile]);
	  ret = dos_snext();
      // fprintf(logfile, "dos_snext: %d\n", ret);
      /* No more than 3 ACCs installed */
	  if (thefile++ >= 3) 
	  {
	    ret = FALSE;
	    desk_sound(TRUE, 660, 4);
	  }
	}
	/* Set empty accessory slots to NULL */
	for(;thefile < 3; thefile++)
	{
		g_inslist[thefile] = NULL;	
	}
#endif 

	iac_mvnames(tree, 0, min(fcount, NUM_FSNAME));
	
	// fprintf(logfile, "return %d\n", fcount);
	return(fcount);
}

WORD  iac_scroll(LPTREE tree, WORD currtop, WORD count, WORD move)
{
	WORD		newtop;
	
	// fprintf(logfile, "iac_scroll(tree, %d, %d, %d)\n", currtop, count, move);
	if (count <= NUM_FSNAME)
	  return(0);
	newtop = currtop + move;
	newtop = max(newtop, 0);
	newtop = min(newtop, count - NUM_FSNAME);
	// fprintf(logfile, "newtop = %d\n", newtop);
	if (newtop != currtop) 
	{
		// fprintf(logfile, "Redraw accessory list\n");
		iac_elev(tree, newtop, count);
		iac_mvnames(tree, newtop, NUM_FSNAME);
		iac_redrw(tree, ACSCROLL, NORMAL, 2);
		iac_redrw(tree, ACNAMBOX, NORMAL, 1);
	}
	// fprintf(logfile, "return %d\n", newtop);
	return(newtop);
}


WORD  iac_dial(LPTREE tree)
{
	WORD		touchob;
	WORD		cont, ret;
	WORD		xd, yd, wd, hd;
	LONG		spec;
	LPBYTE		chspec;
	WORD		newstate;
	WORD		i;
	WORD		move;
	GRECT		pt;
	WORD		mx, my, kret, bret;
	
	ret = 0;
	iac_chkd = ACC1NAME;
	fcount = iac_names(tree);
	fcurrtop = 0;
	iac_elev(tree, fcurrtop, fcount);
#if MULTIAPP
	for (i=0; i<3; i++)
	{
	  tree[ACC1NAME + i].ob_state = NORMAL;
	  chspec = &gl_caccs[i].acc_name[0];
	  if (iac_isnam(chspec))
	  {
	    iac_strcop(tree, ACC1NAME+i, chspec);
	    iac_schar(tree, ACC1FMEM+i, gl_caccs[i].acc_swap);
	  }
	  else
	  {
	    iac_strcop(tree, ACC1NAME+i, "unused");
	    iac_schar(tree, ACC1FMEM+i, 'Y');
	  }
	}
#else
	tree[FMEMTEXT].ob_flags |= HIDETREE;
	tree[ACC1FMEM].ob_flags |= HIDETREE;
	tree[ACC2FMEM].ob_flags |= HIDETREE;
	tree[ACC3FMEM].ob_flags |= HIDETREE;
#endif
	tree[iac_chkd].ob_state = CHECKED;

	form_center(tree, &xd, &yd, &wd, &hd);
	form_dial(FMD_START, 0, 0, 0, 0, xd, yd, wd, hd);
	objc_draw(tree, ROOT, MAX_DEPTH, xd, yd, wd, hd);
	
	cont = TRUE;
	while (cont)
	{
#if !MULTIAPP
	  /* Disable "Remove accessory" button if an empty slot is selected */
	  if(g_inslist[iac_chkd-ACC1NAME]) {
		objc_change(tree, ACREMV, 0, xd, yd, wd, hd, NORMAL, TRUE);
	  } else {
		objc_change(tree, ACREMV, 0, xd, yd, wd, hd, DISABLED, TRUE);	      
	  }
#endif
	  touchob = form_do(tree, 0);
	  touchob &= 0x7fff;
	  newstate = NORMAL;
	  move = 0;
	  graf_mkstate(&mx, &my, &kret, &bret);
	  // fprintf(logfile, "touchob: %d\n", touchob);
	  switch (touchob)
	  {
	    case ACC1NAME:
	    case ACC2NAME:
	    case ACC3NAME:
	      objc_change(tree, iac_chkd, 0, xd, yd, wd, hd, NORMAL, TRUE);
	      iac_chkd = touchob;
	      newstate = CHECKED;
	      break;

#if MULTIAPP
	    case ACC1FMEM:
	    case ACC2FMEM:
	    case ACC3FMEM:
	      spec = (LONG)(tree[touchob].ob_spec);
	      iac_schar(tree, touchob, LHIBT(LHIWD(spec))=='Y'?'N':'Y');
	      iac_redrw(tree, touchob, newstate = tree[touchob].ob_state, 0); 
	      break;
#endif
	    case ACREMV:
#if MULTIAPP
	      iac_strcop(tree, iac_chkd, "unused");
	      iac_redrw(tree, iac_chkd, CHECKED, 0);
#else
		  iac_swap(tree, -1, iac_chkd);
#endif
	      break;

	    case ACA1NAME:
	    case ACA2NAME:
	    case ACA3NAME:
	    case ACA4NAME:
	    case ACA5NAME:
	    case ACA6NAME:
	    case ACA7NAME:
	    case ACA8NAME:
	      chspec = (LPBYTE)tree[touchob].ob_spec;
	      if (iac_isnam(chspec))
	      {
#if MULTIAPP
	        iac_strcop(tree, iac_chkd, chspec);
	        iac_redrw(tree, iac_chkd, CHECKED, 0);
#else
			iac_swap(tree, touchob, iac_chkd);
#endif
	      }
	      break;

	    case ACFUPARO:
	      move = -1;
	      break;

	    case ACFDNARO:
	      move = 1;
	      break;
	    
	    case ACFSVSLI:
		ob_actxywh(tree, ACFSELEV, &pt);
/* APPLE	pt.g_x -= 3;
		pt.g_w += 6; */
		if ( inside(mx, my, &pt) )
		  goto dofelev;
		move = (my <= pt.g_y) ? -1 : 1;
		break;
	    case ACFSELEV:
dofelev:	wind_update(3);
		ob_relxywh(tree, ACFSVSLI, &pt);
/* APPLE 	pt.g_x += 3;
		pt.g_w -= 6; */
		tree[ACFSVSLI].ob_x = pt.g_x;
		tree[ACFSVSLI].ob_width = pt.g_w;
		move = graf_slidebox(tree, ACFSVSLI, ACFSELEV, TRUE);
		// fprintf(logfile, "move=%d, fcount=%d, fcurrtop=%d\n", move, fcount, fcurrtop);
/* APPLE 	pt.g_x -= 3;
		pt.g_w += 6; */
		tree[ACFSVSLI].ob_x = pt.g_x;
		tree[ACFSVSLI].ob_width = pt.g_w;
		wind_update(2);
		move = x_mul_div(move, fcount-8, 1000) - fcurrtop;
		// fprintf(logfile, "line move=%d\n", move);		
		break;

	    case ACINST:
	      ret = iac_save(tree);
	    case ACCNCL:
	      cont = FALSE;
	      break;

	    default:
	      break;
	  }
	  objc_change(tree, touchob, 0, xd, yd, wd, hd, newstate, TRUE);
	  if (move)
	  {
	    fcurrtop = iac_scroll(tree, fcurrtop, fcount, move);
	  }
    
    }					/* undraw the form	*/
	show_hide(FMD_FINISH, tree);
	tree[iac_chkd].ob_state = NORMAL;
	return(ret);
}

/************************************************************************/
/* i n s _ a c c	  						*/
/************************************************************************/

WORD  ins_acc()
{			       
	LPTREE	tree;
	WORD	ret;

	tree = G.a_trees[ADINSACC];

/* get current accessory names */
/* stuff them in slots in dialog */
	return iac_dial(tree);
} /* ins_acc */


