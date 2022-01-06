include ../../makedefs.i

CFLAGS = -0 -os -ox -s -bcl=dos -i=../../include
CC = wcl
AS = wasm
LD = wlink

all:	../format.app ../format.rsc

../%.app:	%.app
	cp $< ..

../format.rsc:	format.rsc
	cp format.rsc ..

format.app:	format.c format.h bootsect.o
	$(CC) -ms $(CFLAGS) -fe=$@ -fm=format.map $< bootsect.o ../../lib/wccgem-s.lib

bootsect.o:	bootsect.asm
	$(AS) -bt=dos -0 $<
	
getdrvs.com:	getdrvs.o
	$(LD) sys dos com file $<
	
getdrvs.o:	getdrvs.asm
	$(AS) -bt=dos -0 $<

frmtdemo.exe:	frmtdemo.c frmt.o
	$(CC) -ms $(CFLAGS) -fe=$@ -fm $< frmt.o
	
frmt.o:	frmt.asm
	$(AS) -0 -bt=dos $<

install:	format.app format.rsc
	cp format.app format.rsc /Volumes/Retrocomputing/dos_share

tidy:
	rm -f *.bak
	rm -f *.sdb
	rm -f *.sym
	rm -f *.map
	rm -f *.o
	rm -f format.app
	rm -f getdrvs.com
	rm -f frmtdemo.exe
	rm -f *.err

clean:	tidy
