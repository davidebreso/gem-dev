include ../../makedefs.i

CFLAGS = -bt=dos -os -s -0 -i../../include
CC = wcc
LD = wlink

all:	../hello-l.app ../hello-s.app ../hello-l.acc ../hello-s.acc 

../%.app:	%.app
	cp $< ..

../%.acc:	%.acc
	cp $< ..

hello-l.app:	hello-l.obj
	$(LD) name $@ system dos file \{ $< ../main-l.obj ../../lib/wccgem-l.lib \}
	
hello-l.obj:	hello.c
	$(CC) -ml $(CFLAGS) -fo=$@ $<	

hello-s.app:	hello-s.obj
	$(LD) name $@ system dos file \{ $< ../main-s.obj ../../lib/wccgem-s.lib \}

hello-s.obj:	hello.c
	$(CC) -ms $(CFLAGS) -fo=$@ $<	

hello-l.acc:	hellolac.obj
	$(LD) name $@ system dos file \{ $< ../main-l.obj ../../lib/wccgem-l.lib \}

hellolac.obj:	hello.c 
	$(CC) -ml -DDESKACC=1 $(CFLAGS) -fo=$@ $< 
	
hello-s.acc:	hellosac.obj
	$(LD) name $@ system dos file \{ $< ../main-s.obj ../../lib/wccgem-s.lib \}

hellosac.obj:	hello.c 
	$(CC) -ms -DDESKACC=1 $(CFLAGS) -fo=$@ $< 

tidy:
	rm -f hello-?.exe
	rm -f hello-?.app
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err

clean:	tidy
