

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_rbox( handle, xy )
WORD handle, xy[];
{
    i_ptsin( xy );

    contrl[0] = 11;
    contrl[1] = 2;
    contrl[3] = 0;
    contrl[5] = 8;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}