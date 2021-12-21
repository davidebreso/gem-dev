

#include "wccgem.h"
#include "wccv0.h"


    WORD
vsf_perimeter( handle, per_vis )
WORD handle, per_vis;
{
    intin[0] = per_vis;

    contrl[0] = 104;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
