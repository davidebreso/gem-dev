include ../../makedefs.i

CLFLAGS = -0 -ox -os -s -bcl=dos -i=../../include
CCL = wcl
CFLAGS = -bt=dos -os -s -0 -i../../include
CC = wcc
LD = wlink

all:	../mgrf-l.app ../mgrf-s.app ../mgrf-l.acc ../mgrf-s.acc 

../%.app:	%.app
	cp $< ..

../%.acc:	%.acc
	cp $< ../


mgrf-l.app:	memgraf.c
	$(CCL) -ml $(CLFLAGS) -fe=$@ $<  ../../lib/wccgem-l.lib

mgrf-s.app:	memgraf.c
	$(CCL) -ms $(CLFLAGS) -fe=$@ $< ../../lib/wccgem-s.lib

mgrflac.obj:	memgraf.c 
	$(CC) -ml -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

mgrf-l.acc:	mgrflac.obj
	$(LD) name $@ system dos file \{ ../start-l.obj $< \} lib ../../lib/wccgem-l.lib \
	      option map=mgrflac.map \
	      order clname CODE segment BEGTEXT segment _TEXT clname FAR_DATA \
	      clname BEGDATA clname DATA clname BSS clname STACK clname EDATA

mgrfsac.obj:	memgraf.c 
	$(CC) -ms -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

mgrf-s.acc:	mgrfsac.obj
	$(LD) name $@ system dos file \{ ../start-s.obj $< \} lib ../../lib/wccgem-s.lib \
	      option map=mgrfsac.map \
	      order clname CODE segment BEGTEXT segment _TEXT clname FAR_DATA \
	      clname BEGDATA clname DATA clname BSS clname STACK clname EDATA

tidy:
	rm -f mgrf-?.acc
	rm -f mgrf-?.app
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.o
	rm -f *.err

clean:	tidy
