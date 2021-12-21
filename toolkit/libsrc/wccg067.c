

#include "wccgem.h"
#include "wccg0.h"



	WORD
shel_find(ppath)
	LPVOID		ppath;
{
	SH_PATH = ppath;
	return( gem_if( SHEL_FIND ) );
}
