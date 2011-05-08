LDFLAGS=$(shell pkg-config --libs libusb-1.0)
CFLAGS=$(shell pkg-config --cflags libusb-1.0) --std=c99
CC=gcc

all: set-light animate-light set-all

makers-party.c: makers-party.h

set-all: set-all.c makers-party.o
set-light: set-light.c makers-party.o
animate-light: animate-light.c makers-party.o
