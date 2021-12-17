include ../../makedefs.i

CFLAGS = -0 -ox -os -bcl=dos -i=../../include
CC = wcl

SOURCE = events.c meta_f.c meta_out.c meta_r.c meta_t.c wnd.c xmain.c
HDRS   = events.h meta.h proto.h types.h window.h

all:	../events-l.app ../events-s.app ../events.rsc ../events.gem

../%.app:	%.app
	cp $< ..

../events.rsc:	events.rsc
	cp events.rsc ..

../events.gem:	events.gem
	cp events.gem ..

events-l.app:	$(SOURCE) $(HDRS)
	$(CC) -ml $(CFLAGS) -fe=$@ $(SOURCE) ../../lib/wccgem-l.lib

events-s.app:	$(SOURCE) $(HDRS)
	$(CC) -ms $(CFLAGS) -fe=$@ $(SOURCE) ../../lib/wccgem-s.lib

tidy:
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o
	rm -f events-?.app

clean:	tidy
