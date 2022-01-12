#
# Where is Open Watcom installed on your system?
#
export WATCOM := /opt/watcom
export PATH := $(PATH):$(WATCOM)/osx64
export INCLUDE := $(WATCOM)/h

#
# Where is GEM Developer's kit installed on your system?
#
GDKDIR=$(HOME)/Sources/gem-dev/toolkit
