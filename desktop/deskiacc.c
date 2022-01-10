
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

#if MULTIAPP

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

GLOBAL LPBYTE	ad_tmp1;
GLOBAL BYTE	gl_tmp1[LEN_FSNAME];
GLOBAL LPBYTE	ad_tmp2;
GLOBAL BYTE	gl_tmp2[LEN_FSNAME];
GLOBAL BYTE	*g_fslist[NUM_AFILES];
GLOBAL BYTE	g_fsnames[LEN_FSNAME * NUM_AFILES];


#if DEBUG
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
	    strcpy("        ", &accstr[0]);	/* eight blanks		*/
	    for (j=0; (j<8) && npt[j] && (npt[j] != '.'); j++)
	      accstr[j] = npt[j];
	    if (appl_find(ADDR(&accstr[0])) < 0)
	      npt[0] = '\0';
	    else				/* check for no full step */
	      gl_keepac |= (gl_caccs[i].acc_swap == 'N');
	  }
	}
}


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

VOID  iac_mvnames(LPTREE tree, WORD start, WORD num)
{
	WORD		i, j, k;
	WORD		len;

	for (i=0; i<num; i++)
	{
	  _fstrcpy(ad_tmp1, (LPBYTE)ADDR(g_fslist[i+start]));
	  len = 0;
	  while (gl_tmp1[len] != '.')
	    len++;
	  if (len < 8)				/* blank pad in middle	*/
	  {
	    for (j=11, k=len+3; j > 7; j--, k--)
	      gl_tmp1[j] = gl_tmp1[k];
	    for (j=len; j < 8; j++)
	      gl_tmp1[j] = ' ';
	  }
	  iac_strcop(tree, ACA1NAME+i, ad_tmp1);
	}
}

WORD  iac_names(LPTREE tree)
{
	WORD		ret;
	WORD		len;
	WORD		i, j, gap;
	WORD		thefile;
	BYTE		*ptr, *temp;

					/* find all .acc files in \GEMBOOT */
					/* stuff first 8 in object	*/
					/* adjust elevator size to number */
	thefile = 0;
	ptr = &g_fsnames[0];
	ad_tmp1 = (LPBYTE)ADDR(&gl_tmp1[0]);
	ad_tmp2 = (LPBYTE)ADDR(&gl_tmp2[0]);
	dos_sdta(G.a_wdta);
	strcpy(":\\GEMBOOT\\*.ACC", &G.g_cmd[1]);
	G.g_cmd[0] = gl_bootdr;
	ret = dos_sfirst(G.a_cmd, 0x16);
	while ( ret )
	{
	  _fstrcpy(ADDR(g_fslist[thefile] = ptr), G.a_wdta+30);
	  len = _fstrlen(ptr);
	  ptr += len+1;

	  ret = dos_snext();

	  if (thefile++ >= NUM_AFILES)
	  {
	    ret = FALSE;
	    desk_sound(TRUE, 660, 4);
	  }
	}

	for(gap = thefile/2; gap > 0; gap /= 2)
	{
	  for(i = gap; i < thefile; i++)
	  {
	    for (j = i-gap; j >= 0; j -= gap)
	    {
	      _fstrcpy(ad_tmp1, ADDR(g_fslist[j]));
	      _fstrcpy(ad_tmp2, ADDR(g_fslist[j+gap]));
	      if ( iac_comp() <= 0 )
		break;
	      temp = g_fslist[j];
	      g_fslist[j] = g_fslist[j+gap];
	      g_fslist[j+gap] = temp;
	    }
	  }
	}
	iac_mvnames(tree, 0, min(thefile, NUM_FSNAME));
	return(thefile);
}


#define ACCMIN	0x2000


VOID  iac_save(LPTREE tree)
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
}


WORD  iac_scroll(LPTREE tree, WORD currtop, WORD count, WORD move)
{
	WORD		newtop;

	if (count <= NUM_FSNAME)
	  return(0);
	newtop = currtop + move;
	newtop = max(newtop, 0);
	newtop = min(newtop, count - NUM_FSNAME);
	if (newtop == currtop)
	  return(currtop);
	iac_elev(tree, newtop, count);
	iac_mvnames(tree, newtop, NUM_FSNAME);
	iac_redrw(tree, ACNAMBOX, NORMAL, 1);
	iac_redrw(tree, ACFSVSLI, NORMAL, 1);
	return(newtop);
}


