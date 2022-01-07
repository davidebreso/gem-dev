/************************************************************************/
/*   File:   RSCREATE.C                                                 */
/************************************************************************/
/*                                                                      */
/*           GGGGG        EEEEEEEE     MM      MM                       */
/*         GG             EE           MMMM  MMMM                       */
/*         GG   GGG       EEEEE        MM  MM  MM                       */
/*         GG   GG        EE           MM      MM                       */
/*           GGGGG        EEEEEEEE     MM      MM                       */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*           +--------------------------+                               */
/*           | Digital Research, Inc.   |                               */
/*           | 70 Garden Court          |                               */
/*           | Monterey, CA.     93940  |                               */
/*           +--------------------------+                               */
/*                                                                      */
/*   The  source code  contained in  this listing is a non-copyrighted  */
/*   work which  can be  freely used.  In applications of  this source  */
/*   code you  are requested to  acknowledge Digital Research, Inc. as  */
/*   the originator of this code.                                       */
/*                                                                      */
/*   Author:    LKW                                                     */
/*   PRODUCT:   GEM RSC-File Converter                                  */
/*   Module:    RSCREATE.C                                              */
/*   Version:   1.1                                                     */
/*   Created:   May 18, 1984                                            */
/*   Modified:  Robert Schneider, April 25, 1989                        */
/*              Cleaned up for use with ANSI-C COMPILERS                */
/************************************************************************/

#include "portab.h"

#if HIGH_C
pragma On (Pointers_compatible_with_ints);
#endif

#include "wccgem.h"
#include <dos.h>
#include <stdio.h>

MLOCAL WORD sub_pointer(VOID *p1, VOID *p2);
MLOCAL VOID *vsub_pointer(BYTE *p1, BYTE *p2);
VOID fix_trindex(VOID);
VOID fix_objects(VOID);
VOID fix_tedinfo(VOID);
VOID fix_frstr(VOID);
VOID fix_str( LONG FAR *where );
VOID fix_iconblk(VOID);
VOID fix_bitblk(VOID);
VOID fix_frimg(VOID);
VOID fix_bb(LONG FAR *where);
VOID fix_img(LONG FAR *where);

#pragma data_seg ( "CONST" )

RSHDR starthdr = 
{
   0,      /* rsh_vrsn    */
   0,      /* rsh_object  */
   0,      /* rsh_tedinfo */
   0,      /* rsh_iconblk */
   0,      /* rsh_bitblk  */
   0,      /* rsh_frstr   */
   0,      /* rsh_string   string data     */
   0,      /* rsh_imdata   image data      */
   0,      /* rsh_frimg   */
   0,      /* rsh_trindex */
   0,      /* rsh_nobs    */
   0,      /* rsh_ntree   */
   0,      /* rsh_nted    */
   0,      /* rsh_nib     */
   0,      /* rsh_nbb     */
   0,      /* rsh_nstring */
   0,      /* rsh_nimages */
   0       /* rsh_rssize  */
};



#include "example.rsh"

WORD  endfile = 0;

#pragma data_seg ( )


FILE *logfile;

main()   
{
    WORD /*jnk1,*/ handle;
    LONG cnt;                        /* in bytes   */

    _asm{ int 3 };
    starthdr.rsh_vrsn = 0;
    printf("starthdr.rsh_vrsn=%04X\n", starthdr.rsh_vrsn);
    
    starthdr.rsh_object = sub_pointer(rs_object,&starthdr);
    printf("starthdr.rsh_object=%04X\n", starthdr.rsh_object);
    starthdr.rsh_tedinfo = sub_pointer(rs_tedinfo,&starthdr);
    printf("starthdr.rsh_tedinfo=%04X\n", starthdr.rsh_tedinfo);

    starthdr.rsh_iconblk = sub_pointer(rs_iconblk,&starthdr);
    printf("starthdr.rsh_iconblk=%04X\n", starthdr.rsh_iconblk);

    starthdr.rsh_bitblk = sub_pointer(rs_bitblk,&starthdr);
    printf("starthdr.rsh_bitblk=%04X\n", starthdr.rsh_bitblk);

    starthdr.rsh_frstr = sub_pointer(rs_frstr,&starthdr);
    printf("starthdr.rsh_frstr=%04X\n", starthdr.rsh_frstr);

    starthdr.rsh_string = sub_pointer(rs_strings[0],&starthdr);
    printf("starthdr.rsh_string=%04X\n", starthdr.rsh_string);

    starthdr.rsh_imdata = sub_pointer(rs_imdope[0].image,&starthdr);
    printf("starthdr.rsh_imdata=%04X\n", starthdr.rsh_imdata);

    starthdr.rsh_frimg = sub_pointer(rs_frimg,&starthdr);
    printf("starthdr.rsh_frimg=%04X\n", starthdr.rsh_frimg);

    starthdr.rsh_trindex = sub_pointer(rs_trindex,&starthdr);
    printf("starthdr.rsh_trindex=%04X\n", starthdr.rsh_trindex);

    starthdr.rsh_nobs    = NUM_OBS;
    printf("starthdr.rsh_nobs=%04X\n", starthdr.rsh_nobs);
    starthdr.rsh_ntree   = NUM_TREE;
    printf("starthdr.rsh_ntree=%04X\n", starthdr.rsh_ntree);
    starthdr.rsh_nted    = NUM_TI;
    printf("starthdr.rsh_nted=%04X\n", starthdr.rsh_nted);
    starthdr.rsh_nib     = NUM_IB;
    printf("starthdr.rsh_nib=%04X\n", starthdr.rsh_nib);
    starthdr.rsh_nbb     = NUM_BB;
    printf("starthdr.rsh_nbb=%04X\n", starthdr.rsh_nbb);
    starthdr.rsh_nimages = NUM_FRIMG;
    printf("starthdr.rsh_nimages=%04X\n", starthdr.rsh_nimages);
    starthdr.rsh_nstring = NUM_FRSTR;
    printf("starthdr.rsh_nstring=%04X\n", starthdr.rsh_nstring);

//     fix_trindex();
//     fix_objects();
//     fix_tedinfo();
//     fix_iconblk();
//     fix_bitblk();
//     fix_frstr();
//     fix_frimg();

    handle = dos_create(pname, 0); 

    cnt = sub_pointer(&endfile,&starthdr);
    starthdr.rsh_rssize = (UWORD)cnt;
    dos_write(handle, cnt, (LPBYTE)(&starthdr)); 

    dos_close( handle );
}

