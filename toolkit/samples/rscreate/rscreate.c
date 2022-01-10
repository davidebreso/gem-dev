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


#include <dos.h>
#include <stdio.h>
#include <string.h>
#include "wccgem.h"


MLOCAL WORD sub_pointer(LPVOID p1, LPVOID p2);
MLOCAL LPVOID vsub_pointer(LPVOID p1, LPVOID p2);
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

#pragma warning * 5

#include "example.rsh"

#pragma warning * 1

RSHDR FAR *starthdr; 
LPLONG ptr_frstr;
LPBIT ptr_bitblk;
LPLONG ptr_frimg;
LPICON ptr_iconblk;
LPTEDI ptr_tedinfo;
LPTREE ptr_object;
LPLONG ptr_trindex;

EXTERN WORD _nullarea;

int main(VOID)
{
    WORD /*jnk1,*/ handle;
    WORD cnt;                        /* in bytes   */
    WORD i;
    LPBYTE srcptr, dstptr, newptr;
    
    /*** allocate memory for RSC data ****/
    cnt = sizeof(RSHDR) + sub_pointer(pname, &_nullarea);
    starthdr = dos_alloc(cnt);
    _asm{ int 3 };
    printf("Allocating %d bytes for the resource data at %lX\n", cnt, (LONG)starthdr);
    if(starthdr == NULL) {
        printf("Cannot allocate memory!\n");
        return 1;
    }
    

    /*** copy strings ****/
    _asm{ int 3 };
    printf("Copy strings\n");
    dstptr = (LPBYTE)starthdr + sizeof(RSHDR);
    printf("Strings start at %lX\n", (LONG)dstptr);    
    for(i = 0; i < NUM_STRINGS; i++)
    {
        newptr = dstptr;
        for(srcptr = rs_strings[i]; *srcptr; srcptr++)
        {
            *dstptr = *srcptr;
            dstptr++;
        }
        rs_strings[i] = newptr;
        *dstptr=0;
        dstptr++;
    }
    /* aling to word boundary */
    if((LONG)dstptr & 0x1L) {
        *dstptr = 0;
        dstptr++;
    }
        
    /*** copy all other data ***/
    printf("Copy %d images at %lX\n", NUM_IMAGES, (LONG)dstptr);
    
    if(NUM_IMAGES >0) {
        /*** fix pointers in rs_imdope */
        printf("Fix image pointers\n");
        for(i = 0; i < NUM_IMAGES - 1; i++)
        {
            printf("Image %d at %lX\n", i, (LONG)dstptr);
            srcptr = (LPBYTE)rs_imdope[i].image;
            cnt = sub_pointer(rs_imdope[i+1].image, srcptr);
            _fmemcpy(dstptr, srcptr, cnt);
            rs_imdope[i].image = (LPWORD)dstptr;
            dstptr += cnt;
        }
        printf("Image %d at %lX\n", NUM_IMAGES - 1, (LONG)dstptr);
        srcptr = (LPBYTE)rs_imdope[NUM_IMAGES - 1].image;
        rs_imdope[NUM_IMAGES - 1].image = (LPWORD)dstptr;
        cnt = sub_pointer(rs_frstr, srcptr);
        _fmemcpy(dstptr, srcptr, cnt);
        dstptr += cnt;
    }
    printf("Copy rs_frstr at %lX\n", (LONG)dstptr);
    ptr_frstr = (LPLONG) dstptr;
    srcptr = (LPBYTE)rs_frstr;
    cnt = sub_pointer(rs_bitblk, srcptr);
    _fmemcpy(dstptr, srcptr, cnt);
    dstptr += cnt;

    printf("Copy rs_bitblk at %lX\n", (LONG)dstptr);
    ptr_bitblk = (LPBIT) dstptr;
    srcptr = (LPBYTE)rs_bitblk;
    cnt = sub_pointer(rs_frimg, srcptr);
    _fmemcpy(dstptr, srcptr, cnt);
    dstptr += cnt;

    printf("Copy rs_frimg at %lX\n", (LONG)dstptr);
    ptr_frimg = (LPLONG) dstptr;
    srcptr = (LPBYTE)rs_frimg;
    cnt = sub_pointer(rs_iconblk, srcptr);
    _fmemcpy(dstptr, srcptr, cnt);
    dstptr += cnt;

    printf("Copy rs_iconblk at %lX\n", (LONG)dstptr);
    ptr_iconblk = (LPICON) dstptr;
    srcptr = (LPBYTE)rs_iconblk;
    cnt = sub_pointer(rs_tedinfo, srcptr);
    _fmemcpy(dstptr, srcptr, cnt);
    dstptr += cnt;

    printf("Copy rs_tedinfo at %lX\n", (LONG)dstptr);
    ptr_tedinfo = (LPTEDI) dstptr;
    srcptr = (LPBYTE)rs_tedinfo;
    cnt = sub_pointer(rs_object, srcptr);
    _fmemcpy(dstptr, srcptr, cnt);
    dstptr += cnt;

    printf("Copy rs_object at %lX\n", (LONG)dstptr);
    ptr_object = (LPTREE) dstptr;
    srcptr = (LPBYTE)rs_object;
    cnt = sub_pointer(rs_trindex, srcptr);
    _fmemcpy(dstptr, srcptr, cnt);
    dstptr += cnt;
        
    printf("Copy rs_trindex at %lX\n", (LONG)dstptr);
    ptr_trindex = (LPLONG) dstptr;
    srcptr = (LPBYTE)rs_trindex;
    cnt = sub_pointer(rs_imdope, srcptr);
    _fmemcpy(dstptr, srcptr, cnt);
    dstptr += cnt;
    fflush(stdout);

    starthdr->rsh_vrsn = 0;
    printf("starthdr->rsh_vrsn=%04X\n", starthdr->rsh_vrsn);
    
    starthdr->rsh_object = sub_pointer(ptr_object,starthdr);
    printf("starthdr->rsh_object=%04X\n", starthdr->rsh_object);
    
    starthdr->rsh_tedinfo = sub_pointer(ptr_tedinfo,starthdr);
    printf("starthdr->rsh_tedinfo=%04X\n", starthdr->rsh_tedinfo);

    starthdr->rsh_iconblk = sub_pointer(ptr_iconblk,starthdr);
    printf("starthdr->rsh_iconblk=%04X\n", starthdr->rsh_iconblk);

    starthdr->rsh_bitblk = sub_pointer(ptr_bitblk,starthdr);
    printf("starthdr->rsh_bitblk=%04X\n", starthdr->rsh_bitblk);

    starthdr->rsh_frstr = sub_pointer(ptr_frstr,starthdr);
    printf("starthdr->rsh_frstr=%04X\n", starthdr->rsh_frstr);

    starthdr->rsh_string = sub_pointer(rs_strings[0],starthdr);
    printf("starthdr->rsh_string=%04X\n", starthdr->rsh_string);

    starthdr->rsh_imdata = sub_pointer(rs_imdope[0].image,starthdr);
    printf("starthdr->rsh_imdata=%04X\n", starthdr->rsh_imdata);

    starthdr->rsh_frimg = sub_pointer(ptr_frimg,starthdr);
    printf("starthdr->rsh_frimg=%04X\n", starthdr->rsh_frimg);

    starthdr->rsh_trindex = sub_pointer(ptr_trindex,starthdr);
    printf("starthdr->rsh_trindex=%04X\n", starthdr->rsh_trindex);

    starthdr->rsh_nobs    = NUM_OBS;
    printf("starthdr->rsh_nobs=%04X\n", starthdr->rsh_nobs);
    starthdr->rsh_ntree   = NUM_TREE;
    printf("starthdr->rsh_ntree=%04X\n", starthdr->rsh_ntree);
    starthdr->rsh_nted    = NUM_TI;
    printf("starthdr->rsh_nted=%04X\n", starthdr->rsh_nted);
    starthdr->rsh_nib     = NUM_IB;
    printf("starthdr->rsh_nib=%04X\n", starthdr->rsh_nib);
    starthdr->rsh_nbb     = NUM_BB;
    printf("starthdr->rsh_nbb=%04X\n", starthdr->rsh_nbb);
    starthdr->rsh_nimages = NUM_FRIMG;
    printf("starthdr->rsh_nimages=%04X\n", starthdr->rsh_nimages);
    starthdr->rsh_nstring = NUM_FRSTR;
    printf("starthdr->rsh_nstring=%04X\n", starthdr->rsh_nstring);

    fix_trindex();
    fix_objects();
    fix_tedinfo();
    fix_iconblk();
    fix_bitblk();
    fix_frstr();
    fix_frimg();

    handle = dos_create(pname, 0); 
    cnt = sub_pointer(dstptr,starthdr);
    printf("Writing %d bytes to %s\n", cnt, pname);
    starthdr->rsh_rssize = (UWORD)cnt;
    dos_write(handle, cnt, (LPBYTE)starthdr); 

    dos_close( handle );
    dos_free(starthdr);
    
    return 0;
}

 
VOID fix_trindex(VOID)
{
   WORD test, ii;

   for (ii = 0; ii < NUM_TREE; ii++)
   {
      test = (WORD) ptr_trindex[ii];
      ptr_trindex[ii] = (WORD) sub_pointer(&ptr_object[test], starthdr);
   }
}

