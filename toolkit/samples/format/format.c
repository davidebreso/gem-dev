/***********************************************************************
*
*  GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*  Modulename: USERDEF.C
*  Date:       07-27-89
*  Author:     Robert Schneider, DR GmbH
*
***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "wccgem.h"
#include "format.h"

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

MLOCAL WORD     work_in[11];
MLOCAL WORD     work_out[57];

MLOCAL WORD     ap_id;
MLOCAL WORD     vdi_handle;
MLOCAL LPTREE   tree;
MLOCAL WCCUBLK  cross;
MLOCAL DISKPARM oldparmblk;
MLOCAL DISKPARM newparmblk;
MLOCAL FMTPARM  fmtparmblk;
MLOCAL WRITEPKT diskio;
MLOCAL MIDSTRUCT media_id = {0, 0L, "           ", "FAT12   "};
MLOCAL ULONG    fat_dtime;
MLOCAL ULONG    serialnum;
MLOCAL BYTE disklabel[15];

MLOCAL LPBYTE a_alert;
MLOCAL BYTE g_1text[256];
MLOCAL BYTE g_2text[256];

extern BYTE * __based( __segname( "Bsect" ) ) Bootsector;
extern BYTE * __based( __segname( "Bsect" ) ) Bootbpb;
extern ULONG  __based( __segname( "Bsect" ) ) Bootserial;
extern BYTE * __based( __segname( "Bsect" ) ) Bootlabel;

MLOCAL UWORD numcyl_table[] = {40, 80, 80, 80};

MLOCAL DISKBPB bpb_table[] = {
    {512, 2, 1, 2, 112, 40*9*2,  0xfd, 2, 9,  2, 0, 0},   /* 360K format */
    {512, 1, 1, 2, 224, 80*15*2, 0xf9, 7, 15, 2, 0, 0},   /* 1.2M format */
    {512, 2, 1, 2, 112, 80*9*2,  0xf9, 3, 9,  2, 0, 0},   /* 720K format */
    {512, 1, 1, 2, 224, 80*18*2, 0xf0, 9, 18, 2, 0, 0}    /* 1.44M format */
};

/*
 *      declarations used by the do_format() code
 */
#define MAXTRACK        80

// FILE *logfile;

VOID  inf_sset(LPTREE tree, WORD obj, BYTE *pstr)
{
	LPTEDI spec;
	
	spec = (LPTEDI)tree[obj].ob_spec;
    _fstrcpy(spec->te_ptext, pstr);
}

VOID  inf_sget(LPTREE tree, WORD obj, BYTE *pstr, WORD maxlen)
{
	LPTEDI spec;
	
	spec = (LPTEDI)tree[obj].ob_spec;
    _fstrncpy(pstr, spec->te_ptext, maxlen);
}

/*
*	Draw a single field of a dialog box
*/
VOID  draw_fld(LPTREE tree, WORD obj)
{
	GRECT		t;

	LWCOPY((LPWORD)ADDR(&t), (LPWORD)(&tree[obj].ob_x), 4);
	objc_offset(tree, obj, &t.g_x, &t.g_y);
	objc_draw(tree, obj, MAX_DEPTH, t.g_x, t.g_y, t.g_w, t.g_h);
} /* draw_fld */

VOID gem_init()
{
    static WORD i;
    static WORD dummy;

    ap_id = appl_init(NULL);

    vdi_handle = graf_handle(&dummy, &dummy, &dummy, &dummy);

    for (i=0; i<10; work_in[i++] = 1)
       ;
    work_in[10] = 2;
    v_opnvwk(work_in, &vdi_handle, work_out);
}

VOID gem_exit()
{
    v_clsvwk(vdi_handle);
    appl_exit();
}

