INSTDIR = c:\gemapps\gemsys

OBJECTS = chasm.obj foo.obj gemaplib.obj gemasm.obj gemasync.obj gembase.obj \
          gemctrl.obj gemdisp.obj gemdos.obj gemdosif.obj gemevlib.obj \
          gemflag.obj gemfmalt.obj gemfmlib.obj gemfslib.obj gemglobe.obj \
          gemgmain.obj gemgraf.obj gemgrlib.obj gemgsxif.obj geminit.obj \
          geminput.obj gemmnlib.obj gemnulp.obj gemobed.obj gemobjop.obj \
          gemoblib.obj gempd.obj gemprlib.obj gemqueue.obj gemrslib.obj \
          gemsclib.obj gemshlib.obj gemstart.obj gemsuper.obj gemwmlib.obj \
          gemwrect.obj gsx2.obj large.obj optimize.obj optimopt.obj proend.obj \


all:	gem.exe

gem.exe:	$(OBJECTS)
	link86 gem30[i

%.obj:	%.a86
	rasm86 $< $$pz

%.obj:	%.c
	lc1 $< -s
	lc2 $< -scode -v
	
install: gem.exe
	copy gem.exe $(INSTDIR)
	
clean:
	del *.obj
	del *.sym
	del gem.exe
