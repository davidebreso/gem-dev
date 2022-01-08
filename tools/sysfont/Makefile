CC=wcc
LD=wlink
AS=wasm
CFLAGS=-bt=dos -os -s -0 -I../../toolkit/include
LIBDIR=../../toolkit/lib
INSTDIR=../../toolkit/tools

OBJECTS = sysfont.obj dlgs.obj memfont.obj loadfont.obj main.obj utils.obj view.obj rsrc.obj
BINARIES = sysfont.app sysfont.rsc sysfont.txt license.txt sysfont.ico sysfdoc.ico sdpsc9.vga
CSOURCE  = sysfont.c dlgs.c memfont.c loadfont.c main.c utils.c view.c rsrc.c
HSOURCE  = sysfont.h memfont.h view.h sysfonti.h
XSOURCE  = sysfont.dfn Makefile
ZIP = zip
RM = rm

all:	sysfont.app

sysfont.app:	$(OBJECTS)
	$(LD) name $@ system dos file \{ $(OBJECTS) \} lib $(LIBDIR)/wccgem-l.lib option map

%.obj:	%.c $(HSOURCE)
	$(CC) -ml $(CFLAGS) -fo=$@ $< 

clean:
	$(RM) $(OBJECTS)

sysfont.zip:	$(BINARIES)
	$(ZIP) $@ $(BINARIES)

sysfonts.zip:	$(BINARIES) $(CSOURCE) $(HSOURCE) $(XSOURCE)
	$(ZIP) $@ $(BINARIES)
	$(ZIP) $@ $(CSOURCE)
	$(ZIP) $@ $(HSOURCE)
	$(ZIP) $@ $(XSOURCE)

zip:	sysfont.zip sysfonts.zip

install: $(BINARIES)
	cp $(BINARIES) $(INSTDIR)

