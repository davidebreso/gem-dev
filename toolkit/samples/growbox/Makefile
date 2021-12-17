include ../../makedefs.i

CFLAGS = -0 -ox -os -bcl=dos -i=../../include
CC = wcl

all:	# ../growbx-l.acc ../growbx-s.acc

../%.acc:	%.exe
	../../bin/exe2acc $< $@


growbx-l.exe:	growbox.c 
	$(CC) -ml -DDESKACC $(CFLAGS) -Hgrowbx-l.sym -fe=$@ $< ../main-l.obj ../../lib/wccgem-l.lib

growbx-s.exe:	growbox.c
	$(CC) -ms -DESKACC $(CFLAGS) -Hgrowbx-s.sym -fe=$@ $< ../main-s.obj ../../lib/wccgem-s.lib

tidy:
	rm -f growbx-?.exe
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o

clean:	tidy
