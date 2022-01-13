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
	$(LD) name $@ system dos file \{ $< \} lib ../../lib/wccgem-l.lib option map

clock-l.obj:	clock.c
	$(CC) -ml $(CFLAGS) -fo=$@ $<

clock-s.app:	clock-s.obj
	$(LD) name $@ system dos file \{ $< \} lib ../../lib/wccgem-s.lib option map

clock-s.obj:	clock.c
	$(CC) -ms $(CFLAGS) -fo=$@ $< 

clocklac.obj:	clock.c 
	$(CC) -ml -DDESKACC $(CFLAGS) -fo=$@ $< 

clock-l.acc:	clocklac.obj
	$(LD) name $@ system dos libfile ../start-l.obj file \{ $< \} lib ../../lib/wccgem-l.lib \
	      option map=clocklac.map \

clocksac.obj:	clock.c 
	$(CC) -ms -DDESKACC $(CFLAGS) -fo=$@ $< 

clock-s.acc:	clocksac.obj
	$(LD) name $@ system dos libfile ../start-s.obj file \{ $< \} lib ../../lib/wccgem-s.lib \
	      option map=clocksac.map \

tidy:
	rm -f clock-?.acc
	rm -f clock-?.app
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err

clean:	tidy
