

#include "wccgem.h"
#include "wccg0.h"



	WORD
wind_update(beg_update)
	WORD		beg_update;
{
	WM_BEGUP = beg_update;
	return( gem_if( WIND_UPDATE ) );
}