WORD  inf_gindex(LPTREE tree, WORD baseobj, WORD numobj)
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
*	Routine to transfer a string that requires integrated variables
*	that are merged in.  The resultant alert is then displayed;
*/
WORD  fun_alert(WORD defbut, WORD stnum, ...)
{
	va_list ap;
	va_start(ap, stnum);

	rsrc_gaddr(R_STRING, stnum, (LPVOID *)&a_alert);
    _fstrncpy(g_2text, a_alert, 256);
    vsprintf(g_1text, g_2text, ap);                    

	va_end(ap);
	return( form_alert(defbut, g_1text) );
}



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
    
    // fprintf(logfile,"Devicetype: %d\n", parblock->Devicetype);
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
        // fprintf(logfile, "Copy BPB to bootsector at %lX\n", (LPVOID)&Bootbpb);
        _fmemcpy(&Bootbpb, &bpb_table[formattype], 25); 
        get_fatdtime();
        // fprintf(logfile, "Serial number is %lX\n", serialnum);
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
    for(i = 0; i < sizeof(DISKPARM); i++) 
    {
        // fprintf(logfile, "%02X ", ((BYTE *)parblock)[i]);
    }
    // fprintf(logfile, "\n");
    
    // fprintf(logfile, "set_floppy_parms returns %d\n", rc);
    return rc;
}

/*
 * adjust the formatting options, disabling options not available for `drive`
 */
