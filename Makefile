VERSION=0.5.0

TARGET=mx2000drv

SRCDIR = src
OBJDIR = build

CC = gcc
CFLAGS += -Wall -Wextra
CFLAGS += -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align
CFLAGS += -Wstrict-prototypes -Wwrite-strings -ftrapv -Wpadded
CFLAGS += -fsanitize=address
#CFLAGS += -march=native
CFLAGS += -DVERSION=\"$(VERSION)\"
SFLAGS = -std=c99 -pedantic
INCLUDES = -I.
#LIBS = -lhidapi-libusb
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

PREFIX ?= /usr
BINDIR = $(DESTDIR)$(PREFIX)/bin
MANDIR = $(DESTDIR)$(PREFIX)/share/man/man1
BSHDIR = $(DESTDIR)$(PREFIX)/share/bash-completion/completions
ZSHDIR = $(DESTDIR)$(PREFIX)/share/zsh/site-functions
UDVDIR = $(DESTDIR)/etc/udev/rules.d


ifeq ($(OS),Windows_NT)
	LIBS = 
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		LIBS = -lhidapi
		BSHDIR = $(DESTDIR)$(PREFIX)/etc/bash_completion.d
	else
		LIBS = -lhidapi-libusb # or could use -lhidapi-hidraw
	endif
endif


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
	$(CC) $(CFLAGS) $(SFLAGS) $(INCLUDES) -c -o $@ $<


install:
	install -d -m 755 "$(BINDIR)"
	install -d -m 755 "$(MANDIR)"
	install -d -m 755 "$(UDVDIR)"
	install -d -m 755 "$(BSHDIR)"
	install -m 755 $(TARGET) "$(BINDIR)/$(TARGET)"
	install -m 644 doc/$(TARGET).1 "$(MANDIR)/$(TARGET).1"
	install -m 644 extra/$(RULES) "$(UDVDIR)/$(RULES)"
	install -m 644 extra/bash_completion "$(BSHDIR)/$(TARGET)"
	#install -m 644 extra/zsh_completion "$(ZSHDIR)/_$(TARGET)"

uninstall:
	$(RM) "$(BINDIR)/$(TARGET)"
	$(RM) "$(UDVDIR)/$(RULES)"
	$(RM) "$(MANDIR)/$(TARGET).1"
	$(RM) "$(BSHDIR)/$(TARGET)"
	#$(RM) "$(ZSHDIR)/_$(TARGET)"

test:
	$(CC) -o test test/*.c

doc:
	a2x -v -d manpage -f manpage -a revnumber=$(VERSION) doc/$(TARGET).1.txt

clean:
	$(RM) $(OBJS) $(TARGET)

.PHONY: all debug clean install uninstall test doc
