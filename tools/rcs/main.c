
/* A quick main() for GEM apps */

#include "wccgem.h"
#include <dos.h>
#include <conio.h>

static char *fail = "GEMAES not present in memory.\n";
/*static char *meme = "Unable to free memory.\n";*/
static char *exte = "Execution terminated.\n";


int main(int argc, char **argv)
{
	LPLONG pGEM = MK_FP(0, 4 * 0xEF);
	LPBYTE pSIG = (LPBYTE)*pGEM;
	int n;

	for (n = 0; n < 6; n++)
	{
		if (pSIG[n+2] != fail[n])
		{
			cputs(fail);
			cputs(exte);
			return 1;
		}
	}

	return GEMAIN(argc, argv);
}
