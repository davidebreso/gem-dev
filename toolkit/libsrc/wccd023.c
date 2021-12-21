

#include "wccgem.h"
#include "wccd0.h"



	WORD
dos_delete(pname)	/* delete file */
	LPBYTE		pname;
{
	dos_lpvoid(0x4100, pname);
	return((WORD) DOS_AX);
}
