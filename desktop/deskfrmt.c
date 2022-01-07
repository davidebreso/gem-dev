/*	DESKFRMT.C	06/01/21 - Davide Bresolin		*/

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
#include "wccdesk.h"

/*
 *      declarations used by the do_format() code
 */
#define MAXTRACK        80

typedef struct DISKBPB {
     UWORD  Bytessect;      /* bytes per sector */
     UBYTE  Sectclust;      /* sectors per cluster */
     UWORD  Resvsect;       /* number of reserved sectors */
     UBYTE  Numfats;        /* number of FATs */
     UWORD  Rootdir;        /* number of root directory entries */
     UWORD  Numsect;        /* total number of sectors */
     UBYTE  Mediaid;        /* media descriptor */
     UWORD  Sectfat;        /* number of sectors per FAT */
     UWORD  Secttrack;      /* sectors per track */
     UWORD  Numheads;       /* number of heads */
     ULONG  Numhidden;      /* # of hidden sectors */
     ULONG  Numhuge;        /* # of sectors if bpbSectors == 0 */ 
     UBYTE  Unused[6];      /* unused data */
} DISKBPB;

typedef struct DISKPARM {
    UBYTE   Specfunc;           /* Special function */
    UBYTE   Devicetype;         /* Device type */
    UWORD   Deviceattr;         /* Device attributes */
    UWORD   Cylinders;          /* Number of cylinders */
    UBYTE   Mediatype;          /* Media type */
    DISKBPB Devicebpb;          /* Device BPB */
    UWORD  Numcyls;             /* number of cylinders */
    UWORD  Tracklayout[18*2];   /* track layout */
} DISKPARM;

typedef struct FMTPARM {
    UBYTE   Specfunc;       /* Special function */
    UWORD   Head;           /* number of disk Head */
    UWORD   Cylinder;       /* number of disk cylinder */
} FMTPARM;

typedef struct WRITEPKT {
    ULONG   Sectbegin;  /* start sector number */
    UWORD   Sectcount;  /* number of sectors to write */
    LPVOID  Buffstart;  /* transfer address */
} WRITEPKT;

typedef struct MIDSTRUCT {
     UWORD  Infolevel;      /* (call) info level (should be 0000h) */
     ULONG  Serialnum;      /* disk serial number */
     BYTE   Vollabel[11];   /* volume label */
     BYTE   Filesys[8];     /* filesystem type */
} MIDSTRUCT;

MLOCAL DISKPARM oldparmblk;
MLOCAL DISKPARM newparmblk;
MLOCAL FMTPARM  fmtparmblk;
MLOCAL WRITEPKT diskio;
MLOCAL MIDSTRUCT media_id = {0, 0L, "           ", "FAT12   "};
MLOCAL ULONG    fat_dtime;
MLOCAL ULONG    serialnum;
MLOCAL BYTE disklabel[15];

extern BYTE * __based( __segname( "Bsect" ) ) Bootsector;
extern BYTE * __based( __segname( "Bsect" ) ) Bootbpb;
extern ULONG  __based( __segname( "Bsect" ) ) Bootserial;
extern BYTE * __based( __segname( "Bsect" ) ) Bootlabel;

MLOCAL DISKBPB bpb_table[] = {
    {512, 2, 1, 2, 112, 40*9*2,  0xfd, 2, 9,  2, 0, 0},   /* 360K format */
    {512, 1, 1, 2, 224, 80*15*2, 0xf9, 7, 15, 2, 0, 0},   /* 1.2M format */
    {512, 2, 1, 2, 112, 80*9*2,  0xf9, 3, 9,  2, 0, 0},   /* 720K format */
    {512, 1, 1, 2, 224, 80*18*2, 0xf0, 9, 18, 2, 0, 0}    /* 1.44M format */
};


/**************************************************************
 * Get all of the Floppy Disk Drives accessible by DOS. 
 * Version: 1.00b
 * Author: Ben Lunt (Forever Young Software)
 * Date: 09 Dec 1998
 * URL: http://www.fysnet.net/getdrvs.htm 
 **************************************************************/
