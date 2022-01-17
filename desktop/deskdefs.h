/*	DESKDEFS.H - 5 November 2001 John Elliott */

/*
*       Copyright 1999, 2001 Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*/

/* The minimum GEM version that supports menu_click(). 
 * Single-tasking GEM 3.0 supports it, but multitasking GEM/XM 2.3 (which
 * reports 3.0) does not. If you add menu_click() support to GEM/XM, please
 * bump the version number to match. */
#if MULTIAPP
#define MIN_MENU_CLICK 0x301
#else
#define MIN_MENU_CLICK 0x300
#endif

#define abs(x) ( (x) < 0 ? -(x) : (x) )

#define WORD_ALIGN(x) ((x) + 8 & 0xfff0)
/* BugFix	*/
					/* keyboard shortcuts & others	*/
#define ESCAPE 0x011B			/* resort directory in window	*/
#define ALTA 0x1E00			/* Configure App		*/
#define ALTC 0x2E00			/* Enter DOS commands		*/
#define ALTD 0x2000			/* Delete			*/
#define ALTI 0x1700			/* Info/Rename			*/
#define ALTN 0x3100			/* Sort by Name			*/
#define ALTP 0x1900			/* Sort by Type			*/
#define ALTS 0x1F00			/* Show as Icons     		*/
#define ALTT 0x1400			/* Sort by Date			*/
#define ALTV 0x2F00			/* Save Desktop			*/
#define ALTX 0x2D00			/* Show as Text			*/
#define ALTZ 0x2C00			/* Sort by Size			*/
#define CNTLA 0x1E01		/* Select all items		*/
#define CNTLN 0x310E		/* Cycle windows		*/
#define CNTLQ 0x1011			/* Exit To Dos			*/
#define CNTLR 0x1312		/* Refresh Window		*/
#define CNTLU 0x1615			/* To Output			*/
#define CNTLW 0x1117			/* Close Window			*/
#define CNTLX 0x2D18			/* Close Folder			*/

#define BEG_UPDATE 1
#define END_UPDATE 0

#define NUM_BB	1

/* */
#define SPACE 0x20


#define E_BADFUNC 1
#define E_FILENOTFND 2
#define E_PATHNOTFND 3
#define E_NOHANDLES 4
#define E_NOACCESS 5
#define E_BADHANDLE 6
#define E_MEMBLKERR 7
#define E_NOMEMORY 8
#define E_BADMEMBLK 9
#define E_BADENVIR 10
#define E_BADFORMAT 11
#define E_BADACCESS 12
#define E_BADDATA 13
#define E_BADDRIVE 15
#define E_NODELDIR 16
#define E_NOTDEVICE 17
#define E_NOFILES 18

#define F_RDONLY 0x01
#define F_HIDDEN 0x02
#define F_SYSTEM 0x04
#define F_VOLUME 0x08
#define F_SUBDIR 0x10
#define F_ARCHIVE 0x20
						/* DESKTOP FLAGS for	*/
						/*   attribute byte	*/
#define F_FAKE 0x40
#define F_DESKTOP 0x80

#define ALLFILES    (F_SUBDIR|F_SYSTEM|F_HIDDEN)

/*
 * attributes used to select files/folders for desktop window display.
 * used by pn_active() when building the list of FNODEs to display, and
 * by search_recursive() when searching for matching FNODEs.
 */
#define DISPATTR    F_SUBDIR


#define F_GETMOD 0x0
#define F_SETMOD 0x1

typedef struct exec_blk
{
	WORD		eb_segenv;
	LONG		eb_pcmdln;
	LONG		eb_pfcb1;
	LONG		eb_pfcb2;
} EXEC_BLK;


typedef struct over_blk
{
	WORD		ob_seglod;
	WORD		ob_relfac;
} OVER_BLK;

#define E_PASSWFAIL 0x56

#define LSEGOFF(x) ((((x)+15)&0xFFFFFFF0)<<12)


