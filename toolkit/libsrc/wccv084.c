

#include "wccgem.h"
#include "wccv0.h"


    WORD
vsm_string( handle, length, echo_mode, echo_xy, string )
WORD handle, length, echo_mode, echo_xy[];
BYTE *string;
{
    WORD    count,t;

    intin[0] = length;
    intin[1] = echo_mode;
    i_ptsin( echo_xy );

    contrl[0] = 31;
    contrl[1] = echo_mode;
    contrl[3] = 2;
    contrl[6] = handle;
    vdi();

    for (count = 0; count < contrl[4]; count++)
      *string++ = intout[count];
    *string = 0;  
    i_ptsin( ptsin );
    t = contrl[4];	/*this is a fix for bug in Lattice v3.2, it will*/
			/*give warning 85, if return(contrl[4]) directly*/
    return( t );
}
