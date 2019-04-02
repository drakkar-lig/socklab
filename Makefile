#
# Makefile for socklab
#
# sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt

# Variables
#
CC	?= gcc

# This is only used when building from tarballs (update this when adding a new tag!)
VERSION := 1.1.0
# When building from the git repository, this is used instead.
# The rev-parse hack is here to detect whether we are at the root of the git
# repository.  If that is not the case, it means that the socklab sources are
# embedded in an unrelated git repository, so don't use a git tag.
GITVERSION := $(shell [ -z "$(git rev-parse --show-cdup)" ] && git describe --tags --abbrev=7 --dirty --match "v[0-9]*" 2> /dev/null | cut -c 2-)

ifneq ($(GITVERSION),)
	VERSION := $(GITVERSION)
endif

prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
mandir = ${datarootdir}/man
bindir = ${exec_prefix}/bin

SOURCES = \
socklab.c \
prim.c \
options.c \
tools.c \
tcp.c \
udp.c \
version.c

TARGETS=socklab
OBJS	= $(SOURCES:.c=.o)

# Let packagers define LDFLAGS and CFLAGS if they want to, but add our own flags
LDFLAGS := -lreadline -lhistory $(LDFLAGS)
CFLAGS  := -Wall $(CFLAGS)

# Adaptation a Darwin / MacOS X
ifeq ($(shell uname), Darwin)
# test for homebrew installed readline
ifneq ("$(wildcard /usr/local/opt/readline/lib)","")
CPPFLAGS+=-I/usr/local/opt/readline/include
LDFLAGS +=-L/usr/local/opt/readline/lib
else
# OSX readline does not support invisible characters in prompt
# http://stackoverflow.com/questions/31329952
CPPFLAGS+=-DOSX_READLINE
LDFLAGS	= -lreadline
endif
endif

# Adaptation a FreeBSD
# Attention : il faut utiliser gmake...
ifeq ($(shell uname),FreeBSD)
CPPFLAGS+=-I/usr/local/include
LDFLAGS +=-L/usr/local/lib
endif

# Adaptation a Solaris
# Attention : il faut que la lib. readline soit dans le repertoire readline...
ifeq ($(shell uname),SunOS)
ifneq ("$(wildcard ../readline/readline.h)","")
CPPFLAGS+= -I..
LDFLAGS	= -L../readline -ltermcap -lsocket -lnsl
else
$(error readline needed in parent directory)
endif
endif

#
# Objectifs du makefile:
#
all: $(TARGETS)

socklab: $(OBJS)

version.c: force
	echo 'const char* socklab_version = "$(VERSION)";' > $@

install: socklab man1/socklab.1
	install -Dm755 socklab $(DESTDIR)$(bindir)/socklab
	install -Dm644 man1/socklab.1 $(DESTDIR)$(mandir)/man1/socklab.1

#
# Nettoyage
#
.PHONY: clean install force
clean :
	rm -f $(OBJS) *~ socklab

#
# Dependencies
#
$(OBJS): %.o: %.c %.h socklab.h tcp.h udp.h prim.h tools.h options.h
