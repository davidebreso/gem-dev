CC=wcc
LD=wlink
AS=wasm
CFLAGS=-bt=dos -os -s -0 -I../../toolkit/include
LIBDIR=../../toolkit/lib

OBJECTS = sysfont.obj dlgs.obj memfont.obj loadfont.obj main.obj utils.obj view.obj rsrc.obj
BINARIES = sysfont.app sysfont.rsc sysfont.txt license.txt sysfont.ico sysfdoc.ico sdpsc9.vga
CSOURCE  = sysfont.c dlgs.c memfont.c loadfont.c main.c utils.c view.c rsrc.c
HSOURCE  = sysfont.h memfont.h view.h sysfonti.h
XSOURCE  = sysfont.dfn Makefile
ZIP = zip
RM = rm -f

all:	../sysfont.app ../sysfont.rsc ../sysfont.txt \
		../sysfont.ico ../sysfdoc.ico ../sdpsc9.vga

../%: %
	cp $< $@

sysfont.app:	$(OBJECTS)
	$(LD) name $@ system dos file \{ $(OBJECTS) \} lib $(LIBDIR)/wccgem-l.lib option map

%.obj:	%.c $(HSOURCE)
	$(CC) -ml $(CFLAGS) -fo=$@ $< 

clean:
	$(RM) *.app
	$(RM) *.obj
	$(RM) *.err
	$(RM) *.map

tidy: clean