VOID fix_objects(VOID)
{
   WORD   test, ii;

   for (ii = 0; ii < NUM_OBS; ii++)
   {
      test = (WORD) ptr_object[ii].ob_spec;
      switch (ptr_object[ii].ob_type & 0xff)
      {
         case G_TITLE:
         case G_STRING:
         case G_BUTTON:
            fix_str((LONG FAR *)&ptr_object[ii].ob_spec);
            break;

         case G_TEXT:
         case G_BOXTEXT:
         case G_FTEXT:
         case G_FBOXTEXT:
           if (test != NIL)
              ptr_object[ii].ob_spec = vsub_pointer(&ptr_tedinfo[test], starthdr);
            break;

         case G_ICON:
            if (test != NIL)
              ptr_object[ii].ob_spec = vsub_pointer(&ptr_iconblk[test], starthdr);
            break;

         case G_IMAGE:
            if (test != NIL)
               ptr_object[ii].ob_spec = vsub_pointer(&ptr_bitblk[test], starthdr);
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
      fix_str((LPLONG)&ptr_tedinfo[ii].te_ptext);
      fix_str((LPLONG)&ptr_tedinfo[ii].te_ptmplt);
      fix_str((LPLONG)&ptr_tedinfo[ii].te_pvalid);
   }
}

VOID fix_frstr(VOID)
{
   WORD ii;

   for (ii = 0; ii < NUM_FRSTR; ii++)
      fix_str((LPLONG)&ptr_frstr[ii]);
}

VOID fix_str(LPLONG where )
{
   if (*where != NIL)
      *where = sub_pointer(rs_strings[(WORD)*where], starthdr);
}

VOID fix_iconblk(VOID)
{
   WORD   ii;

   for (ii = 0; ii < NUM_IB; ii++)
   {
      fix_img((LPLONG)&ptr_iconblk[ii].ib_pmask);
      fix_img((LPLONG)&ptr_iconblk[ii].ib_pdata);
      fix_str((LPLONG)&ptr_iconblk[ii].ib_ptext);
   }
}

VOID fix_bitblk(VOID)
{
   WORD ii;

   for (ii = 0; ii < NUM_BB; ii++)
      fix_img((LPLONG)&ptr_bitblk[ii].bi_pdata);
}

VOID fix_frimg(VOID)
{
   WORD ii;

   for (ii = 0; ii < NUM_FRIMG; ii++)
      fix_bb(&ptr_frimg[ii]);
}

VOID fix_bb(LPLONG where)
{
   if (*where != NIL)
      *where = sub_pointer(&ptr_bitblk[(WORD)*where], starthdr);
}
   
VOID fix_img(LPLONG where)
{
   if (*where != NIL)
      *where = sub_pointer(rs_imdope[(WORD)*where].image, starthdr);
}


MLOCAL WORD sub_pointer(LPVOID p1, LPVOID p2)
{
//    LONG l1,l2;
// 
//    l1 = FPSEG(p1) * 16l;
//    l1 += FPOFF(p1);
//    l2 = FPSEG(p2) * 16l;
//    l2 += FPOFF(p2);

   return ((LPBYTE)p1 - (LPBYTE)p2);
}

MLOCAL LPVOID vsub_pointer(LPVOID p1, LPVOID p2)
{
   return (LPVOID *)(sub_pointer(p1, p2));
}

