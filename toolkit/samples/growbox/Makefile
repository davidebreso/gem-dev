include ../../makedefs.i

CFLAGS = -bt=dos -os -s -0 -i../../include
CC = wcc
LD = wlink

all:	../growbx-l.acc ../growbx-s.acc

../%.acc:	%.acc
	cp $< ../

growbx-l.obj:	growbox.c 
	$(CC) -ml -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

growbx-l.acc:	growbx-l.obj
	$(LD) name $@ system dos file \{ ../start-l.obj $< ../main-l.obj \} lib ../../lib/wccgem-l.lib \
	      option map=growbx-l.map \
	      order clname CODE segment BEGTEXT segment _TEXT clname FAR_DATA \
	      clname BEGDATA clname DATA clname BSS clname STACK clname EDATA

growbx-s.obj:	growbox.c 
	$(CC) -ms -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

growbx-s.acc:	growbx-s.obj
	$(LD) name $@ system dos file \{ ../start-s.obj $< ../main-s.obj \} lib ../../lib/wccgem-s.lib \
	      option map=growbx-s.map \
	      order clname CODE segment BEGTEXT segment _TEXT clname FAR_DATA \
	      clname BEGDATA clname DATA clname BSS clname STACK clname EDATA

tidy:
	rm -f growbx-?.acc
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err

clean:	tidy
