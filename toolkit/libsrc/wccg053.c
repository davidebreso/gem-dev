

#include "wccgem.h"
#include "wccg0.h"



	WORD
wind_close(handle)
	WORD		handle;
{
	WM_HANDLE = handle;
	return( gem_if( WIND_CLOSE ) );
}
