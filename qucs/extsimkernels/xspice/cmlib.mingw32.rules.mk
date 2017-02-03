
.SUFFIXES:

CMPP=$(NGSPICEROOT)/bin/cmpp.exe
CC=gcc.exe
CFLAGS=-O2 -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wnested-externs -Wold-style-definition -Wredundant-decls -Wconversion -s -fPIC 
INCLUDE=-I. -I$(NGSPICEROOT)/share/ngspice/include/ -I$(QUCSSHARE)/xspice_cmlib/include
LD=gcc.exe
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
	pushd & cd $(@D) & \
	$(CMPP) -ifs & \
	popd

%/cfunc.c: %/cfunc.mod
	pushd & cd $(@D) & \
	$(CMPP) -mod & \
	popd




