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
SFLAGS = -std=gnu89 -pedantic
INCLUDES = -I.
LIBS = -lusb-1.0
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

PREFIX ?= /usr
DEST = $(DESTDIR)$(PREFIX)/bin
MANDIR = $(DESTDIR)$(PREFIX)/share/man

RULES = 99-mx2000.rules

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


install:
	install -D -m 755 $(TARGET) "$(DEST)/$(TARGET)"
	install -D -m 644 udev/$(RULES) "$(DESTDIR)/etc/udev/rules.d/$(RULES)"

uninstall:
	$(RM) "$(DEST)/$(TARGET)"
	$(RM) "$(DESTDIR)/etc/udev/rules.d/$(RULES)"

test:
	$(CC) -o test test/*.c

clean:
	$(RM) $(OBJS) $(TARGET)

.PHONY: all debug clean install uninstall test
