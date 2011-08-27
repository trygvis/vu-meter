#include "triac-control.h"

#ifdef DEBUG
#include <stdio.h>
#endif

const int PIN_DS = 1 << 4;
const int PIN_OE_N = 1 << 5;
const int PIN_ST_CP = 1 << 6;
const int PIN_SH_CP = 1 << 7;
const int PIN_MR_N = 1 << 3;

void set_port_d(BYTE b) {
#ifdef DEBUG
//    printf("D: 0x%02x\n", b);
    printf("D: %c %c\n", (b & PIN_MR_N) ? '1' : '0', (b & PIN_OE_N) ? '1' : '0');
#else
    IOD = b;
#endif
}

void init_port_d() {
    set_port_d(0x00);
#ifndef DEBUG
    OED = 0xff;         // port D[0:7] = out
#else
    printf("   MR OE\n");
#endif

    // Master reset
    set_port_d(PIN_MR_N);
    set_port_d(0x00);
    // Clock is low
    set_port_d(PIN_MR_N);
}

void shift_out_byte(BYTE b) {
    BYTE i;

    for(i = 0; i < 8; i++) {
        BYTE bt = b & (1 << i);
        set_port_d(PIN_MR_N | (bt ? PIN_DS : 0x00));
        set_port_d(PIN_MR_N | (bt ? PIN_DS : 0x00) | PIN_SH_CP);
    }

    set_port_d(PIN_MR_N);
    set_port_d(PIN_MR_N | PIN_ST_CP);
}
