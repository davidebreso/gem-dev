include makedefs.i

TARGETS=lib/wccgem-l.lib lib/wccgem-s.lib samples

all:	$(TARGETS)

samples: lib/wccgem-l.lib lib/wccgem-s.lib
	$(MAKE) -C samples

lib/wccgem-l.lib lib/wccgem-s.lib:
	$(MAKE) -C libsrc

install:
	$(MAKE) -C include install
	$(MAKE) -C lib install
	$(MAKE) -C bin install

clean:
	$(MAKE) -C libsrc $@
	$(MAKE) -C samples $@
	$(MAKE) -C lib $@

tidy:
	$(MAKE) -C libsrc $@
	$(MAKE) -C samples $@
	$(MAKE) -C lib $@

zip:
	rm -f wccgem.zip
	zip -9 -r wccgem.zip *