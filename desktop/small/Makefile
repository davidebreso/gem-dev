include ../makedefs.i

CC=wcc
LD=wlink
AS=wasm
CFLAGS=-bt=dos -os -s -0 -I$(GDKDIR)/include
LIBDIR=$(GDKDIR)/lib

OBJECTS0=desktop.obj  
OBJECTS1=desk1.obj    deskact.obj  deskfun.obj  deskwin.obj 
OBJECTS2=deskpro.obj  deskwcc.obj  deskobj.obj  deskfpd.obj  deskins.obj 
OBJECTS3=deskapp.obj deskgraf.obj deskrsrc.obj  deskfrmt.obj bootsect.obj
OBJECTS4=deskiacc.obj deskinf.obj  deskdir.obj  deskgsx.obj  main.obj 
OBJECTS5=desktype.obj deskcds.obj  optimize.obj int3.obj desksupp.obj

OBJECTS=$(OBJECTS0) $(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4) $(OBJECTS5)

all:	desktop.app

desktop.app:	$(OBJECTS) 
	$(LD) name $@ system dos file \{ $(OBJECTS) \} lib $(LIBDIR)/wccgem-s.lib option map=test.map

%.obj:	../%.c ../deskprot.h ../desktop.h ../wccdesk.h
	$(CC) -ms $(CFLAGS) -fo=$@ $< 

%.obj:	../%.asm
	$(AS) -0 $< -fo=$@ -ms

clean:
	rm -f *.obj 
	rm -f *.exe 
	rm -f *.app 
	rm -f *.lib
	rm -f *.map
	rm -f *.err
