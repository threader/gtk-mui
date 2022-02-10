# $Id: Makefile,v 1.5 2006/03/01 14:58:27 itix Exp $

CC = gcc -noixemul
CDEFS  = -DUSE_INLINE_STDARG -DAROS_ALMOST_COMPATIBLE
CFLAGS = -O3 -Wall -I../
TARGET = gtkmuippc.library

OBJS  = Library.o

ECHO = echo
ECHE = echo -e
BOLD = \033[1m
NRML = \033[22m

COMPILING = @$(ECHE) "compiling $(BOLD)$@$(NRML)..."
LINKING = @$(ECHE) "linking $(BOLD)$@$(NRML)..."
STRIPPING = @$(ECHE) "stripping $(BOLD)$@$(NRML)..."
ARCHIVING = @$(ECHE) "archiving $(BOLD)$@$(NRML)..."

all: linklib $(TARGET) libgtk.a

Library.o: Library.h

%.o: %.c
	$(COMPILING)
	@$(CC) $(CFLAGS) $(CDEFS) -o $@ -c $*.c

libgtk-mui.o: libgtk-mui.c
	$(COMPILING)
	@$(CC) $(CFLAGS) $(CDEFS) -o $@ -c $*.c

gtkmuippc-stubs.o: gtkmuippc-stubs.c
	$(COMPILING)
	@$(CC) $(CFLAGS) $(CDEFS) -o $@ -c $*.c

../libgtk-mui.a:
	@cd ..; make -f makefile.morphos

$(TARGET): $(OBJS) ../libgtk-mui.a
	$(LINKING)
	@$(CC) -nostartfiles $(OBJS) -o $(TARGET).db -Wl,-Map=gtkmuippc.map -L../ -lgtk-mui -ldebug
	$(STRIPPING)
	@strip --remove-section=.comment $(TARGET).db -o $(TARGET)

headers:
	@-mkdir -p include/proto include/ppcinline include/inline
	cvinclude.pl --fd include/fd/gtk-mui_lib.fd --clib include/clib/gtk-mui_protos.h --proto include/proto/gtkmuippc.h --inlines include/ppcinline/gtkmuippc.h --vbccinlines include/inline/gtkmuippc_protos.h --root=gtkmuippc

libgtk.a: libgtk-mui.o
	$(ARCHIVING)
	@-rm -f $@
	@cvinclude.pl --fd=include/fd/gtk-mui_lib.fd --clib=include/clib/gtk-mui_protos.h --gluelib=libgtk.a
	@$(AR) cru $@ libgtk-mui.o
	@ppc-morphos-ranlib $@

libgtk-mui_glue.a: include/clib/gtk-mui_protos.h include/fd/gtk-mui_lib.fd
	$(COMPILING)
	@cvinclude.pl --fd=include/fd/gtk-mui_lib.fd --clib=include/clib/gtk-mui_protos.h --gluelib=libgtk-mui_glue.a

linklib:
	@cd ..; if ! $(MAKE) -f Makefile.MorphOS; then exit 1; fi;

install: linklib $(TARGET)
	@copy $(TARGET) LIBS:
	@Avail FLUSH >NIL:
	@$(ECHE) "Installed."

dump:
	objdump --disassemble-all --reloc $(TARGET).db >$(TARGET).s

clean: 
	rm -f *.o $(TARGET) *.library include/proto/* include/ppcinline/* include/inline/*
	rmdir include/proto include/ppcinline include/inline
