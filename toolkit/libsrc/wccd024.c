

#include "wccgem.h"
#include "wccd0.h"



	WORD
dos_rename(poname, pnname)	/* rename file */
	LPBYTE		poname;
	LPBYTE		pnname;
{
	DOS_DI = FP_OFF(pnname);
	DOS_ES = FP_SEG(pnname);
	dos_lpvoid(0x5600, poname);
	return(!DOS_ERR);
}
