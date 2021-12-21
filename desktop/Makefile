INSTDIR=C:\GEMAPPS\GEMSYS

all:	desktop.app desktopl.app \
	icn2xml xml2icn desklo.icn deskhi.icn

desklo.icn:	desklo.xml xml2icn
	./xml2icn $< $@

deskhi.icn:	deskhi.xml xml2icn
	./xml2icn $< $@

icn2xml:	icn2xml.c
	gcc $< -o $@
	
icn2xml.exe:	icn2xml.c
	wcl -bt=dos -0 -ml $<

xml2icn:	xml2icn.c
	gcc $< -o $@

xml2icn.exe:	xml2icn.c
	wcl -bt=dos -0 -ml $<

desktop.app:	small/desktop.app
	cp small/desktop.app $@

desktopl.app:	large/desktop.app
	cp large/desktop.app $@

desksm.app:	smallm/desktop.app
	cp smallm/desktop.app $@

desklm.app:	largem/desktop.app
	cp largem/desktop.app $@

large/desktop.app:	*.c *.h
	$(MAKE) -C large

small/desktop.app:	*.c *.h
	$(MAKE) -C small

largem/desktop.app:	*.c *.h
	$(MAKE) -C largem

smallm/desktop.app:	*.c *.h
	$(MAKE) -C smallm

install:	desktop.app deskhi.icn desklo.icn
	cp deskhi.icn desklo.icn desktop.rsc $(INSTDIR)
	cp desktop.app $(INSTDIR)\desktop.app
	
clean:
	$(MAKE) -C small clean
	$(MAKE) -C large clean
	rm -f desktop.app 
	rm -f desktopl.app 
	rm -f icn2xml 
	rm -f icn2xml.exe 
	rm -f xml2icn 
	rm -f xml2icn.exe
	rm -f desklo.icn 
	rm -f deskhi.icn
	rm -f *.err
	
