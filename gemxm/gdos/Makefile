INSTDIR=c:\gemapps\gemsys

OBJECTS=ch.obj chasm.obj cutasm.obj cxc33.obj dosasm.obj doscut.obj \
	dospaste.obj equates.obj gdos.obj gemdos.obj ints.obj large.obj \
	load.obj mn.obj stealkbd.obj sw.obj tblparse.obj

all:	gemvdi.exe

gemvdi.exe:	$(OBJECTS)
	link86 gemvdi[nop]=gdos,ch,chasm,sw,mn,cutasm,doscut,dospaste,\
			tblparse,stealkbd,gemdos,dosasm,large,cxc33,ints,load

%.obj:	%.a86
	rasm86 $< $$szpz

%.obj:	%.c
	lc1 $< -s
	lc2 $< -scode -v

install: gemvdi.exe
	copy gemvdi.exe $(INSTDIR)

clean:
	del *.obj
	del *.sym
	del gemvdi.exe

