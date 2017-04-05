CC=g++
RANLIB=ranlib

LIBSRC=uthreads.cpp threadStruct.hpp threadStruct.cpp
LIBOBJ=uthreads.o threadStruct.o

INCS=-I.
CFLAGS = -Wall -g $(INCS)
LOADLIBES = -L./ 
LFLAGS = -o


THREADLIB = libuthreads.a

TAR=tar
TARFLAGS=-cvf
TARNAME=ex.tar
TARSRCS=$(LIBSRC) Makefile README

all: dumbusage

libuthreads.a: $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

dumbusage: dumbusage.o
	$(CC) $^ $(LOADLIBES) -luthreads -o $@

dumbusage.o: dumbusage.cpp
	$(CC) $(CFLAGS) $^ -o $@

threadStruct.o: threadStruct.cpp
	$(CC) -c threadStruct.cpp -o threadStruct.o


clean:
	$(RM) $(TARGETS) $(THREADLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)


