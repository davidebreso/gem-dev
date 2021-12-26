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
	$(LD) name $@ system dos file \{ $< ../main-l.obj \} libfile ../start-l.obj lib ../../lib/wccgem-l.lib \
	      option map=growbx-l.map 

growbx-s.obj:	growbox.c 
	$(CC) -ms -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

growbx-s.acc:	growbx-s.obj
	$(LD) name $@ system dos file \{ $< ../main-s.obj \} libfile ../start-s.obj lib ../../lib/wccgem-s.lib \
	      option map=growbx-s.map 
tidy:
	rm -f growbx-?.acc
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err

clean:	tidy
