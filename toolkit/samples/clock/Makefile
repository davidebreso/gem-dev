include ../../makedefs.i

CFLAGS = -bt=dos -ohskl -0 -i../../include
CC = wcc
LD = wlink

all:	../clock-l.app ../clock-s.app # ../clock-l.acc ../clock-s.acc 

../%.app:	%.app
	cp $< ..

../%.acc:	%.exe
	../../bin/exe2acc $< $@

clock-l.app:	clock-l.obj
	$(LD) name $@ system dos file \{ $< ../../lib/wccgem-l.lib \}

clock-l.obj:	clock.c
	$(CC) -ml $(CFLAGS) -fo=$@ $<

clock-s.app:	clock-s.obj
	$(LD) name $@ system dos file \{ $< ../../lib/wccgem-s.lib \}

clock-s.obj:	clock.c
	$(CC) -ms $(CFLAGS) -fo=$@ $< 

clock-l.exe:	clock.c 
	$(CC) -ml -DDESKACC $(CFLAGS) -Hclock-l.sym -fe=$@ $<  ../../lib/wccgem-l.lib

clock-s.exe:	clock.c
	$(CC) -ms -DESKACC $(CFLAGS) -Hclock-s.sym -fe=$@ $< ../../lib/wccgem-s.lib

tidy:
	rm -f clock-?.exe
	rm -f clock-?.app
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj

clean:	tidy