__declspec( naked ) WORD get_drives() 
{
    _asm{
           xor  ax,ax                   // bits set represent drvs available
           mov  dx,0x0040               // low memory area we want to use
           mov  es,dx                   //
           mov  bx,0x0010               // we want word at 0040:0010h
           mov  dx,es:[bx]              //
           shr  dl,0x01                 // Bit 0 of dx: 0- no floppies
           jnc  short NoFlops           //              1- floppies exist
           mov  cl,0x05
           shr  dl,cl                   // bits 7 and 6 show how many
           jz   short OneDrv            //  0- one drive, 1- two drives
           or   ax,0x0003               //  2- three drives, 3- four drives
           jmp  short NoFlops           //
OneDrv:    mov  dx,0x0050               // let us see which drive is active
           mov  es,dx                   //  a: or b: (which is currently
           mov  bx,0x0004               //  the default drive letter)
           mov  dl,es:[bx]              // 0- Acting as drive A:
           or   dl,dl                   // 1- Acting as drive B:
           je   short DrvAasA           //
           or   ax,0x0002               // set accordingly
           jmp  short NoFlops           //  .
DrvAasA:   or   ax,0x0001               //  .
NoFlops:   
#ifdef  __SMALL__
           ret
#else
           retf
#endif
    };
}
#pragma aux get_drives value [ax] modify exact [ax bx dx cx es] nomemory;

/* Get drive parameters by calling BIOS INT 13,8 
 * drive number 0=A:, 1=B: 
 * return CMOS drive type:
 *       01 - 5.25"  360K       03 - 3.5"  720K
 *       02 - 5.25"  1.2Mb      04 - 3.5" 1.44Mb
 */
__declspec( naked ) WORD get_floppy_type(WORD drive) 
{
    _asm{
        pushf               // save flags
        mov dx, ax          // DL = drive number
        mov di, 0x0         // ES:DI = 0000h:0000h to guard against BIOS bugs
        mov es, di          
        mov ax, 0x0800      // select function 8
        int 0x13            // BIOS INT 13,8 is Get Current Drive Parameters
        jc  error           // CF = 1 if error
        mov ax, bx          // drive type is in BL
        xor ah, ah          // reset high byte of AX
        jmp endfun         
error:  xor ax, ax          // Return 0 if ERROR
endfun: popf                // restore flags and return
#ifdef  __SMALL__
        ret
#else
        retf
#endif
    };
}
#pragma aux get_floppy_type parm [ax] value [ax] modify exact [ax bx cx dx es di] nomemory

/* Call IOCTL Generic Block Device Request */
__declspec( naked ) WORD do_ioctl(WORD drive, WORD specfunc, WORD mincode, LPBYTE parblock)
{
    _asm {
        pushf               /* Save flags */
        push ds             /* Save DS since it may change */
        push es
        pop ds              /* set DS to parblock segment */
        mov byte ptr [bx], dl  /* First byte of parblock is special function */
        mov dx, bx          /* DS:DX points to the parameter block (small memory model) */
        mov bx, ax          /* BX is drive number */
        inc bx              /* increment BX, since IOCTL expects 00h=default,01h=A:,etc */
        mov ax, 0x440D      /* IOCTL - Generic block I/O */
        mov ch, 0x08        /* Disk drive category code - CL is minor code */
        int 0x21
        jc terminate        /* if carry is set, AX contains error code */
        xor ax, ax          /* clear ax if no error */
terminate:
        pop ds              /* restore DS */
        popf                /* restore flags */
#ifdef  __SMALL__
        ret
#else
        retf
#endif
    }
}
#pragma aux do_ioctl parm [ax] [dx] [cx] [es bx] value [ax] modify exact [ax bx cx dx] nomemory;


/* Absolute write sectors to drive */
__declspec( naked ) WORD write_sectors(WORD drive)
{
    _asm{
        push    ds                   /* save DS since it may be changed */
        mov     bx, seg diskio
        mov     ds, bx
        mov     bx, offset diskio   /* DS:DX points to the disk write packet */
        /* drive is already in AX */
        mov     cx,-1
        int     0x26
        jc terminate                /* if carry is set, AX contains error code */
        xor ax, ax                  /* clear ax if no error */
terminate:
        popf                        /* Get rid of flags */
        pop     ds                  /* restore DS */
#ifdef  __SMALL__
        ret
#else
        retf
#endif
    };
}
#pragma aux write_sectors parm [ax] value [ax] modify exact [ax bx cx] nomemory;

