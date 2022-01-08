
CC=wcc
LD=wlink
AS=wasm
CFLAGS=-bt=dos -os -s -0 -I../../toolkit/include
LIBDIR=../../toolkit/lib
INSTDIR=../../toolkit/tools

OBJECTS=main.obj rcfile.obj rclib.obj rcmain.obj

all:	rsconv.app

rsconv.app:	$(OBJECTS) 
	$(LD) name $@ system dos file \{ $(OBJECTS) \} lib $(LIBDIR)/wccgem-l.lib option map

%.obj:	%.c rcfile.h rclib.h rsconv.h
	$(CC) -ml $(CFLAGS) -fo=$@ $< 

%.obj:	%.asm
	$(AS) -0 $< -fo=$@ -ml

install: rsconv.app rsconv.rsc
	cp $^ $(INSTDIR)

clean:
	rm -f *.obj 
	rm -f *.exe 
	rm -f *.app 
	rm -f *.lib
	rm -f *.map
	rm -f *.err
