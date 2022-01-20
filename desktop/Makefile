INSTDIR=$(HOME)/Dosbox/c_drive/GEMAPPS/GEMSYS
RELEASE=3.2.6

all:	desktop.app desktopl.app desksm.app desklm.app \
	nop.exe icn2xml xml2icn desklo.icn deskhi.icn

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
	
nop.exe:	nop.obj
	wlink system dos name $@ file $<
	
nop.obj:	nop.asm
	wasm -bt=dos -0 -fo=$@ $<

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

install:	desktop.app deskhi.icn desklo.icn desktop.rsc skeleton.inf nop.exe
	cp deskhi.icn desklo.icn desktop.rsc skeleton.inf nop.exe $(INSTDIR)
	cp desktop.app $(INSTDIR)/desktop.app

xminstall:	desksm.app deskhi.icn desklo.icn desktop.rsc skeleton.inf
	cp deskhi.icn desklo.icn desktop.rsc skeleton.inf $(INSTDIR)
	cp desksm.app $(INSTDIR)/desktop.app

release:	desktop.app desktopl.app desksm.app desklm.app \
			deskhi.icn desklo.icn desktop.rsc nop.exe \
			skeleton.inf readme.txt copying.txt
	rm -f wccdesk$(RELEASE).zip
	zip -9 wccdesk$(RELEASE).zip $^
	
sourcerel:	*.c *.h *.asm *.rsc *.dfn *.txt *.inf *.xml \
			makedefs.i Makefile */Makefile
	rm -f wccdesk$(RELEASE)_src.zip
	zip -9 wccdesk$(RELEASE)_src.zip $^

clean:
	$(MAKE) -C small clean
	$(MAKE) -C large clean
	$(MAKE) -C smallm clean
	$(MAKE) -C largem clean
	rm -f desktop.app 
	rm -f desktopl.app 
	rm -f icn2xml 
	rm -f icn2xml.exe 
	rm -f xml2icn 
	rm -f xml2icn.exe
	rm -f desklo.icn 
	rm -f deskhi.icn
	rm -f nop.exe
	rm -f *.obj
	rm -f *.err
	rm -f wccdesk*.zip
	
