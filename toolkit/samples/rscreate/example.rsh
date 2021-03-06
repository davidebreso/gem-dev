#define T0OBJ 0
#define FREEBB 1
#define FREEIMG 3
#define FREESTR 4

BYTE *rs_strings[] = {
"",
"______",
"TEST:______",
"XXXXXX",
"Free-string TEST"};

WORD IMAG0[] = {
0x0, 0x0, 0x0, 0x0, 
0x73, 0xC000, 0x0, 0xFF, 
0x9C00, 0x0, 0x3FF, 0xFFC0, 
0x0, 0x7FFF, 0xFF00, 0x3, 
0xFFFF, 0xFF00, 0xF, 0xFFFF, 
0xFE00, 0x3F, 0xFFFF, 0xFFC0, 
0x7F, 0xFFFF, 0xFFF0, 0x1FF, 
0xFFFF, 0xFFF0, 0x7FF, 0xFFFF, 
0xFFF8, 0xFFF, 0xFFFF, 0xFFF8, 
0x1FFF, 0xFFFF, 0xFFFC, 0x1FFF, 
0xFFFF, 0xFFFC, 0x3FFF, 0xFFFF, 
0xFFFC, 0x3FFF, 0xFFFF, 0xFFFC, 
0x3DFF, 0xFFFF, 0xFFFC, 0x3CFF, 
0xFFFF, 0xFFFC, 0x3C0F, 0xFFFF, 
0xFFFC, 0x1803, 0xFFFF, 0xFFFC, 
0x0, 0x1FFF, 0xFFF8, 0x0, 
0xFC3, 0xFFF8, 0x7C00, 0xFC1, 
0xFFF8, 0x7C00, 0xFC1, 0xFFE0, 
0x7C00, 0xFC1, 0xFFE0, 0x7C00, 
0xF0F, 0xFFC0, 0x7C00, 0x61F, 
0xFF80, 0x0, 0x40F, 0xFF80, 
0x0, 0xFF, 0xF000, 0x0, 
0x7C, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG1[] = {
0x0, 0x0, 0x0, 0x0, 
0x73, 0xC000, 0x0, 0x8C, 
0x9C00, 0x0, 0x390, 0x67C0, 
0x0, 0x7E30, 0x8900, 0x3, 
0xC263, 0x9300, 0xE, 0x14AA, 
0x2600, 0x38, 0xA955, 0x5F00, 
0x60, 0x4000, 0x8AE0, 0x196, 
0x1800, 0x540, 0x345, 0x500, 
0x1050, 0x227, 0x1444, 0x110, 
0x7C0, 0xB00, 0x4428, 0x20, 
0x1211, 0x1148, 0x1FF2, 0x2A44, 
0x4418, 0x1BF5, 0x1311, 0x12A8, 
0x1182, 0xA6AA, 0xA218, 0x18F9, 
0x4D55, 0x5648, 0x80E, 0x70DF, 
0xFEA8, 0x3, 0xF0E0, 0x7C50, 
0x0, 0x18FE, 0x410, 0x0, 
0x88F, 0x80B0, 0x800, 0xA80, 
0xF960, 0x800, 0xA47, 0x80C0, 
0x1400, 0xBC0, 0xF880, 0x1400, 
0x901, 0x180, 0x800, 0x60E, 
0xF00, 0x0, 0x401, 0x7800, 
0x0, 0x7, 0x8000, 0x0, 
0x7C, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG2[] = {
0x0, 0x0, 0x3FFF, 0xFFF8, 
0x7FFF, 0xFFFC, 0xE000, 0xE, 
0xC000, 0x6, 0xC000, 0x6, 
0xC3F8, 0x3F86, 0xC7FC, 0x7FC6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x7FC6, 0xC3F8, 0x3F86, 
0xC000, 0x6, 0xC000, 0x6, 
0xE000, 0xE, 0x7FFF, 0xFFFC, 
0x3FFF, 0xFFF8, 0x0, 0x0};

WORD IMAG3[] = {
0x7FF, 0xFFFF, 0xFF80, 0xC00, 
0x0, 0xC0, 0x183F, 0xF03F, 
0xF060, 0x187F, 0xF860, 0x1860, 
0x187F, 0xF860, 0x1860, 0x187F, 
0xF860, 0x1860, 0x187F, 0xF860, 
0x1860, 0x187F, 0xF860, 0x1860, 
0x187F, 0xF860, 0x1860, 0x187F, 
0xF860, 0x1860, 0x187F, 0xF860, 
0x1860, 0x187F, 0xF860, 0x1860, 
0x187F, 0xF860, 0x1860, 0x187F, 
0xF860, 0x1860, 0x183F, 0xF03F, 
0xF060, 0xC00, 0x0, 0xC0, 
0x7FF, 0xFFFF, 0xFF80, 0x0, 
0x0, 0x0, 0x3F9, 0xF8FE, 
0x7F00, 0x301, 0x8CC0, 0x6000, 
0x3E1, 0xF8F8, 0x7C00, 0x301, 
0x98C0, 0x6000, 0x301, 0x8CFE, 
0x7F00, 0x0, 0x0, 0x0};

LONG rs_frstr[] = {
4L};

BITBLK rs_bitblk[] = {
2L, 4, 32, 0, 0, 1,
3L, 6, 24, 0, 0, 1};

LONG rs_frimg[] = {
1L};

ICONBLK rs_iconblk[] = {
0L, 1L, 0L, 4096,0,0, 0,0,48,32, 24,24,0,8};

TEDINFO rs_tedinfo[] = {
1L, 2L, 3L, 3, 6, 0, 0x1180, 0x0, -1, 7,12};

OBJECT rs_object[] = {
-1, 1, 3, G_BOX, NONE, OUTLINED, 0x21100L, 0,0, 21,9,
2, -1, -1, G_ICON, NONE, NORMAL, 0x0L, 4,1, 6,1026,
3, -1, -1, G_IMAGE, NONE, NORMAL, 0x0L, 13,1, 4,4,
0, -1, -1, G_FTEXT, 0x28, NORMAL, 0x0L, 5,6, 11,1};

LONG rs_trindex[] = {
0L};

struct foobar {
	WORD	dummy;
	WORD	*image;
	} rs_imdope[] = {
0, &IMAG0[0],
0, &IMAG1[0],
0, &IMAG2[0],
0, &IMAG3[0]};

#define NUM_STRINGS 5
#define NUM_FRSTR 1
#define NUM_IMAGES 4
#define NUM_BB 2
#define NUM_FRIMG 1
#define NUM_IB 1
#define NUM_TI 1
#define NUM_OBS 4
#define NUM_TREE 1

BYTE pname[] = "EXAMPLE.RSC";
