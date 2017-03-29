CC=g++
RANLIB=ranlib

LIBSRC=uthreads.cpp threadStruct.hpp
LIBOBJ=$(LIBSRC:.cpp=.o)

INCS=-I.
CFLAGS = -Wall -g $(INCS)
LOADLIBES = -L./ 

THREADLIB = libuthreads.a

TAR=tar
TARFLAGS=-cvf
TARNAME=ex.tar
TARSRCS=$(LIBSRC) Makefile README

all: libuthreads.a

libuthreads.a: $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(TARGETS) $(THREADLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)


