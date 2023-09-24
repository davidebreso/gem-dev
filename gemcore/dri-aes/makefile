all: gem30.exe

CC=hc.exe
AS=rasm86.exe
LINK=link86.exe

INSTALLDIR=D:\GEMAPPS\GEMSYS

deps = \
    Basepage.h \
    Bind.h \
    Crysbind.h \
    Dos.h \
    Funcdef.h \
    Gem.h \
    Gemlib.h \
    Gsxdefs.h \
    Machine.h \
    Obdefs.h \
    Portab.h \
    Struct.h \
    Taddr.h

c_objs = \
    gemaplib.obj \
    gemasync.obj \
    gembase.obj \
    gemctrl.obj \
    gemdisp.obj \
    gemdos.obj \
    gemevlib.obj \
    gemflag.obj \
    gemfmalt.obj \
    gemfmlib.obj \
    gemfslib.obj \
    gemglobe.obj \
    gemgraf.obj \
    gemgrlib.obj \
    gemgsxif.obj \
    geminit.obj \
    geminput.obj \
    gemmnlib.obj \
    gemobed.obj \
    gemoblib.obj \
    gemobjop.obj \
    gempd.obj \
    gemqueue.obj \
    gemrslib.obj \
    gemsclib.obj \
    gemshlib.obj \
    gemsuper.obj \
    gemwmlib.obj \
    gemwrect.obj \
    optimize.obj \
    hcsmall.obj

asm_objs = \
    gemasm.obj \
    gemdosif.obj \
    gemgmain.obj \
    gemstart.obj \
    gsx2.obj \
    large.obj \
    optimopt.obj \
    proend.obj \
    rtlasm.obj

#
# MAIN EXECUTABLE
#
gem30.exe: $(c_objs) $(asm_objs)
	$(LINK) gem30[i

#
# C SOURCE OBJECTS
#
geminit.obj: geminit.c $(deps)
    $(CC) GEMINIT -tpages 150

gemsclib.obj: gemsclib.c $(deps)
    $(CC) GEMSCLIB -tpages 150

gemsuper.obj: gemsuper.c $(deps)
    $(CC) GEMSUPER -tpages 150

# Implicit rule with defaults
.c.obj:
    $(CC) $*

#
# ASM SOURCE OBJECTS
#
rtlasm.obj: rtlasm.a86
    $(AS) rtlasm $pz sz nc

# Implicit rule with defaults
.a86.obj:
    $(AS) $* $pzsz

# copy driver to INSTALLDIR
install:
	copy gem30.exe $(INSTALLDIR)\gem.exe

# cleanup
clean:
	del  *.obj
	del  *.lib
	del  *.bak
	del  *.map
