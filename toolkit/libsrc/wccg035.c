

#include "wccgem.h"
#include "wccg0.h"


	WORD
form_error(errnum)
	WORD		errnum;
{
	FM_ERRNUM = errnum;
	return( gem_if( FORM_ERROR ) );
}
