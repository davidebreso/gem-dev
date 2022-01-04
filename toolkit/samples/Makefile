
include ../makedefs.i

TARGETS=demo-s.app   demo-l.app   \
	clock-s.app  clock-l.app  \
	clock-s.acc  clock-l.acc  \
	xform-s.app  xform-l.app  \
	hello-s.app  hello-l.app  \
	hello-s.acc  hello-l.acc  \
	growbx-s.acc growbx-l.acc \
	fdtest-s.app fdtest-l.app \
	events-s.app events-l.app \
	spool-s.app  spool-l.app  \
	udef-s.app   udef-l.app   \
	fmem-s.acc   fmem-l.acc   \
	mgraf-s.app  mgraf-l.app  \
	mgraf-s.acc  mgraf-l.acc  \
	format.app   rscr-s.exe

CC = wcc
AS = wasm
CFLAGS = -bt=dos -os -s -0 -i../include

MAIN = main-l.obj main-s.obj

STARTUP = start-s.obj start-l.obj

all: $(TARGETS)

main: $(MAIN)

startup: $(STARTUP)

main-l.obj:	main.c
	$(CC) -fo=$@ -ml $(CFLAGS) $<

main-s.obj:	main.c
	$(CC) -fo=$@ -ms $(CFLAGS) $<

start-l.obj:	accstart.asm
	$(AS) -DSTACKSIZ=800h -fo=$@ -ml -bt=dos -0 $<

start-s.obj:	accstart.asm
	$(AS) -DSTACKSIZ=800h -fo=$@ -ms -bt=dos -0 $<

demo-s.app demo-l.app:	$(MAIN)
	$(MAKE) -C demo

clock-s.app clock-l.app clock-s.acc clock-l.acc: $(MAIN) $(STARTUP)
	$(MAKE) -C clock

events-s.app events-l.app: $(MAIN)
	$(MAKE) -C events

fdtest-s.app fdtest-l.app: $(MAIN)
	$(MAKE) -C fdtest

spool-s.app spool-l.app: $(MAIN)
	$(MAKE) -C spooler

udef-s.app udef-l.app: $(MAIN)
	$(MAKE) -C userdef


hello-s.app hello-l.app hello-s.acc hello-l.acc: $(MAIN) $(STARTUP)
	$(MAKE) -C hello

mgraf-s.app mgraf-l.app mgraf-s.acc mgraf-l.acc:  $(MAIN) $(STARTUP)
	$(MAKE) -C memgraf

growbx-s.acc growbx-l.acc: $(MAIN) $(STARTUP)
	$(MAKE) -C growbox

fmem-s.acc fmem-l.acc: $(MAIN) $(STARTUP)
	$(MAKE) -C freemem

rscr-s.exe: $(MAIN)
	$(MAKE) -C rscreate

xform-s.app xform-l.app: $(MAIN)
	$(MAKE) -C xform

format.app: $(MAIN)
	$(MAKE) -C format

tidy:
	rm -f main-?.obj
	rm -f start-?.obj
	rm -f *.bak
	$(MAKE) -C hello tidy
	$(MAKE) -C demo tidy
	$(MAKE) -C xform tidy
	$(MAKE) -C growbox tidy
	$(MAKE) -C clock tidy
	$(MAKE) -C events tidy
	$(MAKE) -C freemem tidy
	$(MAKE) -C memgraf tidy
	$(MAKE) -C fdtest tidy
	$(MAKE) -C spooler tidy
	$(MAKE) -C userdef tidy
	$(MAKE) -C rscreate tidy
	$(MAKE) -C format tidy

clean:
	rm -f main-?.obj
	rm -f start-?.obj
	rm -f *.rsc
	rm -f *.app
	rm -f *.acc
	rm -f *.exe
	rm -f *.bak
	rm -f *.err
	$(MAKE) -C hello clean
	$(MAKE) -C demo clean
	$(MAKE) -C xform clean
	$(MAKE) -C growbox clean
	$(MAKE) -C clock clean
	$(MAKE) -C events clean
	$(MAKE) -C freemem clean
	$(MAKE) -C memgraf clean
	$(MAKE) -C fdtest clean
	$(MAKE) -C spooler clean
	$(MAKE) -C userdef clean
	$(MAKE) -C rscreate clean
	$(MAKE) -C format clean

# Disable built-in rules	
.SUFFIXES:


