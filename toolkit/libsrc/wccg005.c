

#include "wccgem.h"
#include "wccg0.h"



	WORD
appl_find(pname)
	LPVOID		pname;
{
	AP_PNAME = pname;
	return( gem_if(APPL_FIND) );
}
