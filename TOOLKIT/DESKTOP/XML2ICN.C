
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE 0x908

char linebuf[81];
FILE *fpin, *fpout;
long iconlen = 0;

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

typedef struct 
{
	long	ib_pmask;
	long	ib_pdata;
	long	ib_ptext;
	short	ib_char;
	short	ib_xchar;
	short	ib_ychar;
	short	ib_xicon;
	short	ib_yicon;
	short	ib_wicon;
	short	ib_hicon;
	short	ib_xtext;
	short	ib_ytext;
	short	ib_wtext;
	short	ib_htext;
} ICONBLK;

char caption[32][81];
ICONBLK ib[72];
unsigned char *bitdata[144];

void get_image(int ni)
{
	unsigned fww  = (ib[ni].ib_wicon + 15) / 16;
	unsigned fh   = ib[ni].ib_hicon;	
	unsigned char *pdata, *pmask;
	unsigned x,y;
	unsigned *row, *maskrow, mask;
	int c;

	iconlen = fww * 2 * fh;
	pmask = malloc(2 * iconlen);
	memset(pmask, 0, 2 * iconlen);
	pdata = pmask + iconlen;

	for (y = 0; y < fh; y++)
	{
		maskrow = (unsigned *)(pmask + 2 * y * fww);
		row     = (unsigned *)(pdata + 2 * y * fww);
		mask = 0x8000;
		for (x = 0; x < ib[ni].ib_wicon; x++)
		{
			do
			{
				c = fgetc(fpin);
       				if (c == '<' || c == EOF) return;
			} while (c != '.' && c != '-' && c != '*' && c != '#');

			switch(c)
			{
				case '.': break;
				case '*': row[0]     |= mask; /* FALL THROUGH */
				case '-': maskrow[0] |= mask; break;
				case '#': row[0]     |= mask; break;
			}	
			mask = mask >> 1;
			if (mask == 0)
			{
				mask = 0x8000;
				++row;
				++maskrow;
			}
		}
	}
	x = y = 0;
	for (c = 0; c < 144; c++) if (bitdata[c])
	{
		if (!memcmp(pmask, bitdata[c], iconlen))
		{
			ib[ni].ib_pmask = c;
			x = 1;
		}
		if (!memcmp(pdata, bitdata[c], iconlen))
		{
			ib[ni].ib_pdata = c;
			y = 1;
		}
	}	
	if (x && y) 
	{
		free(pmask);
		return;
	}
	for (c = 0; c < 144; c++) if (!bitdata[c])
	{
		if (x && y) break;
		if (!x) 
		{
			bitdata[c] = pmask;
			ib[ni].ib_pmask = c;
			x = 1;
			continue;
		}
		if (!y)
		{
			bitdata[c] = pdata;
			ib[ni].ib_pdata = c;
			y = 1;
			continue;
		}
	}
}

void get_caption(int nc)
{
	char *lb, *rb;

	if (nc < 0 || nc >= 32) return;
	rb = strchr(linebuf, '>');
	if (!rb) return;
	++rb;
	lb = strchr(rb, '<');
	if (lb) *lb = 0;
	strncpy(caption[nc], rb, 80); 
	caption[nc][80] = 0;
}

void get_long(long *l)
{
	char *s = strchr(linebuf, '>');
	if (!s) 
	{
		*l = 0;
		return;
	}
	*l = atol(s + 1);
}

void get_int(short *s)
{
	long l;
	get_long(&l);
	*s = l;
}

void parse_icon(void)
{
	char *strid = strstr(linebuf, "id=\"");
	int id;

	if (!strid) return;
	id = atoi(strid + 4);
	if (id < 0 || id > 72) return;

	while (fgets(linebuf, sizeof(linebuf), fpin) != NULL)
	{
		if      (strstr(linebuf, "</icon>"))   return;
		else if (strstr(linebuf, "<caption>")) get_caption(id - 8);
		else if (strstr(linebuf, "<letter>"))  get_int(&ib[id].ib_char);
		else if (strstr(linebuf, "<xchar>"))   get_int(&ib[id].ib_xchar);
		else if (strstr(linebuf, "<ychar>"))   get_int(&ib[id].ib_ychar);
		else if (strstr(linebuf, "<xicon>"))   get_int(&ib[id].ib_xicon);
		else if (strstr(linebuf, "<yicon>"))   get_int(&ib[id].ib_yicon);
		else if (strstr(linebuf, "<wicon>"))   get_int(&ib[id].ib_wicon);
		else if (strstr(linebuf, "<hicon>"))   get_int(&ib[id].ib_hicon);
		else if (strstr(linebuf, "<xtext>"))   get_int(&ib[id].ib_xtext);
		else if (strstr(linebuf, "<ytext>"))   get_int(&ib[id].ib_ytext);
		else if (strstr(linebuf, "<wtext>"))   get_int(&ib[id].ib_wtext);
		else if (strstr(linebuf, "<htext>"))   get_int(&ib[id].ib_htext);
		else if (strstr(linebuf, "<ptext>"))   get_long(&ib[id].ib_ptext);
		else if (strstr(linebuf, "<image>"))   get_image(id);
	}

}

void parse_icons(void)
{
	while (fgets(linebuf, sizeof(linebuf), fpin) != NULL)
	{
		if (strstr(linebuf, "<icon")) parse_icon();
		else if (strstr(linebuf, "</icons>")) return;
	}
}


int main(int argc, char **argv)
{
	unsigned totlen;
	unsigned base;
	int n, m;

	if (argc < 3)
	{
		fprintf(stderr, "Syntax: xml2icn xml-file icn-file\n");
		return 1;
	}

	fpin = fopen(argv[1], "r");
	if (!fpin)
	{
		perror(argv[1]);
		return 1;
	}
	while (fgets(linebuf, sizeof(linebuf), fpin) != NULL)
	{
		if (strstr(linebuf, "<icons>")) parse_icons();
	}
	fclose(fpin);

	fpout = fopen(argv[2], "wb");
	if (!fpout)
	{
		perror(argv[2]);
		return 1;
	}
	m = 0;
	for (n = 0; n < 144; n++) if (bitdata[n]) ++m;

	totlen = 4 + 72 * sizeof(ICONBLK) + m * iconlen + BASE;
	for (n = 0; n < 32; n++) totlen += 1 + strlen(caption[n]);
	base = BASE;
	fwrite(&totlen, 1, sizeof(totlen), fpout);
	fwrite(&base,   1, sizeof(base),   fpout);
	fwrite(ib,      1, sizeof(ib),     fpout);
	for (n = 0; n < 144; n++)
	{
		if (bitdata[n]) fwrite(bitdata[n], 1, iconlen, fpout);
	}
	for (n = 0; n < 32; n++)
	{
		fwrite(caption[n], 1, 1 + strlen(caption[n]), fpout);
	}
	base = 4 + 72 * sizeof(ICONBLK) + m * iconlen + BASE;
	for (n = 0; n < 32; n++)
	{
		fwrite(&base, 1, sizeof(base), fpout);
		base += 1 + strlen(caption[n]);
	}
	fclose(fpout);
	return 0;
}
