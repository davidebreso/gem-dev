include ../../makedefs.i

#
# nb: Low warning level, so that the RCS-generated file EXAMPLE.RSH 
# doesn't cause warnings
#
CFLAGS = -bt=dos -os -s -0 -w=0 -i../../include
CC = wcc
LD = wlink

all:	 ../rscr-s.exe ../rscr-l.exe

../%.exe:	%.exe
	cp $< ..

rscr-s.exe:	rscr-s.obj data-s.obj
	$(LD) name $@ system dos file \{ cstart.o $< data-s.obj \} lib ../../lib/wccgem-s.lib option map 
#	order clname CODE clname FAR_DATA clname BEGDATA clname RSCDATA segment RSCHDR segment RSCDATA

rscr-s.obj:	rscreate.c example.rsh
	$(CC) -ms $(CFLAGS) -fo=$@ $<	
	
data-s.obj: rscdata.c example.rsh
	$(CC) -ms $(CFLAGS) -fo=$@ $<	

rscr-l.exe:	rscr-l.obj data-l.obj
	$(LD) name $@ system dos file \{ $< data-l.obj \} lib ../../lib/wccgem-l.lib option map 

rscr-l.obj:	rscreate.c example.rsh
	$(CC) -ml $(CFLAGS) -fo=$@ $<	

data-l.obj: rscdata.c example.rsh
	$(CC) -ml $(CFLAGS) -fo=$@ $<	


tidy:
	rm -f rscr-?.exe
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.obj
	rm -f *.err

clean:	tidy
