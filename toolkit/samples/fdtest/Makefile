include ../../makedefs.i

CFLAGS = -0 -ox -os -s -bcl=dos -i=../../include
CC = wcl

all:	../fdtest-l.app ../fdtest-s.app ../fdtest.rsc

../%.app:	%.app
	cp $< ..

../fdtest.rsc:	fdtest.rsc
	cp fdtest.rsc ..

fdtest-l.app:	fdtest.c fdtest.h 
	$(CC) -ml $(CFLAGS) -fe=$@ $< ../../lib/wccgem-l.lib

fdtest-s.app:	fdtest.c fdtest.h
	$(CC) -ms $(CFLAGS) -fe=$@ $< ../../lib/wccgem-s.lib

tidy:
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o
	rm -f fdtest-?.app
	rm -f *.err

clean:	tidy