/* Get current date/time in FAT 32 bit encoding */
__declspec( naked ) VOID get_fatdtime()
{
    _asm{
        push    ds          /* save DS since it may change */
        mov     ax, seg serialnum   
        mov     ds, ax      /* set DS to the correct segment */
        mov     ah, 0x2C    /* INT 21, 2C is Get system Time */ 
        int     0x21        /* Return:  CH = hour
                             *          CL = minute
                             *          DH = second 
                             *          DL = 1/100 sec */
        mov     word ptr serialnum, cx
        mov     word ptr serialnum+2, dx
        xor     bx, bx      /* BX will get FAT time: 
                             * Bits:    15-11   Hours (0–23)
                             *          10-5    Minutes (0–59)
                             *          4-0     Seconds/2 (0–29) */
        shr     dh, 1       /* DH = seconds/2 */
        mov     bl, dh      /* Bits 4-0 are seconds/2 */
        xchg    ax, cx      /* hour and minutes are in AX */
        shl     al, 1
        shl     al, 1       /* bits 7-2 of AX: minute */
        mov     cl, 3
        shl     ax, cl      /* AX: bits 15-11 hours, 10-5 minute, 4-0 zero */
        or      bx, ax      /* BX is FAT time */
        mov     word ptr fat_dtime, bx /* Save FAT time */        
        mov     ah, 0x2A    /* INT 21, 2A is Get system Date */ 
        int     0x21        /* Return:  CX = year (1980-2099)
                             *          DH = month
                             *          DL = day */
        add     word ptr serialnum, cx
        add     word ptr serialnum+2, dx
        xor     bx, bx      /* BX will get FAT date: 
                             * Bits:    15-9    Year (0 = 1980, 119 = 2099)
                             *          8-5     Month (1–12)
                             *          4-0     Day (1–31) */
        sub     cx, 1980    /* Normalize year */
        shl     cx, 1       
        or      bh, cl      /* Bits 15-9 of BX are Year */
        mov     bl, dl      /* Bits 4-0 of BX are day */
        xchg    dl, dh      /* Bits 7-0 of DX are month */
        xor     dh, dh      /* reset bits 15-8 of DX */
        mov     cl, 5
        shl     dx, cl      /* bits 8-5 of DX are month */
        or      dx, bx      /* DX is FAT date */
        mov     word ptr fat_dtime+2, dx /* Save FAT date */  
        pop     ds          /* restore DS */
#ifdef  __SMALL__
        ret
#else
        retf
#endif
    }
}
#pragma aux get_fatdtime modify exact [ax bx cx dx] nomemory;


WORD set_floppy_parms(WORD drive, WORD formattype, DISKPARM *parblock)
{
    WORD i, rc = 0;
    
    if(parblock->Devicetype < 3 || parblock->Devicetype == 7)
    {
        if(formattype == 0)
        {
            /* if format type is 360K, set cylinders to 40 and mediatype to 1 */
            parblock->Cylinders = 40;
            parblock->Mediatype = 1;
        } else {
            /* all other types have 80 cylinders and mediatype 0 */
            parblock->Cylinders = 80;
            parblock->Mediatype = 0;
        }
        /* set up BPB for selected format type */
        memcpy(&parblock->Devicebpb, &bpb_table[formattype], 25);
        /* copy BPB to bootsector */
        _fmemcpy(&Bootbpb, &bpb_table[formattype], 25); 
        get_fatdtime();
        Bootserial = serialnum;
        /* Set Boot sector disk label */
        _fmemcpy(&Bootlabel, disklabel, 11);
        parblock->Numcyls = parblock->Devicebpb.Secttrack;
        for(i = 0; i < parblock->Numcyls; i++ )
        {
            /* Sector number */
            parblock->Tracklayout[2*i] = i + 1;
            /* Bytes per sector */
            parblock->Tracklayout[2*i + 1] = parblock->Devicebpb.Bytessect;
        }
        
        /* Special function code 5, mincode 0x40 is Set device parameters */ 
        rc = do_ioctl(drive, 05, 0x40, (LPBYTE)parblock);
    } else {
        rc = 1;   
    }

    return rc;
}

/*
 * adjust the formatting options, disabling options not available for `drive`
 */
