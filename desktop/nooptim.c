/*	NOOPTIM.C	10/11/21	Davide Bresolin	    */

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

/*
*	Enable/Disable the menu items in dlist
*/
VOID __near men_list(LPTREE mlist, BYTE *dlist, WORD enable)
{
	while (*dlist)
	  menu_ienable(mlist, *dlist++, enable);
}

/*
*	Scan off and convert the next two hex digits and return with
*	pcurr pointing one space past the end of the four hex digits
*/

BYTE * __near scan_2(BYTE *pcurr, WORD *pwd)
{
	UWORD		temp;
	
	temp = 0x0;
	temp |= hex_dig(*pcurr++) << 4;
	temp |= hex_dig(*pcurr++);
	if (temp == 0x00ff) temp = NIL;
	*pwd = temp;
	return(	pcurr );
}

VOID  __near merge_v(BYTE *pdst, BYTE *ptmp, va_list ap)
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
