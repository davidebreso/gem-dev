include ../../makedefs.i

CFLAGS = -bt=dos -os -s -0 -i../../include
CC = wcc
LD = wlink

all:	../demo-l.app ../demo-s.app ../demo.rsc

../%.app:	%.app
	cp $< ..

../demo.rsc:	demo.rsc
	cp demo.rsc ..

demo-l.app:	demo-l.obj
	$(LD) name $@ system dos file \{ $< ../main-l.obj\} lib ../../lib/wccgem-l.lib option map=demo-l

demo-l.obj:	demo.c demo.h 
	$(CC) -ml $(CFLAGS) -fo=$@ $< 

demo-s.app:	demo-s.obj
	$(LD) name $@ system dos file \{ $< ../main-s.obj \} lib ../../lib/wccgem-s.lib option map=demo-s

demo-s.obj:	demo.c demo.h
	$(CC) -ms $(CFLAGS) -fo=$@ $< 

tidy:
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err
	rm -f demo-?.app

clean:	tidy
