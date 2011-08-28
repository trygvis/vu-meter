LDFLAGS=$(shell pkg-config --libs libusb-1.0)
CFLAGS=$(shell pkg-config --cflags libusb-1.0) --std=c99 -g -MD -D_XOPEN_SOURCE=600
CC=gcc

BINS=set-light animate-light set-all list-lights

all: $(BINS)

clean:
	@rm -rf $(BINS)
	@rm -rf $(wildcard *.o)
	@rm -rf $(wildcard *.d)

set-all: set-all.o makers-party.o
set-light: set-light.o makers-party.o
list-lights: list-lights.o makers-party.o
animate-light: animate-light.o makers-party.o

-include $(patsubst %.c,%.d,$(wildcard *.c))
