

#include "wccgem.h"
#include "wccg0.h"



	WORD
evnt_dclick(rate, setit)
	WORD		rate, setit;
{
	EV_DCRATE = rate;
	EV_DCSETIT = setit;
	return( gem_if(EVNT_DCLICK) );
}
