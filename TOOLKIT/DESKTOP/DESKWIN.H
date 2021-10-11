/*	DESKWIN.H	06/11/84 - 01/04/85		Lee Lorenzen	*/
/*	changed NUM_WOBS from 128 to 300	11/19/87	mdf	*/
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

#define DROOT 1
/* For DESKTOP v1.2 */
#define NUM_WNODES 4
#define NUM_WOBS 128
/*
#define NUM_WNODES 2
#define NUM_WOBS 300
*/
#define NUM_SOBS (NUM_WOBS + NUM_WNODES + 1)


/* Window controls for a regular (non-split mode) window. */
#define WKIND_FILE	(NAME | INFO | VSLIDE | DNARROW | UPARROW \
			    | FULLER | CLOSER | MOVER | SIZER)

/* Window controls for the right and left sides of split-mode windows.	*/
/* NOTE: CLOSE is on the TREEWIN and FULL is on the SIBLWIN.		*/
#define WKIND_RSPLIT (NAME | INFO | FULLER | VSLIDE | DNARROW | UPARROW | \
			MOVER | SIZER)
#define WKIND_LSPLIT (NAME | INFO | CLOSER | VSLIDE | DNARROW | UPARROW | \
			HSLIDE | RTARROW | LFARROW | MOVER | SIZER)
			    
/* Window controls for a program group window. */
#define WKIND_GROUP	(NAME | VSLIDE | DNARROW | UPARROW \
			    | FULLER | CLOSER | MOVER | SIZER)


/* Window attributes for Show Contents window */
#define WKIND_SHOW   ( WKIND_FILE | HSLIDE | RTARROW | LFARROW )

/* Values for w_type field in a WNODE */
#define FILEWIN		0	/* Ordinary, unsplit directory window	*/
#define	SIBLWIN		1	/* Right side of a split mode window	*/
#define TREEWIN		2	/* Left side of a split mode window	*/
#define GROUPWIN	4	/* Program Group window			*/

typedef struct windnode
{
	struct windnode	*w_next;	// 00
	WORD		w_flags;	// 02
	WORD		w_id;		// 04	/* window handle id #	*/
	WORD		w_obid;		// 06	/* desktop object id	*/
	WORD		w_root;		// 08	/* pseudo root ob. in	*/
						/*   gl_screen for this	*/
						/*   windows objects	*/
	WORD		w_cvcol;	// 0A	/* current virt. col	*/
	WORD		w_cvrow;	// 0C	/* current virt. row	*/
	WORD		w_pncol;	// 0E	/* physical # of cols	*/
	WORD		w_pnrow;	// 10	/* physical # of rows	*/
	WORD		w_vncol;	// 12	/* virtual # of cols 	*/
	WORD		w_vnrow;	// 14	/* virtual # of rows	*/
	PNODE		*w_path;	// 16
	BYTE		w_name[LEN_ZPATH];
// DESKTOP v1.2 uses this line...
	BYTE		w_info[81];	/*	NOT USED v2.1		*/
} WNODE;


