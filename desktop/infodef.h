/*	INFODEF.H	09/26/84 - 09/26/84		Gregg Morris	*/
/*	merge source	5/26/87				mdf		*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985, 1986		Digital Research Inc.
*	-------------------------------------------------------------
*/

typedef struct fcb
{
	BYTE		fcb_reserved[21];
	BYTE		fcb_attr;
	WORD		fcb_time;
	WORD		fcb_date;
	LONG		fcb_size;
	BYTE		fcb_name[13];
} FCB;

#define ARROW 0
#define HGLASS 2

#define FLOPPY 0
#define HARD 1

