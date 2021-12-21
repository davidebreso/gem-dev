

#include "wccgem.h"
#include "wccg0.h"



	WORD
shel_rdef(lpcmd, lpdir)
	LPVOID		lpcmd;
	LPVOID		lpdir;
{
	SH_LPCMD = lpcmd;
	SH_LPDIR = lpdir;
	return( gem_if( SHEL_RDEF ) );
}
