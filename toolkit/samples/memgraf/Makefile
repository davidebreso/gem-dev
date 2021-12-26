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
	$(LD) name $@ system dos libfile ../start-l.obj file \{  $< \} lib ../../lib/wccgem-l.lib \
	      option map=mgrflac.map

mgrfsac.obj:	memgraf.c 
	$(CC) -ms -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

mgrf-s.acc:	mgrfsac.obj
	$(LD) name $@ system dos libfile ../start-s.obj file \{  $< \} lib ../../lib/wccgem-s.lib \
	      option map=mgrfsac.map

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
