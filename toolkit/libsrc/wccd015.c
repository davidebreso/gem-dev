

#include "wccgem.h"
#include "wccd0.h"


	WORD
dos_wait()	/* get return code of a sub-process */
{
	DOS_AX = 0x4d00;
	__DOS();

	return((WORD) DOS_AX); /* 0 = normal termination */
}
