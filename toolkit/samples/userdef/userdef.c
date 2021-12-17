/***********************************************************************
*
*  GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*  Modulename: USERDEF.C
*  Date:       07-27-89
*  Author:     Robert Schneider, DR GmbH
*
***********************************************************************/

#include <portab.h>
#include <stdio.h>
#include <aes.h>
#include <vdi.h>
#include <oblib.h>
#include <i86.h>
#include "userdef.h"

WORD objc_cross(WORD seg, WORD offs);
WORD __declspec( naked ) far_draw(WORD seg, WORD ofs);

MLOCAL WORD       work_in[11];
MLOCAL WORD       work_out[57];

MLOCAL WORD       ap_id;
MLOCAL WORD       vdi_handle;
MLOCAL OBJECT FAR *baum;
MLOCAL APPLBLK    cross;
// MLOCAL PPDUBLK    cross;

LPPARM fardr_start();
VOID fardr_end(WORD retcode);

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
   WORD a;

   gem_init();

   if (!rsrc_load("USERDEF.RSC"))
   {
      form_alert(1,"[3][Fatal Error !|USERDEF.RSC|File Not Found !][ Ok ]");
      gem_exit();
      return;
   }
   rsrc_gaddr(R_TREE, TEST, (LPVOID *)&baum);
/*
#if MS_C && M_I86SM
   cross.ab_code = MKFP(fardr_cs(), objc_cross);
#else
#if HIGH_C
   cross.ab_code = (FAR WORD (*)())objc_cross;
#else
   cross.ab_code = (WORD (FAR *)())objc_cross;
#endif
#endif
*/
   _asm{ int 3 };
   cross.ab_code = far_draw;
   cross.ab_parm = 0l;

   for (a = RB1; a <= RB4; a++)
   {
      // _asm { int 3 };
      // ppd_userdef(baum, a, &cross);
      baum[a].ob_type = G_USERDEF;
      baum[a].ob_spec = &cross;
   }

   for (a = SB1; a <= SB4; a++)
   {
      // ppd_userdef(baum, a, &cross);
      baum[a].ob_type = G_USERDEF;
      baum[a].ob_spec = &cross;
   }

   ob_draw_dialog(baum, 0, 0, 0, 0);
   form_do(baum, 0);
   ob_undraw_dialog(baum, 0, 0, 0, 0);

   gem_exit();
}

/******
#if MSDOS
MLOCAL WORD objc_cross()
{
   LPPARM pb;
   // PARMBLK FAR *fardr_start();
   WORD xy[10];

   _asm{ int 3 };  
   pb = fardr_start();
#endif

#if FLEXOS
MLOCAL WORD objc_cross(pb)
PARMBLK *pb;
{
   WORD xy[10];
#endif
*****/
LPPARM pb;
BYTE newstack[1024];
WORD save_ss, save_sp;

WORD __declspec( naked ) far_draw(WORD seg, WORD ofs)
{
    _asm{
        int 3
        push ds
        push cx
        mov cx, seg pb
        mov ds, cx
        mov save_ss, ss
        mov save_sp, sp
        mov ss, cx
        mov sp, offset newstack + 1022
        mov word ptr pb, bx
        mov word ptr pb+2, ax
#ifdef __LARGE__        
        callf objc_cross
        mov cx, seg pb
        mov ds, cx
#else
        call objc_cross
#endif
        mov ss, save_ss
        mov sp, save_sp
        pop cx
        pop ds
        retf
    };
}
#pragma aux far_draw parm [ax] [bx] value [ax];


WORD objc_cross()
{
   WORD xy[10];
     
   // pb = MK_FP(seg, ofs);

   vsf_color(vdi_handle, WHITE);

   if (pb->pb_prevstate & SELECTED)
   {
      xy[0] = pb->pb_x;
      xy[1] = pb->pb_y;
      xy[2] = pb->pb_x + pb->pb_w - 1;
      xy[3] = pb->pb_y + pb->pb_h - 1;
      vr_recfl(vdi_handle, xy);
   }

   xy[0] = xy[6] = xy[8] = pb->pb_x;
   xy[1] = xy[3] = xy[9] = pb->pb_y;
   xy[2] = xy[4] = pb->pb_x + pb->pb_w - 1;
   xy[5] = xy[7] = pb->pb_y + pb->pb_h - 1;
   v_pline(vdi_handle, 5, xy);

   if (pb->pb_currstate & SELECTED)
   {
      xy[3] = xy[5];
      v_pline(vdi_handle, 2, xy);
      xy[0] = xy[2];
      xy[2] = pb->pb_x + 1;
      v_pline(vdi_handle, 2, xy);
   }

   return pb->pb_currstate & (~SELECTED);
/*****
#if MSDOS
   fardr_end(pb->pb_currstate & (~SELECTED));
#endif
******/
}
#pragma aux objc_cross value [ax];

