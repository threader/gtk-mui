
TARGET=eurocalc
CFLAGS = -g -O3 -Wall -I/usr/local/amiga/i686-aros/include/glib-2.0/glib -I/usr/local/amiga/i686-aros/include/glib-2.0 -I/usr/local/amiga/i686-aros/lib/glib-2.0/include -I../..

LIBS= -lgtk-mui -lgobject-2.0 -lglib-2.0 -liconv -lmui -larossupport -lamiga -larosc  -larosm


EURO_OBJS = \
	main.o \
	support.o \
	interface.o \
	callbacks.o 

all: $(OBJS) $(EURO_OBJS)
	$(CC) -g $(EURO_OBJS) $(OBJS) -o $(TARGET) -L../.. $(LIBS)

clean: 
	rm -f $(EURO_OBJS) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c


