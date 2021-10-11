/*	OPTIMIZE.C	1/25/84 - 06/05/85	Lee Jay Lorenzen	*/
/*	merge source	5/28/87			mdf			*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985 - 1987		Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "ppddesk.h"

EXTERN UWORD	intin[];
EXTERN UWORD	intout[];
EXTERN UWORD	contrl[];


	WORD
bit_num(flag)
	UWORD		flag;
{
	WORD		i;
	UWORD		test;

	if ( !flag )
	  return(-1);
	for (i=0,test=1; !(flag & test); test <<= 1,i++);
	return(i);
}

VOID rc_constrain(LPGRECT pc, LPGRECT pt)
{
	  if (pt->g_x < pc->g_x)
	    pt->g_x = pc->g_x;
	  if (pt->g_y < pc->g_y)
	    pt->g_y = pc->g_y;
	  if ((pt->g_x + pt->g_w) > (pc->g_x + pc->g_w))
	    pt->g_x = (pc->g_x + pc->g_w) - pt->g_w;
	  if ((pt->g_y + pt->g_h) > (pc->g_y + pc->g_h))
	    pt->g_y = (pc->g_y + pc->g_h) - pt->g_h;
}
/*

VOID rc_union(LPGRECT p1, LPGRECT p2)
{
	WORD		tx, ty, tw, th;

	tw = wmax(p1->g_x + p1->g_w, p2->g_x + p2->g_w);
	th = wmax(p1->g_y + p1->g_h, p2->g_y + p2->g_h);
	tx = wmin(p1->g_x, p2->g_x);
	ty = wmin(p1->g_y, p2->g_y);
	p2->g_x = tx;
	p2->g_y = ty;
	p2->g_w = tw - tx;
	p2->g_h = th - ty;
}


	WORD
rc_intersect(p1, p2)
	GRECT		*p1, *p2;
{
	WORD		tx, ty, tw, th;

	tw = wmin(p2->g_x + p2->g_w, p1->g_x + p1->g_w);
	th = wmin(p2->g_y + p2->g_h, p1->g_y + p1->g_h);
	tx = wmax(p2->g_x, p1->g_x);
	ty = wmax(p2->g_y, p1->g_y);
	p2->g_x = tx;
	p2->g_y = ty;
	p2->g_w = tw - tx;
	p2->g_h = th - ty;
	return( (tw > tx) && (th > ty) );
}
*/

	WORD
mid(lo, val, hi)
	WORD		lo, val, hi;
{
	if (val < lo)
	  return(lo);
	if (val > hi)
	  return(hi);
	return(val);
}

	BYTE
*strscn(ps, pd, stop)
	BYTE		*ps, *pd, stop;
{
	while ( (*ps) &&
		(*ps != stop) )
	  *pd++ = *ps++;
	return(pd);
}




/*
*	Strip out period and turn into raw data.
*/
	VOID
fmt_str(instr, outstr)
	BYTE		*instr, *outstr;
{
	WORD		count;
	BYTE		*pstr;

	pstr = instr;
	while( (*pstr) && (*pstr != '.') )
	  *outstr++ = *pstr++;
	if (*pstr)
	{
	  count = 8 - (pstr - instr);
	  while ( count-- )
	    *outstr++ = ' ';
	  pstr++;
	  while (*pstr)
	    *outstr++ = *pstr++;
	}
	*outstr = 0;
}


/*
*	Insert in period and make into true data.
*/
	VOID
unfmt_str(instr, outstr)
	BYTE		*instr, *outstr;
{
	BYTE		*pstr, temp;

	pstr = instr;
	while( (*pstr) && ((pstr - instr) < 8) )
	{
	  temp = *pstr++;
	  if (temp != ' ')
	    *outstr++ = temp;
	}
	if (*pstr)
	{
	  *outstr++ = '.';
	  while (*pstr)
	  {
	    temp = *pstr++;
	    if (temp != ' ')
	      *outstr++ = temp;
	  }
	}
	*outstr = 0;
}


VOID fs_sset(LPTREE tree, WORD obj, LPBYTE pstr, LPBYTE *ptext, WORD *ptxtlen)
{
	LPTEDI spec;
/*	char buf[82]; */
	
	spec = (LPTEDI)tree[obj].ob_spec;

	/* Why are we copying these three in turn to the same buffer? 
	 * Debugging? 
	lstlcpy(ADDR(buf), spec->te_ptext,   sizeof(buf));
	lstlcpy(ADDR(buf), spec->te_ptmplt,  sizeof(buf));
	lstlcpy(ADDR(buf), spec->te_pvalid,  sizeof(buf));
*/	
	
	*ptext = spec->te_ptext;
	LSTCPY(*ptext, pstr);
	*ptxtlen = spec->te_txtlen;

/*	lstlcpy(ADDR(buf), spec->te_ptext, sizeof(buf)); */
}


VOID inf_sset(LPTREE tree, WORD obj, BYTE *pstr)
{
	LPBYTE		text;
	WORD		txtlen;

	fs_sset(tree, obj, ADDR(pstr), &text, &txtlen);
}


VOID fs_sget(LPTREE tree, WORD obj, LPBYTE pstr, WORD maxlen)
{
	LPBYTE		ptext;

	ptext = ((LPTEDI)(tree[obj].ob_spec))->te_ptext;
	lstlcpy(pstr, ptext, maxlen);
}


VOID inf_sget(LPTREE tree, WORD obj, BYTE *pstr, WORD maxlen)
{
	fs_sget(tree, obj, ADDR(pstr), maxlen);
}


