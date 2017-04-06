CC=g++
RANLIB=ranlib

LIBSRC=uthreads.cpp uthreads.h threadStruct.hpp threadStruct.cpp
LIBOBJ=uthreads.o threadStruct.o

INCS=-I.
CFLAGS = -Wall -g $(INCS)
LOADLIBES = -L./ 
LFLAGS = -o


THREADLIB = libuthreads.a

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS=$(LIBSRC) Makefile README

all: test tara ctest

libuthreads.a: $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

dumbusage: dumbusage.o libuthreads.a
	$(CC) $^ $(LOADLIBES) -luthreads -o $@

dumbusage.o: dumbusage.cpp
	$(CC) -c $^ -o $@

test: test.o libuthreads.a
	$(CC) $^ $(LOADLIBES) -luthreads -o $@

test.o: test.cpp
	$(CC) -g $^ -o $@

threadStruct.o: threadStruct.cpp
	$(CC) -g threadStruct.cpp -o threadStruct.o

clean:
	$(RM) $(TARGETS) $(THREADLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tara:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)

ctest:
	cp -f ex2.tar ex2sanity

rtest:
	python3 ex2sanity/test.py
