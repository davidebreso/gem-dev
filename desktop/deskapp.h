/*	DESKAPP.H	06/11/84 - 06/11/85		Lee Lorenzen	*/
/*	for 3.0		1/21/86  - 12/30/86		MDF		*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/


#define AP_APPLOPEN 0
#define AP_DATAOPEN 1
#define AP_DATADRAG 2

#define AF_ISCRYS 0x0001			/* is crystal appl.	*/
#define AF_ISGRAF 0x0002			/* is graphic appl.	*/
#define AF_ISDESK 0x0004			/* is placed on desk	*/
#define AF_ISPARM 0x0008			/* is in need of input	*/
						/*   parameters		*/
#define AF_ISFMEM 0x0010			/* is in need of full	*/
						/*   memory		*/
#define AF_WASDET 0x0020			/* disk was autodetected*/
						/* BALJ 19990829        */

#define AT_ISFILE 0				/* file                 */
#define AT_ISFOLD 1				/* folder               */
#define AT_ISDISK 2				/* disk or diskette     */
#define AT_ISTRSH 3				/* Trash                */
#define AT_ISPITM 4				/* Program item         */
#define AT_ISWIND 5				/* Windows app          */
#define AT_ISGRP  6				/* Program group        */

						/* gem icons		*/
#define IG_HARD   0
#define IG_FLOPPY 1
#define IG_FOLDER 2
#define IG_TRASH  3
#define IG_NET    4				/* Added 19990721 BALJ  */
#define IG_5QRTR  5
#define IG_CD     6
#define IG_7RESV  7
						/* application icons	*/
#define IA_GENERIC 8
#define IA_SS 9
#define IA_WP 10
#define IA_DB 11
#define IA_DRAW 12
#define IA_PAINT 13
#define IA_PROJECT 14
#define IA_GRAPH 15
#define IA_OUTLINE 16
#define IA_ACCNT 17
#define IA_MULTI 18
#define IA_EDUC 19
#define IA_COMM 20
#define IA_TOOL 21
#define IA_GAME 22
#define IA_OUTPUT 23
						/* document icons	*/
#define ID_GENERIC 40
#define ID_SS 41
#define ID_WP 42
#define ID_DB 43
#define ID_DRAW 44
#define ID_PAINT 45
#define ID_PROJECT 46
#define ID_GRAPH 47
#define ID_OUTLINE 48
#define ID_ACCNT 49
#define ID_MULTI 50
#define ID_EDUC 51
#define ID_COMM 52
#define ID_TOOL 53
#define ID_GAME 54
#define ID_OUTPUT 55

#define LEN_ZPATH 67
#define LEN_ZFNAME 13

#define NUM_ANODES 64				/* # of appl. nodes	*/
#define SIZE_AFILE 2048				/* size of DESKTOP.APP	*/
						/*   file		*/
#define SIZE_BUFF 4096				/* size of a string 	*/
						/*   buffer		*/
#define NUM_IBLKS 72
#define NUM_NAMICS ((NUM_IBLKS - 8)/2)		/* # of named icons	*/
						/*   possible		*/

typedef struct applstr
{
	struct applstr	*a_next;	/* 00 */
	WORD		a_flags;		/* 02 */
	WORD		a_type;			/* 04 icon/appl type	*/
	WORD		a_obid;			/* 06 object index		*/
	BYTE		*a_pappl;		/* 08 filename.ext of ap.	*/
	BYTE		*a_pdata;		/* 0A wldcards of data file*/
	WORD		a_aicon;		/* 0C application icon #	*/
	WORD		a_dicon;		/* 0E data icon #		*/
	WORD		a_letter;		/* 10 letter for icon	*/
	WORD		a_xspot;		/* 12 desired spot on desk	*/
	WORD		a_yspot;		/* 14 desired spot on desk	*/
} ANODE;

#if 1

#define a_memreq	a_xspot
#define DEFMEMREQ	256

#define ACCNODE	struct acc_info
ACCNODE
{
/*	WORD	acc_chan;	*/
	WORD	acc_swap;
	BYTE	acc_name[13];
};

#endif


typedef struct window_save
{
	WORD	x_save;			// 00
	WORD	y_save;			// 02
	WORD	w_save;			// 04
	WORD	h_save;			// 06
	WORD	hsl_save;		// 08
	WORD	vsl_save;		// 0A
	WORD	obid_save;		// 0C
	BYTE	pth_save[LEN_ZPATH];	// 0E
} WSAVE;


typedef struct context_save
{
	WORD	sitem_save;
	WORD	vitem_save;
	WORD	ccopy_save;
	WORD	cdele_save;
	WORD	covwr_save;	// Commented out bits not in DESKTOP v1.2 
	WORD	cdclk_save;
	WORD	cmclk_save;
	WORD	ctmfm_save;
	WORD	cdtfm_save;
	WORD    cdetd_save;                     /* BALJ - det. drives   */
	WORD    cdetn_save;                     /* BALJ - det. network  */
	WSAVE	win_save[4];
} CSAVE;
