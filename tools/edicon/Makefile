
CC=wcc
LD=wlink
AS=wasm
CFLAGS=-bt=dos -os -s -0 -I../../toolkit/include
LIBDIR=../../toolkit/lib

OBJECTS=aboutbox.obj ediconst.obj iconfile.obj popcolou.obj utils.obj clipbd.obj \
        editor.obj main.obj popmenu.obj x_formdo.obj edicon.obj farmem.obj \
        newfile.obj popshade.obj

HEADERS=clipbd.h ediconf.h farmem.h popcolou.h popshade.h edicon.h editor.h \
        iconfile.h popmenu.h win3ico.h

all:	../edicon.app ../edicon.rsc ../edicon.doc

../%: %
	cp $< $@

edicon.app:	$(OBJECTS) 
	$(LD) name $@ system dos file \{ $(OBJECTS) \} lib $(LIBDIR)/wccgem-l.lib \
	option map

%.obj:	%.c $(HEADERS)
	$(CC) -ml $(CFLAGS) -fo=$@ $< 

%.obj:	%.asm
	$(AS) -0 $< -fo=$@ -ml

clean:
	rm -f *.obj 
	rm -f *.exe 
	rm -f *.app 
	rm -f *.lib
	rm -f *.map
	rm -f *.err
	
tidy: clean

