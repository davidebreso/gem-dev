include ../../makedefs.i

CFLAGS = -0 -ox -os -s -bcl=dos -i=../../include
CC = wcl

all:	../xform-l.app ../xform-s.app ../xform.rsc

../%.app:	%.app
	cp $< ..

../xform.rsc:	xform.rsc
	cp xform.rsc ..

xform-l.app:	xform.c xform.h
	$(CC) -ml $(CFLAGS) -fe=$@ xform.c -fm=xform-l.map ../main-l.obj ../../lib/wccgem-l.lib

xform-s.app:	xform.c xform.h
	$(CC) -ms $(CFLAGS) -fe=$@ xform.c -fm=xform-s.map ../main-s.obj ../../lib/wccgem-s.lib

tidy:
	rm -f *.obj
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o
	rm -f xform-?.app
	rm -f *.err

clean:	tidy
