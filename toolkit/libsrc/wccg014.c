

#include "wccgem.h"
#include "wccg0.h"



	WORD
evnt_timer(locnt, hicnt)
	UWORD		locnt, hicnt;
{
	T_LOCOUNT = locnt;
	T_HICOUNT = hicnt;
	return( gem_if(EVNT_TIMER) );
}
