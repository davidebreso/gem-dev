#
# Where is Open Watcom installed on your system?
#
export WATCOM := /opt/watcom
export PATH := $(PATH):$(WATCOM)/osx64
export INCLUDE := $(WATCOM)/h
#
# Where do you want to install the GDK ?
#
INSTDIR=$(HOME)/watcom