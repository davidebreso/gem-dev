

#include "wccgem.h"
#include "wccg0.h"



					/* Scrap Manager		*/
	WORD
scrp_read(pscrap)
	LPVOID		pscrap;
{
	SC_PATH = pscrap;
	return( gem_if( SCRP_READ ) );
}
