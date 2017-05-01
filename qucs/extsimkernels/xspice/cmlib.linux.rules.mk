
.SUFFIXES:

CMPP=cmpp
CC=gcc
CFLAGS=-O2 -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wnested-externs -Wold-style-definition -Wredundant-decls -Wconversion -s -fPIC 
INCLUDE=-I. -I$(NGSPICEROOT)/share/ngspice/include/ -I$(QUCSSHARE)/xspice_cmlib/include
LD=gcc
LDFLAGS=-shared

HEADERS=cmextrn.h  cminfo.h  udnextrn.h  udninfo.h

all: $(TARGET)

$(TARGET) : $(HEADERS) $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

$(HEADERS): modpath.lst udnpath.lst
	$(CMPP) -lst

%.o : %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@

%/ifspec.c: %/ifspec.ifs
	CMPP_IDIR=$(@D) CMPP_ODIR=$(@D) $(CMPP) -ifs

%/cfunc.c: %/cfunc.mod
	CMPP_IDIR=$(@D) CMPP_ODIR=$(@D) $(CMPP) -mod

clean:
	rm gainmike/cfunc.c gainmike/ifspec.c res/cfunc.c res/ifspec.c *.h
	rm *.o *.cm
	rm gainmike/*.o
	rm res/*.o



