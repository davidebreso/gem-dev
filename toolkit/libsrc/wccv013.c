

#include "wccgem.h"
#include "wccv0.h"


    WORD
v_eeol( handle )
WORD handle;
{
    contrl[0] = 5;
    contrl[1] = contrl[3] = 0;
    contrl[5] = 10;
    contrl[6] = handle;
    vdi();
}
