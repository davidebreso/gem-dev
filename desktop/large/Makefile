
CC=wcc
LD=wlink
AS=wasm
CFLAGS=-bt=dos -os -s -0 -I../../toolkit/include
LIBDIR=../../toolkit/lib

OBJECTS0=desktop.obj  
OBJECTS1=desk1.obj    deskact.obj  deskfun.obj  deskwin.obj nooptim.obj
OBJECTS2=deskpro.obj  deskppd.obj  deskobj.obj  deskfpd.obj  deskins.obj 
OBJECTS3=deskapp1.obj deskapp2.obj deskapp3.obj deskgraf.obj deskrsrc.obj 
OBJECTS4=deskosif.obj deskinf.obj  deskdir.obj  deskgsx.obj  main.obj 
OBJECTS5=desktype.obj deskcds.obj  optimize.obj int3.obj desksupp.obj

OBJECTS=$(OBJECTS0) $(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4) $(OBJECTS5)

all:	desktop.app

desktop.app:	$(OBJECTS) 
	$(LD) name $@ system dos file \{ $(OBJECTS) $(LIBDIR)/wccgem-l.lib \} option map=test.map

nooptim.obj:	../nooptim.c ../deskprot.h ../desktop.h ../ppddesk.h
	$(CC) -ml $(CFLAGS) -fo=$@ $< 

%.obj:	../%.c ../deskprot.h ../desktop.h ../ppddesk.h
	$(CC) -ml $(CFLAGS) -fo=$@ $< 

%.obj:	../%.asm
	$(AS) -0 $< -fo=$@ -ml

clean:
	rm -f *.obj 
	rm -f *.exe 
	rm -f *.app 
	rm -f *.lib
	rm -f *.map
	rm -f *.err
