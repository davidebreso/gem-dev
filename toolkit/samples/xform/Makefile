include ../../makedefs.i

CFLAGS = -bt=dos -os -s -0 -i../../include
CC = wcc
LD = wlink

all:	../xform-l.app ../xform-s.app ../xform.rsc

../%.app:	%.app
	cp $< ..

../xform.rsc:	xform.rsc
	cp xform.rsc ..

xform-l.app:	xform-l.obj
	$(LD) name $@ system dos file \{ $< ../main-l.obj \} lib ../../lib/wccgem-l.lib option map
	
xform-l.obj:	xform.c xform.h
	$(CC) -ml $(CFLAGS) -fo=$@ $<	

xform-s.app:	xform-s.obj
	$(LD) name $@ system dos file \{ $< ../main-s.obj \} lib ../../lib/wccgem-s.lib option map
	
xform-s.obj:	xform.c xform.h
	$(CC) -ms $(CFLAGS) -fo=$@ $<	

tidy:
	rm -f *.obj
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f xform-?.app
	rm -f *.err

clean:	tidy
