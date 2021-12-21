

#include "wccgem.h"
#include "wccd0.h"



	WORD
dos_rmdir(ppath)	/* remove directory entry */
	LPBYTE		ppath;
{
	dos_lpvoid(0x3a00, ppath);
	return(!DOS_ERR);
}
