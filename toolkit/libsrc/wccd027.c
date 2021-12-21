

#include "wccgem.h"
#include "wccd0.h"


	WORD
dos_chmod(pname, func, attr)	/* change file mode */
	LPBYTE		pname;
	WORD		func;
	WORD		attr;
{
	DOS_CX = attr;
	dos_lpvoid((UWORD) 0x4300 + func, pname);
	return((WORD) DOS_CX);
}
