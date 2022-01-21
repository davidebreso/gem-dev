INSTDIR=../toolkit/tools

TARGETS=rcs.app rcs.rsc rcslow.rsc \
		edicon.app edicon.rsc edicon.doc \
		rsconv.app rsconv.rsc \
		sysfont.app sysfont.rsc sysfont.txt sysfont.ico sysfdoc.ico sdpsc9.vga \

all: $(TARGETS)

rcs.app rcs.rsc rcslow.rsc:
	$(MAKE) -C rcs
	
edicon.app edicon.rsc edicon.doc:
	$(MAKE) -C edicon

rsconv.app rsconv.rsc:
	$(MAKE) -C rsconv

sysfont.app sysfont.rsc sysfont.txt sysfont.ico sysfdoc.ico sdpsc9.vga:
	$(MAKE) -C sysfont

install: $(TARGETS) license.txt
	cp $^ $(INSTDIR)
	
release: $(TARGETS) license.txt
	rm -f gemtools.zip
	zip gemtools.zip $(TARGETS) license.txt

sourcerel: license.txt clean
	rm -f gemtools_src.zip
	zip -r gemtools_src.zip *

tidy:
	rm -f *.bak
	rm -f *.err
	$(MAKE) -C rcs tidy
	$(MAKE) -C edicon tidy
	$(MAKE) -C rsconv tidy
	$(MAKE) -C sysfont tidy

clean:
	rm -f $(TARGETS)
	rm -f *.bak
	rm -f *.err
	rm -f *.zip
	$(MAKE) -C rcs clean
	$(MAKE) -C edicon clean
	$(MAKE) -C rsconv clean
	$(MAKE) -C sysfont clean

# Disable built-in rules
.SUFFIXES:


