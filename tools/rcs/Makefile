
CC=wcc
LD=wlink
AS=wasm
CFLAGS=-bt=dos -os -s -0 -I../../toolkit/include
LIBDIR=../../toolkit/lib
INSTDIR=../../toolkit/tools

OBJECTS=main.obj rcsmain.obj rcsdata.obj rcslib.obj wccerror.obj rcsvdi.obj \
	rcsintf.obj rcsedit.obj rcsobjs.obj rcsmenu.obj rcsiinit.obj rcsimain.obj \
	rcsiedit.obj rcstrees.obj rcsfiles.obj rcsread.obj rcsinit.obj rcsifile.obj \
	rcsalert.obj rcswrite.obj rcsasm.obj rcsload.obj tcrtlasm.obj

HEADERS=wccrcs.h

all:	rcs.app

rcs.app:	$(OBJECTS) 
	$(LD) name $@ system dos file \{ $(OBJECTS) \} lib $(LIBDIR)/wccgem-l.lib \
	option map

%.obj:	%.c $(HEADERS)
	$(CC) -ml $(CFLAGS) -fo=$@ $< 

%.obj:	%.asm
	$(AS) -0 $< -fo=$@ -ml
	
install: rcs.app rcs.rsc rcslow.rsc
	cp $^ $(INSTDIR)

clean:
	rm -f *.obj 
	rm -f *.exe 
	rm -f *.app 
	rm -f *.lib
	rm -f *.map
	rm -f *.err