VOID set_format_options(LPTREE tree, WORD drive)
{
    WORD drivetype, i, last;

    drivetype = get_floppy_type(drive);
    if (drivetype == 0) {
        /* Unknown drive type, disable everything */
        tree[FMT_5DD].ob_state |= DISABLED;
        tree[FMT_5DD].ob_state &= ~SELECTED;
        tree[FMT_5HD].ob_state |= DISABLED;
        tree[FMT_5HD].ob_state &= ~SELECTED;
        tree[FMT_3DD].ob_state |= DISABLED;
        tree[FMT_3DD].ob_state &= ~SELECTED;
        tree[FMT_3HD].ob_state |= DISABLED;    
        tree[FMT_3HD].ob_state &= ~SELECTED;
        tree[FMT_OK].ob_state |= DISABLED;
        return;    
    }
    /* Enable every option */
    tree[FMT_5DD].ob_state &= ~DISABLED;
    tree[FMT_5HD].ob_state &= ~DISABLED;
    tree[FMT_3DD].ob_state &= ~DISABLED;
    tree[FMT_3HD].ob_state &= ~DISABLED;    
    tree[FMT_OK].ob_state  &= ~DISABLED;
    switch(drivetype)
    {
        case 1:     /* 5.25" 360K drive */
            /* Disable 5.25" HD option */
            tree[FMT_5HD].ob_state |= DISABLED;
            tree[FMT_5HD].ob_state &= ~SELECTED;
            /* fall through next case */
        case 2:     /* 5.25" 1.2M drive */
            /* Disable 3.5" options */
            tree[FMT_3DD].ob_state |= DISABLED;
            tree[FMT_3DD].ob_state &= ~SELECTED;
            tree[FMT_3HD].ob_state |= DISABLED;
            tree[FMT_3HD].ob_state &= ~SELECTED;
            break;
        case 3:     /* 3.5" 720K drive */
            /* Disable 3.5" HD option */
            tree[FMT_3HD].ob_state |= DISABLED;
            tree[FMT_3HD].ob_state &= ~SELECTED;
            /* fall through next case */
        default:    /* 3.5" 1.44M drive */
            /* Disable 5.25" options */
            tree[FMT_5DD].ob_state |= DISABLED;
            tree[FMT_5DD].ob_state &= ~SELECTED;
            tree[FMT_5HD].ob_state |= DISABLED;
            tree[FMT_5HD].ob_state &= ~SELECTED;
            break;
    }
    /*
     * if NO option is selected, select the last enabled one
     */
    last = FMT_5DD;
    for (i = FMT_5DD; i <= FMT_3HD; i++)
    {
        if(tree[i].ob_state & SELECTED)
        {
            last = i;
            break;
        }
        if(!(tree[i].ob_state & DISABLED))
        {
            last = i;
        }        
    }
    tree[last].ob_state |= SELECTED;
}

/*
 *  Initialise starting sectors of floppy disk (boot sector and FATs)
 */
WORD init_start(WORD drive)
{
    WORD i, buffsize, rc = 0;
    LPBYTE buff;
    /* Fill the diskio structure */
    diskio.Sectbegin = 0;
    diskio.Sectcount = 1;
    diskio.Buffstart = &Bootsector;
    
    /* Write the boot sector */
    if((rc = write_sectors(drive)))
        return rc;
    /* Set the media ID */
    /* mincode 0x46 is Set media ID */
    media_id.Serialnum = serialnum;
    _fmemcpy(media_id.Vollabel, disklabel, 11);
    rc = do_ioctl(drive, 0x0, 0x46, (LPBYTE)&media_id);
    /* Write the FAT */
    /* Allocate memory for a copy of the FAT */
    buffsize = newparmblk.Devicebpb.Sectfat * newparmblk.Devicebpb.Bytessect;
    buff = dos_alloc(buffsize);
    /* Clear memory area */
    _fmemset(buff, 0, buffsize);
    /* store FAT ID followed by 0xFFFF filler */
    buff[0] = newparmblk.Devicebpb.Mediaid;
    buff[1] = 0xFF;
    buff[2] = 0xFF;
    /* Fill the diskio structure */
    diskio.Sectbegin = 1;
    diskio.Sectcount = newparmblk.Devicebpb.Sectfat;
    diskio.Buffstart = buff;
    /* write FATs to disk */
    for(i = 0; i < newparmblk.Devicebpb.Numfats; i++)
    {
        if((rc = write_sectors(drive)))
        {
            break;              /* rc will still be set */
        }
        /* Advance Sectbegin to write next FAT */
        diskio.Sectbegin += newparmblk.Devicebpb.Sectfat;        
    }
    /* Free memory and return */
    dos_free(buff);
    return rc;
}

/*
 *  Initialise the root dir and set volume label
 */