/* v3.2: Allow proper field states, not the rather blunt methods of DR GEM */
VOID inf_fldset(LPTREE tree, WORD obj, 
				UWORD testfld, UWORD testbit, 
				UWORD truestate, UWORD falsestate)
{
	if (testfld & testbit) 
	{
		tree[obj].ob_state &= ~falsestate;
		tree[obj].ob_state |= truestate;
	}
	else
	{
		tree[obj].ob_state &= ~truestate;
		tree[obj].ob_state |= falsestate;
	}
}


WORD inf_gindex(LPTREE tree, WORD baseobj, WORD numobj)
{
	WORD		retobj;

	for (retobj=0; retobj < numobj; retobj++)
	{
	  if (tree[baseobj+retobj].ob_state & SELECTED)
	    return(retobj);
	}
	return(-1);
}


/*
*	Return 0 if cancel was selected, 1 if okay was selected, -1 if
*	nothing was selected.
*/

WORD inf_what(LPTREE tree, WORD ok, WORD cncl)
{
/* [JCE] Rewritten to avoid "dangerous" assumptions of object order */

	WORD		field = -1;

	if (tree[ok  ].ob_state & SELECTED) 
	{ 
		field  = 1; 
		tree[ok  ].ob_state &= ~SELECTED;
	}
	if (tree[cncl].ob_state & SELECTED) 
	{ 
		field  = 0;
		tree[cncl].ob_state &= ~SELECTED;
	}
	return(field);
}


VOID merge_str(BYTE *pdst, BYTE *ptmp, ...)
{
	va_list		ap;

	va_start(ap, ptmp);
	merge_v(pdst, ptmp, ap);
	va_end(ap);
	
}

VOID merge_v(BYTE *pdst, BYTE *ptmp, va_list ap)
{
	WORD		do_value;
	BYTE		lholder[12];
	BYTE		*pnum, *psrc;
	LONG		lvalue, divten;
	WORD		digit;
	
	while(*ptmp)
	{
		if (*ptmp != '%') 
		{
			*pdst++ = *ptmp++;
			continue;
		}
	    ptmp++;
	    do_value = FALSE;
	    switch(*ptmp++)
	    {
	      case '%':
			*pdst++ = '%'; break;
	      case 'L':
			lvalue = va_arg(ap, LONG);
			do_value = TRUE;
			break;
	      case 'W':
			lvalue = va_arg(ap, UWORD);
			do_value = TRUE;
			break;
	      case 'S':
			psrc = va_arg(ap, BYTE *);
			while(*psrc)
		  		*pdst++ = *psrc++;
			break;
	    }
	    if (do_value)
	    {
	    	pnum = &lholder[0];
	     	while(lvalue)
	    	{
				divten = lvalue / 10;
				digit  = lvalue % 10;
				*pnum++ = '0' + ((BYTE) digit);
				lvalue = divten;
	      	}
	      	if ( pnum == ((BYTE *) &lholder[0]) ) *pdst++ = '0';
	      	else while(pnum != ((BYTE *) &lholder[0]) )
		  		*pdst++ = *--pnum;
	    }
	}
	*pdst = 0;
}

/*
*	Routine to see if the test filename matches one of a set of 
*	comma delimited wildcard strings.
*		e.g.,	pwld = "*.COM,*.EXE,*.BAT"
*		 	ptst = "MYFILE.BAT"
*/
WORD wildcmp(BYTE *pwld, BYTE *ptst)
{
	BYTE		*pwild;
	BYTE		*ptest;
						/* skip over *.*, and	*/
						/*   *.ext faster	*/
	while(*pwld)
	{
	  ptest = ptst;
	  pwild = pwld;
						/* move on to next 	*/
						/*   set of wildcards	*/
	  pwld = scasb(pwld, ',');
	  if (*pwld)
	    pwld++;
						/* start the checking	*/
	  if (pwild[0] == '*')
	  {
	    if (pwild[2] == '*')
	      return(TRUE);
	    else
	    {
	      pwild = &pwild[2];
	      ptest = scasb(ptest, '.');
	      if (*ptest)
	        ptest++;
	    }
	  }
						/* finish off comparison*/
	  while( (*ptest) && 
	         (*pwild) &&
		 (*pwild != ',') )
	  {
	    if (*pwild == '?')
	    {
	       pwild++;
	       if (*ptest != '.')
	         ptest++;
	    }
	    else
	    {
	      if (*pwild == '*')
	      {
	        if (*ptest != '.')
		  ptest++;
	        else		
		  pwild++;
	      }
	      else
	      {
	        if (*ptest == *pwild)
	        {
	          pwild++;
	          ptest++;
	        }
	        else
	          break;
	      }
	    }
	  }
						/* eat up remaining 	*/
						/*   wildcard chars	*/
	  while( (*pwild == '*') ||
	         (*pwild == '?') ||
	         (*pwild == '.') )
	    pwild++;
						/* if any part of wild-	*/
						/*   card or test is	*/
						/*   left then no match	*/
	  if ( ((*pwild == 0) || (*pwild == ',')) && 
	       (!*ptest) )
	    return( TRUE );
	}
	return(FALSE);
}



/*
*	Routine to insert a character in a string by
*/
	VOID
ins_char(str, pos, chr, tot_len)
	REG BYTE	*str;
	WORD		pos;
	BYTE		chr;
	REG WORD	tot_len;
{
	REG WORD	ii, len;

	len = strlen(str);

	for (ii = len; ii > pos; ii--)
	  str[ii] = str[ii-1];
	str[ii] = chr;
	if (len+1 < tot_len)
	  str[len+1] = 0;
	else
	  str[tot_len-1] = 0;
}





