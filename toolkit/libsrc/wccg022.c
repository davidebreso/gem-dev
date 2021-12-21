

#include "wccgem.h"
#include "wccg0.h"


	WORD
menu_register(pid, pstr)
	WORD		pid;
	LPVOID		pstr;
{
	MM_PID = pid;
	MM_PSTR = pstr;
	return( gem_if( MENU_REGISTER ) );
}
