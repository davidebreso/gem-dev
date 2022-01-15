
CFLAGS=-bt=dos -os -s -0 
CC=wcc
LD=wlink
AS=wasm

OBJECTS1=gemstart.obj geminit.obj gemgmain.obj gemasync.obj gemctrl.obj gembase.obj 
OBJECTS2=gemdisp.obj gemflag.obj geminput.obj gempd.obj gemqueue.obj gemwrect.obj 
OBJECTS4=gemsuper.obj gemaplib.obj gemevlib.obj gemmnlib.obj gemoblib.obj 
OBJECTS5=gemfmalt.obj gemfmlib.obj gemsclib.obj gemwmlib.obj  
OBJECTS6=gemrslib.obj gemgrlib.obj gemgsxif.obj gempplib.obj gemasm.obj gemcicon.obj 
OBJECTS7=xstate.obj debug.obj gemgraf.obj gsx2.obj optimize.obj large.obj gemdos.obj 
OBJECTS8=gemobjop.obj gemdosif.obj gememsif.obj optimopt.obj gemglobe.obj proend.obj
OBJECTS9=gemobed.obj gemfslib.obj gemshlib.obj

OBJECTS=$(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4) $(OBJECTS5) $(OBJECTS6) \
        $(OBJECTS7) $(OBJECTS8) $(OBJECTS9) $(OBJECTS0)

all:	ppdaes.exe

wccaes.exe: $(OBJECTS)
	$(LD) name $@ system dos file \{ $(OBJECTS) \} option map
			
%.obj:	%.c
	$(CC) $(CFLAGS) -fo=$@ $<
	
%.obj:	%.as
	$(AS) -bt=dos -0 -ms $<

clean:
	rm -f *.obj
	rm -f *.map
	rm -f *.err
	del ppdaes.exe

binzip:
	$(ZIP) aes30.zip gem.exe gem.rsc readme.doc license.txt
	$(ZIP) aes30.zip gem3d.exe settings.app gemset.rsc *.cfg

srczip:
	$(ZIP) aes30s.zip Makefile *.h *.c *.as gem.rsc gem.dfn getldiv.bat 
	$(ZIP) aes30s.zip ppdaes.prj build.doc readme.doc license.txt
	$(ZIP) aes30s.zip settings.zip