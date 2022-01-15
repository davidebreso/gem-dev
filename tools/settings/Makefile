
OBJECTS = settings.obj theme.obj windlg.obj main.obj setutil.obj clrdlg.obj \
          popcolou.obj popmenu.obj guidlg.obj

CC = pacc
CFLAGS = -Bs -O

all:	settings.app

settings.app:	settings.exe
	copy settings.exe settings.app

settings.exe:	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) ppdgem-s.lib

%.obj:	%.c
	$(CC) $(CFLAGS) -c $<
