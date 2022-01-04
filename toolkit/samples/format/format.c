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
#include "wccgem.h"
#include "format.h"

MLOCAL WORD     work_in[11];
MLOCAL WORD     work_out[57];

MLOCAL WORD     ap_id;
MLOCAL WORD     vdi_handle;
MLOCAL LPTREE   tree;
MLOCAL WCCUBLK  cross;

/*
 *      declarations used by the do_format() code
 */
#define MAXTRACK        80


FILE *logfile;

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
NoFlops:   ret                          //
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
        ret
    };
}
#pragma aux get_floppy_type parm [ax] value [ax] modify exact [ax bx cx dx es di] nomemory

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

VOID main()
{
    LPTREE obj;
    LONG total, avail;
    WORD i, drivebits, drive, drivetype;
    WORD exitobj, rc;
    WORD max_width, incr;
    BOOLEAN done = FALSE;
    BYTE disklabel[15];

    gem_init();

    if (!rsrc_load("FORMAT.RSC"))
    {
      form_alert(1,"[3][Fatal Error !|FORMAT.RSC|File Not Found !][ Ok ]");
      gem_exit();
      return;
    }

    logfile = fopen("format.log","w");
    fprintf(logfile, "Starting FORMAT.APP\n");
    
    rsrc_gaddr(R_TREE, ADFORMAT, (LPVOID *)&tree);

    /*
     * enable button(s) for existent drives, disable for non-existent
     */
    drivebits = get_drives();  /* floppy devices */
    fprintf(logfile, "drivebits=%X\n", drivebits);
    
    for (i = 0, obj = &tree[FMT_DRVA]; i < 2; i++, obj++, drivebits >>= 1)
    {
        if (drivebits & 0x0001)
        {
            fprintf(logfile, "Enable drive %c\n", i+'A');
            obj->ob_state &= ~DISABLED;
        }
        else
        {
            fprintf(logfile, "Disable drive %c\n", i+'A');
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
            fprintf(logfile, "Drive %c is currently selected.\n", i+'A');
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
                fprintf(logfile, "Select drive %c.\n", i+'A');
                drive = i;
                break;
            }
        }
        if (drive >= 0)
            obj->ob_state |= SELECTED;
    }
    
    fprintf(logfile, "drive=%d\n", drive);

    /*
     * if there are no enabled drives, disallow OK
     */
    if (drive < 0)
        tree[FMT_OK].ob_state |= DISABLED;

    tree[FMT_CNCL].ob_state &= ~SELECTED;

    /*
     * adjust the initial default formatting option, disabling
     * the high density option if not available
     */
    drivetype = get_floppy_type(drive);
    fprintf(logfile,"drive type %d\n", drivetype);
    if (drivetype == 0) {
      form_alert(1,"[3][Fatal Error !|Could not determine drive type!][ Ok ]");
      gem_exit();
      return;    
    }
    if (drivetype == 1 || drivetype == 3)   /* if drive type 360k or 720k */
    {
        if (tree[FMT_HD].ob_state & SELECTED)   /* first time */
        {
            tree[FMT_HD].ob_state &= ~SELECTED;
            tree[FMT_HD].ob_flags |= DISABLED;
            tree[FMT_DS].ob_state |= SELECTED;
        }
    }

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
    exitobj = form_do(tree, FMTLABEL) & 0x7fff;
    ob_undraw_dialog(tree, 0, 0, 0, 0);
    inf_sget(tree, FMTLABEL, disklabel, 15);
    fprintf(logfile, "form_do terminated with exit code %X and label %s\n", exitobj, disklabel);
    fprintf(logfile, "Closing FORMAT.APP\n");
    fclose(logfile);
    
    gem_exit();
}

