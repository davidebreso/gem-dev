

#include "wccgem.h"
#include "wccd0.h"



	WORD
dos_setdt(handle, time, date)	/* set a file's date and time */
	WORD		handle, time, date;
{
	DOS_AX = 0x5701;
	DOS_BX = handle;
	DOS_CX = time;
	DOS_DX = date;

	__DOS();
} /* DOSBIND.C  */
