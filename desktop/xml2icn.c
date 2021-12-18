
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BASE 0x908

char linebuf[81];
FILE *fpin, *fpout;
int32_t iconlen = 0;

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

/* pack structures to 2-bytes to prevent adding extra bytes to ICONBLK */
#pragma pack(2)

typedef struct 
{
	int32_t	ib_pmask;
	int32_t	ib_pdata;
	int32_t	ib_ptext;
	int16_t	ib_char;
	int16_t	ib_xchar;
	int16_t	ib_ychar;
	int16_t	ib_xicon;
	int16_t	ib_yicon;
	int16_t	ib_wicon;
	int16_t	ib_hicon;
	int16_t	ib_xtext;
	int16_t	ib_ytext;
	int16_t	ib_wtext;
	int16_t	ib_htext;
} ICONBLK;

char caption[32][81];
ICONBLK ib[72];
unsigned char *bitdata[144];

void get_image(int ni)
{
	uint16_t fww  = (ib[ni].ib_wicon + 15) / 16;
	uint16_t fh   = ib[ni].ib_hicon;	
	unsigned char *pdata, *pmask;
	uint16_t x,y;
	uint16_t *row, *maskrow, mask;
	int c;

	iconlen = fww * 2 * fh;
	pmask = malloc(2 * iconlen);
	memset(pmask, 0, 2 * iconlen);
	pdata = pmask + iconlen;

	for (y = 0; y < fh; y++)
	{
		maskrow = (uint16_t *)(pmask + 2 * y * fww);
		row     = (uint16_t *)(pdata + 2 * y * fww);
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

void get_long(int32_t *l)
{
	char *s = strchr(linebuf, '>');
	if (!s) 
	{
		*l = 0;
		return;
	}
	*l = atol(s + 1);
}

void get_int(int16_t *s)
{
	int32_t l;
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
	uint16_t totlen;
	uint16_t base;
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