/* 
VOID fix_trindex(VOID)
{
   WORD test, ii;

   for (ii = 0; ii < NUM_TREE; ii++)
   {
      test = (WORD) rs_trindex[ii];
      rs_trindex[ii] = (WORD) sub_pointer(&rs_object[test], &starthdr);
   }
}

VOID fix_objects(VOID)
{
   WORD   test, ii;

   for (ii = 0; ii < NUM_OBS; ii++)
   {
      test = (WORD) rs_object[ii].ob_spec;
      switch (rs_object[ii].ob_type & 0xff)
      {
         case G_TITLE:
         case G_STRING:
         case G_BUTTON:
            fix_str((LONG FAR *)&rs_object[ii].ob_spec);
            break;

         case G_TEXT:
         case G_BOXTEXT:
         case G_FTEXT:
         case G_FBOXTEXT:
           if (test != NIL)
              rs_object[ii].ob_spec = vsub_pointer(&rs_tedinfo[test], &starthdr);
            break;

         case G_ICON:
            if (test != NIL)
              rs_object[ii].ob_spec = vsub_pointer(&rs_iconblk[test], &starthdr);
            break;

         case G_IMAGE:
            if (test != NIL)
               rs_object[ii].ob_spec = vsub_pointer(&rs_bitblk[test], &starthdr);
            break;

         default:
            break;
      }
   }
}

VOID fix_tedinfo(VOID)
{
   WORD  ii;

   for (ii = 0; ii < NUM_TI; ii++)
   {
      fix_str((LPLONG)&rs_tedinfo[ii].te_ptext);
      fix_str((LPLONG)&rs_tedinfo[ii].te_ptmplt);
      fix_str((LPLONG)&rs_tedinfo[ii].te_pvalid);
   }
}

VOID fix_frstr(VOID)
{
   WORD ii;

   for (ii = 0; ii < NUM_FRSTR; ii++)
      fix_str((LPLONG)&rs_frstr[ii]);
}

VOID fix_str( where )
LPLONG where;
{
   if (*where != NIL)
      *where = sub_pointer(rs_strings[(WORD)*where], &starthdr);
}

VOID fix_iconblk(VOID)
{
   WORD   ii;

   for (ii = 0; ii < NUM_IB; ii++)
   {
      fix_img((LPLONG)&rs_iconblk[ii].ib_pmask);
      fix_img((LPLONG)&rs_iconblk[ii].ib_pdata);
      fix_str((LPLONG)&rs_iconblk[ii].ib_ptext);
   }
}

VOID fix_bitblk(VOID)
{
   WORD ii;

   for (ii = 0; ii < NUM_BB; ii++)
      fix_img((LPLONG)&rs_bitblk[ii].bi_pdata);
}

VOID fix_frimg(VOID)
{
   WORD ii;

   for (ii = 0; ii < NUM_FRIMG; ii++)
      fix_bb(&rs_frimg[ii]);
}

VOID fix_bb(LPLONG where)
{
   if (*where != NIL)
      *where = sub_pointer(&rs_bitblk[(WORD)*where], &starthdr);
}
   
VOID fix_img(LPLONG where)
{
   if (*where != NIL)
      *where = sub_pointer(rs_imdope[(WORD)*where].image, &starthdr);
}
 */

MLOCAL WORD sub_pointer(VOID *p1, VOID *p2)
{
//    LONG l1,l2;
// 
//    l1 = FPSEG(p1) * 16l;
//    l1 += FPOFF(p1);
//    l2 = FPSEG(p2) * 16l;
//    l2 += FPOFF(p2);

   return ((BYTE *)p1 - (BYTE *)p2);
}

MLOCAL VOID *vsub_pointer(BYTE *p1, BYTE *p2)
{
   return (LPVOID *)(sub_pointer(p1, p2));
}

