#
# small Makefile to build gpassutil for AROS
#
# CC must be AROS cross compiler
#
# Oliver Brunner 2005
#
# $Id: Makefile,v 1.2 2011/07/04 12:16:21 o1i Exp $
#
TARGET=gpass
CFLAGS=-I/usr/local/amiga/i686-aros/include/glib-2.0/glib -I/usr/local/amiga/i686-aros/include/glib-2.0 -I/usr/local/amiga/i686-aros/lib/glib-2.0/include -I../.. 
STATICLIBS = -lgtk-mui -lgobject-2.0 -lglib-2.0 -liconv -lmui -larossupport -lamiga -larosc  -larosm


OBJS = \
	main.o \
	support.o \
	interface.o \
	callbacks.o 

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

all: $(OBJS) $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -L../.. $(STATICLIBS)

clean: 
	rm -f $(OBJS) $(TARGET)

