
CC=PACC
CFLAGS=-Bl -Zg -I.. -DMULTIAPP
LIBDIR=d:/tools/c/lib

OBJECTS0=desktop.obj  
OBJECTS1=desk1.obj    deskact.obj  deskfun.obj  deskwin.obj 
OBJECTS2=deskpro.obj  deskwcc.obj  deskobj.obj  deskfpd.obj  deskins.obj 
OBJECTS3=deskapp.obj deskgraf.obj deskrsrc.obj 
OBJECTS4=deskosif.obj deskinf.obj  deskdir.obj  deskgsx.obj  main.obj 
OBJECTS5=desktype.obj desksupp.obj deskcds.obj  optimize.obj int3.obj
OBJECTS6=deskiacc.obj

OBJECTS=$(OBJECTS0) $(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4) \
	$(OBJECTS5) $(OBJECTS6)

all:	desktop.app

desktop.app:	desktop.exe
	copy $< $@

desktop.exe:	$(OBJECTS) 
	libr r tmp.lib $(OBJECTS1)
	libr r tmp.lib $(OBJECTS2)
	libr r tmp.lib $(OBJECTS3)
	libr r tmp.lib $(OBJECTS4)
	libr r tmp.lib $(OBJECTS5)
	libr r tmp.lib $(OBJECTS6)
	$(CC) -Bl $(OBJECTS0) tmp.lib $(LIBDIR)/wccgem-l.lib
	del tmp.lib

%.obj:	../%.c ../deskprot.h ../desktop.h ../wccdesk.h
	$(CC) -O$@ $(CFLAGS) -c $<

%.obj:	../%.as
	$(CC) -O$@ $(CFLAGS) -c $<	

