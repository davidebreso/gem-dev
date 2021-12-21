

#include "wccgem.h"
#include "wccd0.h"




	WORD
dos_chdir(pdrvpath) 	/*	change the current directory 	*/
	LPBYTE		pdrvpath;
{
	dos_lpvoid(0x3b00, pdrvpath);
}
