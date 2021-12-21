

#include "wccgem.h"
#include "wccd0.h"



	WORD
dos_sdta(ldta)	/* set the Disk Transfer Address ( DTA ) */
	LPVOID		ldta;
{
	dos_lpvoid(0x1a00, ldta);
}
