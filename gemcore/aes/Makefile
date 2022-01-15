
CFLAGS=-bt=dos -os -s -0
CC=wcc
LD=wlink
AS=wasm

OBJECTS0=gemstart.obj gemgmain.obj 
OBJECTS=geminit.obj gemasync.obj gemctrl.obj gembase.obj gemdisp.obj gemflag.obj \
		geminput.obj gempd.obj gemqueue.obj gemwrect.obj gemsuper.obj gemaplib.obj \
		gemevlib.obj gemmnlib.obj gemoblib.obj gemfmalt.obj gemfmlib.obj gemsclib.obj \
		gemwmlib.obj gemrslib.obj gemgrlib.obj gemgsxif.obj gempplib.obj  gemcicon.obj \
		xstate.obj debug.obj gemgraf.obj  optimize.obj  gemdos.obj gemobjop.obj \
		gemglobe.obj gemobed.obj gemfslib.obj gemshlib.obj
ASMOBJS=gemasm.obj gsx2.obj large.obj gemdosif.obj gememsif.obj \
 		optimopt.obj proend.obj

all:	wccaes.exe

wccaes.exe: $(OBJECTS)
	$(LD) name $@ system dos file \{ $(OBJECTS) \} option map
			
%.obj:	%.c
	$(CC) $(CFLAGS) -fo=$@ $<
	
%.obj:	%.asm
	$(AS) -bt=dos -0 -ms $<

clean:
	rm -f *.obj
	rm -f *.map
	rm -f *.err
	rm -f wccaes.exe

binzip:
	$(ZIP) aes30.zip gem.exe gem.rsc readme.doc license.txt
	$(ZIP) aes30.zip gem3d.exe settings.app gemset.rsc *.cfg

srczip:
	$(ZIP) aes30s.zip Makefile *.h *.c *.as gem.rsc gem.dfn getldiv.bat 
	$(ZIP) aes30s.zip ppdaes.prj build.doc readme.doc license.txt
	$(ZIP) aes30s.zip settings.zip