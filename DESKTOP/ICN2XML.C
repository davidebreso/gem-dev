
#include <stdio.h>
#include <stdlib.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

unsigned char *buf;
FILE *fpout;

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

void dump_graphic(ICONBLK *ib)
{
	int x, y;
	unsigned fww = ((ib->ib_wicon + 15) / 16);
	unsigned iconsize = fww * 2 * ib->ib_hicon;
	unsigned mask, *row, *maskrow;
	unsigned char *pdata, *pmask;
     	unsigned char *iconbase = buf + 4 + (72 * sizeof(ICONBLK)); 

	pdata = iconbase + iconsize * ib->ib_pdata;
	pmask = iconbase + iconsize * ib->ib_pmask;

	fprintf(fpout, "    <image>\n");
	for (y = 0; y < ib->ib_hicon; y++)
	{
		mask = 0x8000;
		fprintf(fpout, "      ");
		row     = (unsigned *)(pdata + y * 2 * fww);
		maskrow = (unsigned *)(pmask + y * 2 * fww);
		for (x = 0; x < ib->ib_wicon; x++)
		{
			if (row[0] & mask)
			{
				if (maskrow[0] & mask) fputc('*', fpout);
				else		       fputc('#', fpout);
			}
			else
			{
				if (maskrow[0] & mask) fputc('-', fpout);
				else		       fputc('.', fpout);
			}
			mask = mask >> 1;
			if (mask == 0)
			{
				mask = 0x8000;
				++row;
				++maskrow;
			}
		}
		fputc('\n', fpout);

	}
	fprintf(fpout, "    </image>\n");

}

void dump_icon(int n)
{
	ICONBLK *ib = ((ICONBLK *)(buf + 4)) + n;
	short *sb = (short *)buf;
	short *captions = (short *) (buf + sb[0] - sb[1]);

	fprintf(fpout, "  <icon id=\"%d\">\n", n);
/*	fprintf(fpout, "    <mask>%ld</mask>\n",   ib->ib_pmask);
	fprintf(fpout, "    <image>%ld</image>\n", ib->ib_pdata);*/
	fprintf(fpout, "    <ptext>%ld</ptext>\n", ib->ib_ptext);
	fprintf(fpout, "    <letter>%d</letter>\n", ib->ib_char);
	fprintf(fpout, "    <xchar>%d</xchar>\n", ib->ib_xchar);
	fprintf(fpout, "    <ychar>%d</ychar>\n", ib->ib_ychar);
	fprintf(fpout, "    <xicon>%d</xicon>\n", ib->ib_xicon);
	fprintf(fpout, "    <yicon>%d</yicon>\n", ib->ib_yicon);
	fprintf(fpout, "    <wicon>%d</wicon>\n", ib->ib_wicon);
	fprintf(fpout, "    <hicon>%d</hicon>\n", ib->ib_hicon);
	fprintf(fpout, "    <xtext>%d</xtext>\n", ib->ib_xtext);
	fprintf(fpout, "    <ytext>%d</ytext>\n", ib->ib_ytext);
	fprintf(fpout, "    <wtext>%d</wtext>\n", ib->ib_wtext);
	fprintf(fpout, "    <htext>%d</htext>\n", ib->ib_htext);
	dump_graphic(ib);
	if (n > 7 && n < 40)
	{
		fprintf(fpout, "    <caption>%s</caption>\n", buf + captions[n-8] - sb[1]);
	}
	fprintf(fpout, "  </icon>\n");

}

int main(int argc, char **argv)
{
	long buflen;
	int n;
	FILE *fp;

	if (argc < 2)
	{
		fprintf(stderr, "Syntax: icn2xml icn-file {xml-file}\n");
		return 1;
	}
	if (argc > 2)
	{
		fpout = fopen(argv[2], "w");
		if (!fpout) { perror(argv[2]); return 1; }
	}
	else	fpout = stdout;

	fp = fopen(argv[1], "rb");
	if (!fp)
	{
		perror(argv[1]);
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	buflen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = malloc(buflen);
	fread(buf, 1, buflen, fp);
	fclose(fp);

	fprintf(fpout, "<icons>\n");
	for (n = 0; n < 72; n++)
	{
		dump_icon(n);	
	}
	fprintf(fpout, "</icons>\n");
	return 0;
}
