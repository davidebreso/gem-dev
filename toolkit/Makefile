include makedefs.i

TARGETS=lib/wccgem-l.lib lib/wccgem-s.lib bin/exe2acc.exe samples

all:	$(TARGETS)

samples: bin/exe2acc.exe
	$(MAKE) -C samples

lib/wccgem-l.lib lib/wccgem-s.lib:
	$(MAKE) -C libsrc

bin/exe2acc.exe:
	$(MAKE) -C exe2acc

install:
	$(MAKE) -C include install
	$(MAKE) -C lib install
	$(MAKE) -C bin install

clean:
	$(MAKE) -C libsrc $@
	$(MAKE) -C samples $@
	$(MAKE) -C exe2acc $@
	$(MAKE) -C bin $@
	$(MAKE) -C lib $@

tidy:
	$(MAKE) -C libsrc $@
	$(MAKE) -C samples $@
	$(MAKE) -C exe2acc $@
	$(MAKE) -C bin $@
	$(MAKE) -C lib $@

zip:
	rm -f wccgem.zip
	zip -9 -r wccgem.zip *