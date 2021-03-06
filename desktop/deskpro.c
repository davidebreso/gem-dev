/*	DESKPRO.C	4/18/84 - 03/19/85	Lee Lorenzen		*/
/*	for 3.0		3/11/86 - 01/28/87	MDF			*/
/*	merge source	5/27/87	- 5/28/87	mdf			*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985 - 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "wccdesk.h"

/* forward declarations */
MLOCAL WORD  pro_exec(WORD isgraf, WORD isover, LPBYTE pcmd, LPBYTE ptail);

#if MULTIAPP

#define LMIN(x,y) ((x)<(y)?(x):(y))


	/* in pro_chcalc long addresses are flattened out with no segment */

MLOCAL VOID  pro_chcalc(LONG appsize, LPBYTE *begaddr, LONG *chsize)
{
	static LONG	begfree = 0l;
	LONG		maxmem;

	if (appsize == -1)			/* full step	*/
	{
	  *begaddr = (LPBYTE)LSEGOFF(pr_beggem); 
	  *chsize = pr_topmem - pr_beggem;
	  return;
	}
	if ((begfree >= pr_topmem) || (begfree < pr_topdsk))
	  begfree = pr_topdsk;	
	
	maxmem = pr_topmem - pr_begdsk;
	*chsize = LMIN(appsize+pr_ssize, maxmem);

	if ((begfree + *chsize) < pr_topmem)
	  *begaddr = (LPBYTE)LSEGOFF(begfree);
	else
	  *begaddr = (LPBYTE)LSEGOFF(pr_topmem - *chsize);
	begfree += *chsize;	
}
#endif


WORD  pro_chdir(WORD drv, BYTE *ppath)
{
	WORD		tmpdrv;
						/* change to directory	*/
						/*   that application	*/
						/*   is in		*/
	// dbg("pro_chdir: %d %s\n", drv, ppath);
	if (!drv)
	  return( (DOS_ERR = TRUE) );

	if ( drv != '@' ) 
	{
	  tmpdrv = dos_gdrv();
	  dos_sdrv(drv - 'A');
	  /***** remove error check
	  if (DOS_ERR)
	  {
	    // dbg("deskpro.c:66 Error %d in dos_sdrv(%d)\n", DOS_ERR, drv - 'A');
	    dos_sdrv(tmpdrv);
	    // dbg("deskpro.c:71 Error %d in dos_sdrv(%d)\n", DOS_ERR, tmpdrv);
	    return(FALSE);
	  }
	  ********/
	  G.g_srcpth[0] = drv;
	  G.g_srcpth[1] = ':';
	  G.g_srcpth[2] = '\\';
	  strcpy(G.g_srcpth + 3, ppath);
	  dos_chdir(ADDR(&G.g_srcpth[0]));
	  if (DOS_ERR)
	  {
	    // dbg("deskpro.c:80 Error %d in dos_chdir( )\n", DOS_ERR);
	    dos_sdrv(tmpdrv);
	    return(FALSE);
	  }
	}
	else {
	  dos_sdrv(gl_stdrv);		/* don't leave closed drive hot */
	  // dbg("deskpro.c: 63 drv == '@'\n", DOS_ERR, gl_stdrv);
	}
	return(TRUE);
} /* pro_chdir */

WORD  pro_cmd(BYTE *psubcmd, BYTE *psubtail, WORD exitflag)
{
	LPBYTE		lp;
	WORD		i, ii, drv;
	BYTE		save_ch;

	shel_envrn(ADDR(&lp), ADDR("COMSPEC="));
	if (lp)
	{
	  lstlcpy(ADDR(&G.g_cmd[0]), lp, sizeof(G.g_cmd));
/* BugFix	*/
	  if (!exitflag)
	  {
	    i = 0;
	    while(G.g_cmd[i] != '\\' && G.g_cmd[i] != '/')	
		    /* find first backslash		*/
	      i++;
				/* change to drive specified by COMSPEC	*/
	    drv = G.g_cmd[i - 2] - 'A';
	    dos_sdrv(drv);
	    // dbg("deskpro.c:106 Error %d in dos_sdrv(%d)\n", DOS_ERR, drv);
				/* chdir to path specified by COMSPEC	*/
/* mdf */
	    ii = strlen(&G.g_cmd[0]);
	    while(G.g_cmd[--ii] != '\\');	/* find last backslash	*/
	    if (i == ii)		/* root directory?		*/
	      ii++;			/* keep backslash in path name	*/
/* *** */
	    save_ch = G.g_cmd[ii];	/* save char for later		*/
	    G.g_cmd[ii] = 0;		/* make a null-term. string	*/
	    dos_chdir(ADDR(&G.g_cmd[0]));  /* change to that dir.	*/
	    G.g_cmd[ii] = save_ch;	/* put the char back		*/
	  }
/* */
	  if (exitflag)
	  {
#if PCDOS
	    strcpy(G.g_tail + 1, "/C");
#endif
	    strcat(G.g_tail + 1, psubcmd);
	    strcat(G.g_tail + 1, " ");
	    strcat(G.g_tail + 1, psubtail);
	  }
	  else  G.g_tail[1] = 0;
	  return(TRUE);
	} /* if lp */
	else
	  return(FALSE);
} /* pro_cmd */


WORD  pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr)
{
	WORD		ret, len, i;

	G.g_tail[0] = len = strlen(&G.g_tail[1]);
	if ( (len) && (!isgraf) )
	{
	  for(i = len; i; i--)
	    G.g_tail[i+1] = G.g_tail[i];
	  G.g_tail[1] = ' ';
	  len++;
	} /* if */
	G.g_tail[0] = len;
	G.g_tail[len+1] = 0x0D;
#if MULTIAPP
	if (isover != 3)		/* keep icon SELECTED during FORMAT */
	  do_wopen(FALSE, wh, curr, G.g_xdesk, G.g_ydesk, G.g_wdesk, G.g_hdesk);
#endif
	ret = pro_exec(isgraf, isover, G.a_cmd, G.a_tail);
	if (isover == -1)
	  ret = FALSE;
	else
	{
#if MULTIAPP
	  if (isover == 3)			/* for FORMAT		*/
#else
	  if (wh != -1)
#endif
	    do_wopen(FALSE, wh, curr, G.g_xdesk, G.g_ydesk,
	    	     G.g_wdesk, G.g_hdesk);
	} /* else */
	return(ret);
} /* pro_run */



MLOCAL WORD  pro_exec(WORD isgraf, WORD isover, LPBYTE pcmd, LPBYTE ptail)
{
	WORD		ret;
#if MULTIAPP
	WORD		chnum;
	LPBYTE		begaddr;
	LONG		csize;

	if (isover != 3)
#endif
	graf_mouse(HGLASS, 0x0L);

#if MULTIAPP
	if ((isover == -1) || (isover == 2) || (isover == 3))
	{
	  if (isover == 2)			/* full step	*/
	    pro_chcalc((LONG)-1, &begaddr, &csize);
	  else
	    pro_chcalc((LONG)pr_kbytes << 10, &begaddr, &csize);
/*
dbg("NEW START ADDRESS = %X\r\n", begaddr);
dbg("NEW CHANNEL SIZE  = %X\r\n", csize);
*/
	  ret = proc_create(begaddr, csize, 1, isgraf, &chnum);
	  if (!ret)
	  {
	    fun_alert(1,STNOROOM);
	    return(FALSE);
	  }
	  if (isover == 2)
	    gl_fmemflg |= (1 << chnum);

/*
dbg("CREATE = %d\r\n", ret);
dbg("CHNUM  = %d\r\n", chnum);
dbg("PCMD   = %s\r\n", pcmd);
dbg("PTAIL  = %s\r\n", ptail);
*/
	  ret = proc_run(chnum, isgraf, isover, pcmd, ptail);
	  if (isover==3)
	    ret = 0;
	}
	else
#endif
	  ret = shel_write(TRUE, isgraf, isover, pcmd, ptail);
	if (!ret)
	  graf_mouse(ARROW, 0x0L);
	return( ret );
} /*  */

WORD  pro_exit(LPBYTE pcmd, LPBYTE ptail)
{
	WORD		ret;

	ret = shel_write(FALSE, FALSE, 1, pcmd, ptail);
	return( ret );
} /* pro_exit */

WORD  pro_restart(LPBYTE pcmd, LPBYTE ptail)
{
	WORD		ret;

	ret = shel_write(TRUE, FALSE, 2, pcmd, ptail);
	return( ret );
} /* pro_exit */
