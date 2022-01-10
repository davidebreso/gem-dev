/************************************************************************

    EDICON 2.00 - icon editor for GEM

    Copyright (C) 1998,1999,2000  John Elliott <jce@seasip.demon.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*************************************************************************/


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

