/*
*       WCCPTR.H: True "far pointer" types for Open Watcom 
*
*       Copyright 1999, John Elliott
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*/

#ifdef __WATCOMC__
/* macros to break 'far' pointers into segment and offset components */
#define FP_OFF(__p) ((unsigned)(__p))
#define FP_SEG(__p) ((unsigned)((unsigned long)(void __far*)(__p) >> 16))
/* make a far pointer from segment and offset */
#define MK_FP(__s,__o) (((unsigned short)(__s)):>((void __near *)(__o)))

typedef VOID    FAR *LPVOID;
typedef LPVOID  FAR *LPLPTR;
typedef BYTE    FAR *LPBYTE;
typedef WORD    FAR *LPWORD;
typedef LONG    FAR *LPLONG;
typedef UWORD   FAR *LPUWORD;
typedef ULONG   FAR *LPULONG;
#else
#error This requires Open Watcom!
#endif

