include ../../makedefs.i

#
# nb: Low warning level, so that the RCS-generated file EXAMPLE.RSH 
# doesn't cause warnings
#
CFLAGS = -bt=dos -os -s -0 -i../../include
CC = wcc
LD = wlink

all:	 ../rscreate.exe

../%.exe:	%.exe
	cp $< ..

rscreate.exe:	rscreate.obj 
	$(LD) name $@ system dos file \{ $< \} lib ../../lib/wccgem-l.lib option map 

rscreate.obj:	rscreate.c example.rsh
	$(CC) -ml $(CFLAGS) -fo=$@ $<	
	

tidy:
	rm -f rscreate.exe
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err

clean:	tidy
