#
# Makefile for socklab
#
# sous Licence CeCILL V2, voir le fichier Licence_CeCILL_V2-fr.txt

# Variables
#
CC	?= gcc
OBJS	= tcp.o udp.o tools.o socklab.o prim.o options.o 
OPTIONS	= -lreadline -lhistory
LFLAGS	=
CFLAGS	= -Ofast -Wall

# Adaptation a Darwin / MacOS X
ifeq ($(shell uname), Darwin)
OPTIONS	= -lreadline
endif

# Adaptation a Linux
ifeq ($(shell uname), Linux)
OPTIONS	+= -ltermcap
endif

# Adaptation a FreeBSD
# Attention : il faut utiliser gmake...
ifeq ($(shell uname),FreeBSD)
OPTIONS	+= -ltermcap
endif

# Adaptation a Solaris
# Attention : il faut que la lib. readline soit dans le repertoire readline...
ifeq ($(shell uname),SunOS)
OPTIONS	+= -ltermcap  -lsocket -lnsl
LFLAGS 	=  -L../readline
CFLAGS	+= -I..
endif


#
# Objectifs du makefile:
#
all: socklab

socklab: $(OBJS)
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(OPTIONS)

#
# Nettoyage
#
clean :
	rm -f $(OBJS) ; rm -f *~ socklab

#
# Objets
#
socklab.o : socklab.c socklab.h prim.h tools.h options.h
	$(CC) $(CFLAGS) -c socklab.c 

prim.o: prim.c socklab.h prim.h 
	$(CC) $(CFLAGS) -c prim.c 

options.o: options.c socklab.h options.h 
	$(CC) $(CFLAGS) -c options.c 

tools.o: tools.c socklab.h tools.h 
	$(CC) $(CFLAGS) -c tools.c 

tcp.o: tcp.c socklab.h tools.h 
	$(CC) $(CFLAGS) -c tcp.c 

udp.o: udp.c socklab.h tools.h 
	$(CC) $(CFLAGS) -c udp.c 
