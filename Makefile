VERSION=0.0.1

TARGET=mx2000drv

SRCDIR = src
OBJDIR = build

CC = gcc
CFLAGS += -Wall -Wextra
CFLAGS += -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align
CFLAGS += -Wstrict-prototypes -Wwrite-strings -ftrapv
CFLAGS += -Wpadded
CFLAGS += -fsanitize=address
#CFLAGS += -march=native
#CFLAGS += -pthread
SFLAGS = -std=c89 -pedantic
INCLUDES = -I.
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

PREFIX ?= /usr
DEST = $(DESTDIR)$(PREFIX)/bin
MANDIR = $(DESTDIR)$(PREFIX)/share/man



all: CFLAGS += -O2
all: $(TARGET)

debug: CFLAGS += -O0 -g -DDEBUG
debug: $(TARGET)


#automatic recompile when makefile changes
$(OBJS): Makefile

#automatically creates build directory if it doesn't exist
dummy := $(shell test -d $(OBJDIR) || mkdir -p $(OBJDIR))


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(SFLAGS) $(INCLUDES) -c -o $@ $< $(LIBS)


#install:
#	install -D -m 755 $(TARGET) "$(LIBDIR)/$(TARGET)"
#	ln -srf "$(LIBDIR)/$(TARGET)" "$(LIBDIR)/$(TBASE)"
#	ln -srf "$(LIBDIR)/$(TARGET)" "$(LIBDIR)/$(SONAME)"

#uninstall:
#	$(RM) "$(LIBDIR)/$(TARGET)"
#	$(RM) "$(LIBDIR)/$(STARGET)"
#	$(RM) "$(LIBDIR)/$(TBASE)"
#	$(RM) "$(LIBDIR)/$(SONAME)"
#	$(RM) "$(INCDIR)/$(NAME).h"

test:
	$(CC) -o test test/*.c

clean:
	$(RM) $(OBJS) $(TARGET)

.PHONY: all debug clean install uninstall test
