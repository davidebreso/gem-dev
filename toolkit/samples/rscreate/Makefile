include ../../makedefs.i

#
# nb: Low warning level, so that the RCS-generated file EXAMPLE.RSH 
# doesn't cause warnings
#
CFLAGS = -0 -ox -os -s -bcl=dos -w=0 -i=../../include
CC = wcl

all:	../rscr-s.exe 

../%.exe:	%.exe
	cp $< ..

rscr-s.exe:	rscreate.c
	$(CC) -ms $(CFLAGS) -fe=$@ -fm=rscr-s.map $< ../../lib/wccgem-s.lib


tidy:
	rm -f rscr-?.exe
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o
	rm -f *.err

clean:	tidy
