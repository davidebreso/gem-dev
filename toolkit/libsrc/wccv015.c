

#include "wccgem.h"
#include "wccv0.h"


    WORD
v_curtext( handle, string )
WORD handle;
BYTE *string; 
{
    WORD *intstr;

    intstr = intin;
    while (*intstr++ = *(unsigned char *)string++)
      ;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = intstr - intin - 1;
    contrl[5] = 12;
    contrl[6] = handle;
    vdi();
}