VOID set_format_options(WORD drive)
{
    WORD drivetype, i, last;

    drivetype = get_floppy_type(drive);
    // fprintf(logfile,"drive type %d\n", drivetype);
    if (drivetype == 0) {
      fun_alert(1,STNODRIV, drive+'A');
      gem_exit();
      return;    
    }
    /* Enable every option */
    tree[FMT_5DD].ob_state &= ~DISABLED;
    tree[FMT_5HD].ob_state &= ~DISABLED;
    tree[FMT_3DD].ob_state &= ~DISABLED;
    tree[FMT_3HD].ob_state &= ~DISABLED;    
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
            // fprintf(logfile, "Option %d is currently selected.\n", i);
            last = i;
            break;
        }
        if(!(tree[i].ob_state & DISABLED))
        {
            // fprintf(logfile, "Option %d is enabled.\n", i);
            last = i;
        }        
    }
    // fprintf(logfile,"last option is %d, select it\n", last);
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
    
    // fprintf(logfile, "Writing boot sector at %d from %lX\n", diskio.Sectcount, diskio.Buffstart);
    /* Write the boot sector */
    if((rc = write_sectors(drive)))
        return rc;
    /* Set the media ID */
    /* mincode 0x46 is Set media ID */
    // fprintf(logfile, "Setting media ID\n");
    media_id.Serialnum = serialnum;
    _fmemcpy(media_id.Vollabel, disklabel, 11);
    rc = do_ioctl(drive, 0x0, 0x46, (LPBYTE)&media_id);
    // fprintf(logfile, "return value %d\n", rc);
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
        // fprintf(logfile,"Writing FAT %d at sector %ld\n",i+1, diskio.Sectbegin);
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
    (ULONG)(buff[0x16]) = fat_dtime;
    // fprintf(logfile,"Label time %04X date %04X\n", buff[0x16], buff[0x18]);
    /* Fill the diskio structure */
    diskio.Sectbegin = newparmblk.Devicebpb.Numfats * newparmblk.Devicebpb.Sectfat + 1;
    diskio.Sectcount = buffsize / newparmblk.Devicebpb.Bytessect;
    diskio.Buffstart = buff;
    /* write the directory to disk */
    // fprintf(logfile,"Writing directory at sector %ld\n", diskio.Sectbegin);
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
    // fprintf(logfile, "get_floppy_parms returned %d\n", rc);
    if(rc != 0) 
    {
        fun_alert(1,STNODRIV, drive+'A');
        return rc;
    }
    
    /* copy parmblock to work area */
    memcpy(&newparmblk, &oldparmblk, sizeof(DISKPARM));
    
    formattype = inf_gindex(tree, FMT_5DD, 4);
    inf_sget(tree, FMTLABEL, disklabel, 12);
    // fprintf(logfile, "Disklabel is %s\n", disklabel);
    /* find disklabel lenght */
    for(i = 0; (i < 11) && disklabel[i]; i++) {
        /* skip */
    }
    /* pad with spaces if label is shorter than 11 bytes */
    for(; i < 11; i++)
        disklabel[i] = ' ';
    rc = set_floppy_parms(drive, formattype, &newparmblk);
    // fprintf(logfile, "set_floppy_parms(%d, %d, newparmblk) returned %d\n", drive, formattype, rc);
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
            // fprintf(logfile, "Format side %d track %d\n", side, track);
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
            // fprintf(logfile, "Error %d in init_start\n", rc);
            if (!retry_format())
                break;                  /* rc will still be set */
        }
    }

    if (rc == 0)
    {
        while((rc=write_dir(drive, disklabel)))
        {
            // fprintf(logfile, "Error %d in write_dir\n", rc);
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

VOID main()
{
    LPTREE obj;
    LONG total, avail;
    WORD i, drivebits, drive, drivetype;
    WORD exitobj, rc;
    WORD max_width, incr;
    BOOLEAN done = FALSE;

    gem_init();

    if (!rsrc_load("FORMAT.RSC"))
    {
      form_alert(1,"[3][Fatal Error !|FORMAT.RSC|File Not Found !][ Ok ]");
      gem_exit();
      return;
    }

    // logfile = fopen("format.log","w");
    // fprintf(logfile, "Starting FORMAT.APP\n");
    // fprintf(logfile, "sizeof(DISKPARM)=%d, sizeof(DISKBPB)=%d\n",sizeof(DISKPARM), sizeof(DISKBPB));
    rsrc_gaddr(R_TREE, ADFORMAT, (LPVOID *)&tree);

    /*
     * enable button(s) for existent drives, disable for non-existent
     */
    drivebits = get_drives();  /* floppy devices */
    // fprintf(logfile, "drivebits=%X\n", drivebits);
    
    for (i = 0, obj = &tree[FMT_DRVA]; i < 2; i++, obj++, drivebits >>= 1)
    {
        if (drivebits & 0x0001)
        {
            // fprintf(logfile, "Enable drive %c\n", i+'A');
            obj->ob_state &= ~DISABLED;
        }
        else
        {
            // fprintf(logfile, "Disable drive %c\n", i+'A');
            obj->ob_state &= ~SELECTED;
            obj->ob_state |= DISABLED;
        }
    }

    /*
     * if a drive is currently selected, don't change it
     */
    drive = -1;
    for (i = 0, obj = &tree[FMT_DRVA]; i < 2; i++, obj++)
    {
        if (obj->ob_state & SELECTED)
        {
            // fprintf(logfile, "Drive %c is currently selected.\n", i+'A');
            drive = i;
            break;
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
                // fprintf(logfile, "Select drive %c.\n", i+'A');
                drive = i;
                break;
            }
        }
        if (drive >= 0)
            obj->ob_state |= SELECTED;
    }
    
    // fprintf(logfile, "drive=%d\n", drive);

    /*
     * if there are no enabled drives, disallow OK
     */
    if (drive < 0) 
    {
        tree[FMT_OK].ob_state |= DISABLED;
    } else {
        /*
         * adjust the formatting options for current drive
         */
        set_format_options(drive);
    }
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
    ob_draw_dialog(tree, 0, 0, 0, 0);
    do {    
        exitobj = form_do(tree, FMTLABEL) & 0x7fff;
        switch(exitobj)
        {
            case FMT_DRVA:
            case FMT_DRVB:
                drive = exitobj - FMT_DRVA;
                // fprintf(logfile, "New drive is %d\n", drive);
                done = FALSE;
                /*
                 * Update and redraw formatting options
                 */
                set_format_options(drive);
                draw_fld(tree, FMTBOX);
                break;
            case FMT_OK:
                /* format floppy */
                rc = format_floppy(tree, drive, max_width, incr);
                /* If no error, show info dialog */
                if (rc == 0)
                {
                    dos_space(drive + 1, &total, &avail);
                    // fprintf(logfile, "total: %ld, avail: %ld\n", total, avail);
                    if (fun_alert(2, STFMTINF, avail) == 2)
                        done = TRUE;
                }                
                /* reset to starting values */
                tree[FMT_BAR].ob_width = max_width;     
                tree[FMT_BAR].ob_spec = (LPVOID)0x00FF1101L;
                tree[FMT_OK].ob_state &= ~SELECTED;
                draw_fld(tree, FMT_BAR);
                draw_fld(tree, FMT_OK);
                break;
            default:
                done = TRUE;
        }
    } while(!done);
    
    ob_undraw_dialog(tree, 0, 0, 0, 0);
    inf_sget(tree, FMTLABEL, disklabel, 15);
    // fprintf(logfile, "form_do terminated with exit code %X and label %s\n", exitobj, disklabel);
    // fprintf(logfile, "Closing FORMAT.APP\n");
    // fclose(logfile);
    
    gem_exit();
}