WORD write_dir(WORD drive, BYTE *label)
{
    WORD buffsize, i, rc = 0;
    LPBYTE buff;
    ULONG FAR *dtimeptr;
    
    /* Write the Directory */
    /* Allocate memory for the directory. Its size can be calculated multiplying
     * the max number of directory entries by 32 (bytes per directory entry). */
    buffsize = newparmblk.Devicebpb.Rootdir * 32;
    buff = dos_alloc(buffsize);
    /* Clear memory area */
    _fmemset(buff, 0, buffsize);
    /* Set disk label */
    /* first 11 bytes of dir entry is the disk label */
    _fmemcpy(buff, label, 11);
    buff[0x0B] = 0x28;                /* attribute byte is Archive | Volume Label */
    dtimeptr = (ULONG FAR *)&buff[0x16];
    *dtimeptr = fat_dtime;
    /* Fill the diskio structure */
    diskio.Sectbegin = newparmblk.Devicebpb.Numfats * newparmblk.Devicebpb.Sectfat + 1;
    diskio.Sectcount = buffsize / newparmblk.Devicebpb.Bytessect;
    diskio.Buffstart = buff;
    /* write the directory to disk */
    rc = write_sectors(drive);
    /* Free memory */
    dos_free(buff);    
    
    return rc;
}

/*
 *  Issue alert & return TRUE iff user wants to retry
 */
static BOOLEAN retry_format(void)
{
    graf_mouse(ARROW,NULL);
    if (fun_alert(3, STFMTERR) == 2)
        return FALSE;
    graf_mouse(HOURGLASS,NULL);    /* say we're busy again */

    return TRUE;
}

/*
 *  Do the real formatting work
 */
static WORD format_floppy(LPTREE tree, WORD drive, WORD max_width, WORD incr)
{
    WORD track, numtracks, side;
    WORD width, rc, i, formattype;

    /* special function 0 is Want for current bpb; mincode 0x60 is Get device parameters */
    rc = do_ioctl(drive, 00, 0x60, (BYTE *)&oldparmblk);
    if(rc != 0) 
    {
        fun_alert(1,STNODRIV, drive+'A');
        return rc;
    }
    
    /* copy parmblock to work area */
    memcpy(&newparmblk, &oldparmblk, sizeof(DISKPARM));
    
    formattype = inf_gindex(tree, FMT_5DD, 4);
    inf_sget(tree, FMTLABEL, disklabel, 12);
    /* find disklabel lenght */
    for(i = 0; (i < 11) && disklabel[i]; i++) {
        /* skip */
    }
    /* pad with spaces if label is shorter than 11 bytes */
    for(; i < 11; i++)
        disklabel[i] = ' ';
    rc = set_floppy_parms(drive, formattype, &newparmblk);
    if(rc != 0) 
    {
        fun_alert(1,STNODRIV, drive+'A');
        return rc;
    }

    tree[FMT_BAR].ob_width = 0;
    tree[FMT_BAR].ob_spec = (LPVOID)0x00FF1121L;
    numtracks = newparmblk.Cylinders;
    /* For 40 tracks format progress bar increment is doubled */
    if(numtracks == 40)
        incr *= 2;

    graf_mouse(HOURGLASS,NULL);    /* say we're busy */

    for (track = 0, rc = 0; (track < numtracks) && !rc ; track++)
    {
        for (side = 0; (side < 2) && !rc; side++)
        {
            fmtparmblk.Head = side;
            fmtparmblk.Cylinder = track;
            /* special function 0, mincode 0x42 is Format/verify track */
            while((rc = do_ioctl(drive, 00, 0x42, (BYTE *)&fmtparmblk)))
            {
                if (!retry_format())
                    break;              /* rc will still be set */
            }
        }
        /* update progress bar */
        width = tree[FMT_BAR].ob_width + incr;
        if (width > max_width)
            width = max_width;
        tree[FMT_BAR].ob_width = width;
        draw_fld(tree,FMT_BAR);
    }

    if (rc == 0)
    {
        while((rc=init_start(drive)))
        {
            if (!retry_format())
                break;                  /* rc will still be set */
        }
    }

    if (rc == 0)
    {
        while((rc=write_dir(drive, disklabel)))
        {
            if (!retry_format())
                break;                  /* rc will still be set */
        }
    }

    /* Restore parms to their old condition */
    /* Special function code 4, mincode 0x40 is Set device parameters */ 
    do_ioctl(drive, 0x04, 0x40, (BYTE *)&oldparmblk);
    
    graf_mouse(ARROW,NULL);     /* no longer busy */

    return rc;
}


/*
 * Function called after a format to redisplay any windows
 * associated with the specified drive letter
 */
