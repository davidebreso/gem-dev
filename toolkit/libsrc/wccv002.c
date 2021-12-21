

#include "wccgem.h"
#include "wccv0.h"


    WORD
v_clrwk( handle )
WORD handle;
{
    contrl[0] = 3;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
