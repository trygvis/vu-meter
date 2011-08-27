CC=sdcc
CPP=sdcpp
CPP=cpp
# Include "AS=sdas8051" in your Makefile.local if this is not found
AS=asx8051
-include Makefile.local

SDCCFLAGS=-mmcs51 \
          --code-size 0x3c00 \
          --xram-size 0x0200 \
          --xram-loc 0x3c00 \
          -I$(FX2LIBDIR)/include

ifeq ($(FX2LIBDIR),)
$(error FX2LIBDIR has to be defined in Makefile.local)
endif

ifeq ($(DEVICE),)
OPT_D=
else
OPT_D=-d=$(DEVICE)
endif

firmwares=vu-meter

#
# Targets
#

TESTS=vu-meter-test

all: target $(patsubst %,target/%.rel,$(firmwares)) $(patsubst %,target/%.ihx,$(firmwares)) $(TESTS)
vu-meter.c: triac-control.h
triac-control.c: triac-control.h

vu-meter-test: vu-meter-test.c triac-control.c triac-control.h
	gcc -DDEBUG -o vu-meter-test vu-meter-test.c triac-control.c

#
# Dependencies
#

target/vu-meter.ihx: target/triac-control.rel target/vu-meter-descriptor.rel target/vu-meter-dscr.rel 

#
# Common targets
#

clean:
	@echo Cleaning...
	@rm -rf target

target:
	@mkdir target

list:
	cycfx2prog --list

program-%: target/%.ihx
	cycfx2prog $(OPT_D) prg:$< run

#
# Rules
#

target/%.rel: %.c
	@echo CC $<
	@mkdir -p target
	@$(CC) $(SDCCFLAGS) -c -o $@ $<

target/%.rel: %.asm
	@echo AS $<
	$(AS) -logsp $<
	@mkdir -p target
	@mv *.rel target/
	@mv *.lst target/
	@mv *.sym target/

target/%.ihx: target/%.rel
	@echo LD $@
	@$(CC) $(SDCCFLAGS) -Wl"-b INT2JT=0x1A00" -Wl"-b DSCR_AREA=0x3e00" -o $@ $^ fx2.lib -L$(FX2LIBDIR)/lib