void refresh_drive(WORD drvletter)
{
	WORD		ii;
	WORD		drv;
	BYTE		path[66], name[9], ext[4];
	WNODE		*pw;
	
	for(ii = 0; ii < NUM_WNODES; ii++)
	{
	  pw = &G.g_wlist[ii];
	  if (pw->w_id)
	  {
	    fpd_parse(&pw->w_path->p_spec[0], &drv, &path[0],
	    	      &name[0], &ext[0]);
	    if(drv == drvletter) {
	        path[0] = '\0';     /* return to root of drive */
	        do_fopen(pw, 0, drv, &path[0], &name[0], &ext[0], TRUE, TRUE);
	    }
	  }
	}
} /* refresh_drive */


/*
*	Format a floppy disk.
*/
	VOID 
do_format(curr)
	WORD		curr;
{
    LPTREE tree, obj;
    LONG total, avail;
    WORD i, drivebits, drive, drivetype;
    WORD exitobj, rc, junk;
    WORD max_width, incr;
    BOOLEAN done = FALSE;
    ANODE   *pa;
    FNODE   *pf;

    tree = G.a_trees[ADFORMAT];

    /*
     * enable button(s) for existent drives, disable for non-existent
     */
    drivebits = get_drives();  /* floppy devices */
    
    for (i = 0, obj = &tree[FMT_DRVA]; i < 2; i++, obj++, drivebits >>= 1)
    {
        if (drivebits & 0x0001)
        {
            obj->ob_state &= ~DISABLED;
            obj->ob_state &= ~SELECTED;
        }
        else
        {
            obj->ob_state &= ~SELECTED;
            obj->ob_state |= DISABLED;
        }
    }

    /*
     * If 'curr' is a floppy drive, select it
     */
    drive = -1;
    pa = i_find(G.g_cwin, curr, &pf, &junk);
    if ( (pa) && (pa->a_type == AT_ISDISK) )
    {
        // DESKTOP v2.x+ version pf->f_junk;
        drive = (get_spec(G.g_screen, curr)->ib_char) & 0xFF;
        drive = drive - 'A';
        /* If drive is A: or B: and enabled, select it */
        if(drive < 2 && !(tree[FMT_DRVA + drive].ob_state & DISABLED))
        {
            tree[FMT_DRVA + drive].ob_state |= SELECTED;
        } else {
            drive = -1;
        }
    }

    /*
     * if NO drive was previously selected, select the first enabled one
     */
    if (drive < 0)
    {
        for (i = 0, obj = &tree[FMT_DRVA]; i < 2; i++, obj++)
        {
            if (!(obj->ob_state & DISABLED))
            {
                drive = i;
                break;
            }
        }
        if (drive >= 0)
            obj->ob_state |= SELECTED;
    }
    
    /*
     * adjust the formatting options for current drive
     */
    set_format_options(tree, drive);
    tree[FMT_CNCL].ob_state &= ~SELECTED;
    
    /*
     * fix up the progress bar width, increment & fill pattern
     */
    incr = tree[FMT_BAR].ob_width / MAXTRACK;
    max_width = incr * MAXTRACK;
    tree[FMT_BAR].ob_width = max_width;
    tree[FMT_BAR].ob_spec = (LPVOID) 0x00FF1101L;

    /*
     * do the actual work
     */
    inf_sset(tree, FMTLABEL, "");
    inf_start(tree);
    do {    
        exitobj = form_do(tree, FMTLABEL) & 0x7fff;
        switch(exitobj)
        {
            case FMT_DRVA:
            case FMT_DRVB:
                drive = exitobj - FMT_DRVA;
                done = FALSE;
                /*
                 * Update and redraw formatting options
                 */
                set_format_options(tree, drive);
                draw_fld(tree, FMTBOX);
                break;
            case FMT_OK:
                /* format floppy */
                rc = format_floppy(tree, drive, max_width, incr);
                /* If no error, show info dialog */
                if (rc == 0)
                {
                    dos_space(drive + 1, &total, &avail);
                    if (fun_alert(2, STFMTINF, avail) == 2)
                        done = TRUE;
                }                
                refresh_drive('A' + drive);     /* update relevant windows */
                /* reset to starting values and redraw the entire dialog */
                tree[FMT_BAR].ob_width = max_width;     
                tree[FMT_BAR].ob_spec = (LPVOID)0x00FF1101L;
                tree[FMT_OK].ob_state &= ~SELECTED;
                inf_start(tree);
                break;
            default:
                done = TRUE;
        }
    } while(!done);
    
    inf_end(tree);
} /* do_format */


/*
 *	EOF:	deskfrmt.c
 */
