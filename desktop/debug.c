/*  DEBUG.C     16-01-2022 Davide Bresolin                          
 *
 *       Copyright 2022 Davide Bresolin                                  
 *       This software is licenced under the GNU Public License.         
 *       Please see LICENSE.TXT for further information.                 
 *                                                                       
 */

#include "wccdesk.h"

#if DEBUG 
static char buf[4096];
static FILE *logfile;

WORD  form_valert(WORD button, BYTE *str, ...)
{
	va_list ap;
	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);
	return form_alert(button, ADDR(buf));
}

VOID  dbg(BYTE *str, ...)
{	
	va_list ap;
	va_start(ap, str);
	vfprintf(logfile, str, ap);
	va_end(ap);
}

VOID start_log()
{
    logfile = fopen("desktop.log", "w");
    fprintf(logfile, "Starting DESKTOP\n");
}

VOID end_log()
{
	fprintf(logfile, "Closing DESKTOP.\n");
	fclose(logfile);
}

extern WORD  getcs(void);
extern WORD  getip(void);

 * Dump system memory to disc, one byte at a time! 
 * Writes 1Mb of data, followed by four bytes far 
 * pointer to this function giving CS and IP.
 * 
VOID  crashdump(BYTE bt)
{
	char filename[20];
	long n;
	FILE *fp;
	LPBYTE ptr;
	WORD cs, ip;

	dbg("Creating crashdump %c\n", bt);
	sprintf(filename, "c:/gemapp_%c.mem", bt);
	fp = fopen(filename, "wb");
	for (n = 0; n < 0x100000L; n++)
	{
		ptr = MK_FP(n >> 4, n & 0x0F);
		fputc(ptr[0], fp);
	}
	cs = getcs();
	ip = getip();
	fwrite(&ip, 1, 2, fp);
	fwrite(&cs, 1, 2, fp);
	fclose(fp);

	dbg("Created crashdump %c as %s CS=%04x IP=%04x\n", bt, filename,
			cs, ip);
}

void  debugbreak(void)
{
    _asm {
        int 3
    };
}

__declspec( naked ) WORD  getcs(void) {
    _asm{
        mov ax, cs
        ret
    }
}
#pragma aux getcs value [ax] modify exact [ax] nomemory;

__declspec( naked ) WORD  getip(void) {
    _asm{
        pop ax
        push ax
        ret
    }
}
#pragma aux getip value [ax] modify exact [ax] nomemory;

#endif
