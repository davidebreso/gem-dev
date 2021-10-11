/*	DESKFPD.H	06/11/84 - 03/25/85	Lee Lorenzen	*/
/*	for 3.0		11/4/87			mdf		*/
/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 3.0
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/


#define OP_COUNT 0
#define OP_DELETE 1
#define OP_COPY 2

#define D_PERM 0x0001

#define V_ICON 0
#define V_TEXT 1

#define S_NAME 0
#define S_DATE 1
#define S_SIZE 2
#define S_TYPE 3
#define S_DISK 4

#define E_NOERROR 0
#define E_NOFNODES 100
#define E_NOPNODES 101
#define E_NODNODES 102

#define NUM_FNODES 400
#define NUM_PNODES 6
					/* one more than windows for	*/
					/*   unopen disk copy		*/
					/* [JCE] I've assumed that for DESKTOP v1.2 we have */
					/* 4 windows + desktop + unopen disk copy */
typedef struct filenode
{
	struct filenode	*f_next;		// 00
	BYTE		f_junk;				// 02 /* to align on even boundaries	*/
	unsigned char	f_attr;			// 03
/* BugFix	*/
	UWORD		f_time;				// 04
	UWORD		f_date;				// 06
/* */
	LONG		f_size;				// 08
	char		f_name[LEN_ZFNAME];	// 0C
	WORD		f_obid;				// 19 1A 
	ANODE		*f_pa;				// 1B 1C
	WORD		f_isap;				// 1D 1E
} FNODE;


typedef struct pathnode
{
	struct pathnode		*p_next;	// 00
	WORD		p_flags;			// 02
	WORD		p_attr;				// 04
	BYTE		p_spec[LEN_ZPATH];	// 06
	FNODE		*p_flist;			// 0x4A
	WORD		p_count;			// 0x4C
	LONG		p_size;				// 0x4E
} PNODE;


