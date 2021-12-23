include ../../makedefs.i

CFLAGS = -0 -os -ox -s -bcl=dos -i=../../include
CC = wcl

all:	../udef-l.app ../udef-s.app ../userdef.rsc

../%.app:	%.app
	cp $< ..

../userdef.rsc:	userdef.rsc
	cp userdef.rsc ..

udef-l.app:	userdef.c userdef.h 
	$(CC) -ml $(CFLAGS) -fe=$@ -fm=udef-l.map -fo=udef-l.o $<  ../../lib/wccgem-l.lib
	
fardrawl.o: fardraw.asm
	wasm -ml -0 -bt=dos -fo=$@ $<

udef-s.app:	userdef.c userdef.h 
	$(CC) -ms $(CFLAGS) -fe=$@ -fm=udef-s.map -fo=udef-s.o $<  ../../lib/wccgem-s.lib

tidy:
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o
	rm -f udef-?.app
	rm -f *.err

clean:	tidy
