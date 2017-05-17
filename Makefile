#
# Makefile for socklab
#
# sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt

# Variables
#
CC	?= gcc

GITVERSION := $(shell git describe --tags --always --abbrev=7 --dirty --match "v[0-9]*" | cut -c 2-)

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

LDFLAGS	= -lreadline -lhistory
CFLAGS	= -Wall

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

# Adaptation a Linux
ifeq ($(shell uname), Linux)
LDFLAGS	+= -ltermcap
endif

# Adaptation a FreeBSD
# Attention : il faut utiliser gmake...
ifeq ($(shell uname),FreeBSD)
LDFLAGS	+= -ltermcap
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
	echo 'const char* socklab_version = "$(GITVERSION)";' > $@

#
# Nettoyage
#
.PHONY: clean force
clean :
	rm -f $(OBJS) *~ socklab

#
# Dependencies
#
$(OBJS): %.o: %.c %.h socklab.h tcp.h udp.h prim.h tools.h options.h
