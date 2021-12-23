include ../../makedefs.i

CFLAGS = -bt=dos -os -s -0 -i../../include
CC = wcc
LD = wlink

all:	../fmem-l.acc ../fmem-s.acc ../freemem.rsc

../%.acc:	%.acc
	cp $< ..

../%.rsc:	%.rsc
	cp $< ..


fmem-l.obj:	freemem.c 
	$(CC) -ml -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

fmem-l.acc:	fmem-l.obj
	$(LD) name $@ system dos file \{ ../start-l.obj $< \} lib ../../lib/wccgem-l.lib \
	      option map=fmem-l.map \
	      order clname CODE segment BEGTEXT segment _TEXT clname FAR_DATA \
	      clname BEGDATA clname DATA clname BSS clname STACK clname EDATA

fmem-s.obj:	freemem.c 
	$(CC) -ms -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

fmem-s.acc:	fmem-s.obj
	$(LD) name $@ system dos file \{ ../start-s.obj $< \} lib ../../lib/wccgem-s.lib \
	      option map=fmem-s.map \
	      order clname CODE segment BEGTEXT segment _TEXT clname FAR_DATA \
	      clname BEGDATA clname DATA clname BSS clname STACK clname EDATA

tidy:
	rm -f fmem-?.acc
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err

clean:	tidy
