

#include "wccgem.h"
#include "wccd0.h"



	WORD
dos_sfirst(pspec, attr)	/* search for first matching file */
	LPBYTE		pspec;
	WORD		attr;
{
	DOS_CX = attr; /* file attributes */

	dos_lpvoid(0x4e00, pspec);
	return(!DOS_ERR);
}
