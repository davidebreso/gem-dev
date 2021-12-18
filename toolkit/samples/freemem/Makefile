include ../../makedefs.i

CFLAGS = -0 -ox -os -s -bcl=dos -i=../../include
CC = wcl

all:	# ../fmem-l.acc ../fmem-s.acc ../freemem.rsc

../%.acc:	%.exe
	../../bin/exe2acc $< $@

../%.rsc:	%.rsc
	cp $< ..


fmem-l.exe:	freemem.c 
	$(CC) -ml -DDESKACC $(CFLAGS) -Hfmem-l.sym -fe=$@ $<  ../../lib/wccgem-l.lib

fmem-s.exe:	freemem.c
	$(CC) -ms -DESKACC $(CFLAGS) -Hfmem-s.sym -fe=$@ $< ../../lib/wccgem-s.lib

tidy:
	rm -f fmem-?.exe
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o
	rm -f *.err

clean:	tidy