WORD  iac_dial(LPTREE tree)
{
	WORD		touchob;
	WORD		cont;
	WORD		xd, yd, wd, hd;
	LONG		spec;
	LPBYTE		chspec;
	WORD		newstate;
	WORD		i;
	WORD		fcurrtop, fcount;
	WORD		move;
	GRECT		pt;
	WORD		mx, my, kret, bret;
	
	iac_chkd = ACC1NAME;
	fcount = iac_names(tree);
	fcurrtop = 0;
	iac_elev(tree, fcurrtop, fcount);
	for (i=0; i<3; i++)
	{
	  tree[ACC1NAME + i].ob_state = NORMAL;
	  chspec = ADDR(&(gl_caccs[i].acc_name[0]));
	  if (iac_isnam(chspec))
	  {
	    iac_strcop(tree, ACC1NAME+i, chspec);
	    iac_schar(tree, ACC1FMEM+i, gl_caccs[i].acc_swap);
	  }
	  else
	  {
	    iac_strcop(tree, ACC1NAME+i, ADDR("unused"));
	    iac_schar(tree, ACC1FMEM+i, 'Y');
	  }
	}
	tree[iac_chkd].ob_state = CHECKED;

	form_center(tree, &xd, &yd, &wd, &hd);
	form_dial(FMD_START, 0, 0, 0, 0, xd, yd, wd, hd);
	objc_draw(tree, ROOT, MAX_DEPTH, xd, yd, wd, hd);
	
	cont = TRUE;
	while (cont)
	{
	  touchob = form_do(tree, 0);
	  touchob &= 0x7fff;
	  newstate = NORMAL;
	  move = 0;
	  graf_mkstate(&mx, &my, &kret, &bret);
	  switch (touchob)
	  {
	    case ACC1NAME:
	    case ACC2NAME:
	    case ACC3NAME:
	      objc_change(tree, iac_chkd, 0, xd, yd, wd, hd, NORMAL, TRUE);
	      iac_chkd = touchob;
	      newstate = CHECKED;
	      break;

	    case ACC1FMEM:
	    case ACC2FMEM:
	    case ACC3FMEM:
	      spec = (LONG)(tree[touchob].ob_spec);
	      iac_schar(tree, touchob, LHIBT(LHIWD(spec))=='Y'?'N':'Y');
	      iac_redrw(tree, touchob, newstate = tree[touchob].ob_state, 0); 
	      break;

	    case ACREMV:
	      iac_strcop(tree, iac_chkd, ADDR("unused"));
	      iac_redrw(tree, iac_chkd, CHECKED, 0);
	      break;

	    case ACA1NAME:
	    case ACA2NAME:
	    case ACA3NAME:
	    case ACA4NAME:
	    case ACA5NAME:
	    case ACA6NAME:
	    case ACA7NAME:
	    case ACA8NAME:
	      chspec = *((LPBYTE FAR *)(tree[touchob].ob_spec));
	      if (iac_isnam(chspec))
	      {
	        iac_strcop(tree, iac_chkd, chspec);
	        iac_redrw(tree, iac_chkd, CHECKED, 0);
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
/* APPLE 	pt.g_x -= 3;
		pt.g_w += 6; */
		tree[ACFSVSLI].ob_x = pt.g_x;
		tree[ACFSVSLI].ob_width = pt.g_w;
		wind_update(2);
		move = x_mul_div(move, fcount-1, 1000) - fcurrtop;
		break;


	    case ACINST:
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
	return(touchob);
}

/************************************************************************/
/* i n s _ a c c	  						*/
/************************************************************************/

VOID  ins_acc()
{			       
	LPTREE		tree;

	tree = G.a_trees[ADINSACC];

/* get current accessory names */
/* stuff them in slots in dialog */

	if (iac_dial(tree) == ACINST)
	{
	  iac_save(tree);
/* copy names from tree to current acc list */
/* delete some/all current accs and free channels */
/* run the new accessories */

	}
} /* ins_acc */

#endif /* MULTIAPP */
