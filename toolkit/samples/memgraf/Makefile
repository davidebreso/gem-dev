include ../../makedefs.i

CFLAGS = -0 -ox -os -bcl=dos -i=../../include
CC = wcl

all:	../mgrf-l.app ../mgrf-s.app # ../mgrf-l.acc ../mgrf-s.acc 

../%.app:	%.app
	cp $< ..

../%.acc:	%.exe
	../../bin/exe2acc $< $@


mgrf-l.app:	memgraf.c
	$(CC) -ml $(CFLAGS) -fe=$@ $<  ../../lib/wccgem-l.lib

mgrf-s.app:	memgraf.c
	$(CC) -ms $(CFLAGS) -fe=$@ $< ../../lib/wccgem-s.lib


mgrf-l.exe:	memgraf.c 
	$(CC) -ml -DDESKACC $(CFLAGS) -Hmgrf-l.sym -fe=$@ $<  ../../lib/wccgem-l.lib

mgrf-s.exe:	memgraf.c
	$(CC) -ms -DESKACC $(CFLAGS) -Hmgrf-s.sym -fe=$@ $< ../../lib/wccgem-s.lib

tidy:
	rm -f mgrf-?.exe
	rm -f mgrf-?.app
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o

clean:	tidy
