include makedefs.i
RELEASE=1.0.2

TARGETS=lib/wccgem-l.lib lib/wccgem-s.lib samples

all:	$(TARGETS)

samples: lib/wccgem-l.lib lib/wccgem-s.lib
	$(MAKE) -C samples

lib/wccgem-l.lib lib/wccgem-s.lib:
	$(MAKE) -C libsrc

install:
	$(MAKE) -C include install
	$(MAKE) -C lib install

clean:
	rm -f wccgem$(RELEASE).zip
	$(MAKE) -C libsrc $@
	$(MAKE) -C samples $@
	$(MAKE) -C lib $@

tidy:
	$(MAKE) -C libsrc $@
	$(MAKE) -C samples $@
	$(MAKE) -C lib $@

release: $(TARGETS) tidy
	rm -f wccgem$(RELEASE).zip
	zip -r wccgem$(RELEASE).zip *