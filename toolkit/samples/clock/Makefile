include ../../makedefs.i

CFLAGS = -bt=dos -os -s -0 -i../../include
CC = wcc
LD = wlink

all:	../clock-l.app ../clock-s.app ../clock-l.acc ../clock-s.acc 

../%.app:	%.app
	cp $< ..

../%.acc:	%.acc
	cp $< ..

clock-l.app:	clock-l.obj
	$(LD) name $@ system dos file \{ $< \} lib ../../lib/wccgem-l.lib

clock-l.obj:	clock.c
	$(CC) -ml $(CFLAGS) -fo=$@ $<

clock-s.app:	clock-s.obj
	$(LD) name $@ system dos file \{ $< \} lib ../../lib/wccgem-s.lib 

clock-s.obj:	clock.c
	$(CC) -ms $(CFLAGS) -fo=$@ $< 

clocklac.obj:	clock.c 
	$(CC) -ml -DDESKACC $(CFLAGS) -fo=$@ $< 

clock-l.acc:	clocklac.obj
	$(LD) name $@ system dos file \{ ../start-l.obj $< \} lib ../../lib/wccgem-l.lib \
	      option map=clocklac.map \
	      order clname CODE segment BEGTEXT segment _TEXT clname FAR_DATA \
	      clname BEGDATA clname DATA clname BSS clname STACK clname EDATA

clocksac.obj:	clock.c 
	$(CC) -ms -DDESKACC $(CFLAGS) -fo=$@ $< 

clock-s.acc:	clocksac.obj
	$(LD) name $@ system dos file \{ ../start-s.obj $< \} lib ../../lib/wccgem-s.lib \
	      option map=clocksac.map \
	      order clname CODE segment BEGTEXT segment _TEXT clname FAR_DATA \
	      clname BEGDATA clname DATA clname BSS clname STACK clname EDATA

tidy:
	rm -f clock-?.exe
	rm -f clock-?.app
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err

clean:	tidy
