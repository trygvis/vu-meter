#include "triac-control.h"

#ifndef DEBUG
#include <delay.h>
#else
#include <stdio.h>
#endif

#ifndef DEBUG
extern __code struct light_set_descriptor light_set_descriptor;
extern __code struct light_descriptor light_descriptors;
#endif

const int PIN_DS = 1 << 4;
const int PIN_OE_N = 1 << 5;
const int PIN_ST_CP = 1 << 6;
const int PIN_SH_CP = 1 << 7;
const int PIN_MR_N = 1 << 3;

void set_port_d(BYTE b) {
#ifndef DEBUG
    IOD = b;
#else
//    printf("D: 0x%02x\n", b);
    printf("D:  %c  %c  %c  %c  %c\n",
            (b & PIN_MR_N) ? '1' : '0',
            (b & PIN_OE_N) ? '1' : '0',
            (b & PIN_DS) ? '1' : '0',
            (b & PIN_ST_CP) ? '1' : '0',
            (b & PIN_SH_CP) ? '1' : '0');
#endif
}

void init_port_d() {
    set_port_d(0x00);
#ifndef DEBUG
    OED = 0xff;         // port D[0:7] = out
#else
    printf("   MR OE DS ST SH\n");
#endif

    // Master reset
    set_port_d(PIN_MR_N);
    set_port_d(0x00);
    // Clock is low
    set_port_d(PIN_MR_N);

    shift_out_byte(1 << 0); delay(200);
    shift_out_byte(1 << 1); delay(200);
    shift_out_byte(1 << 2); delay(200);
    shift_out_byte(1 << 3); delay(200);
    shift_out_byte(1 << 4); delay(200);
    shift_out_byte(1 << 5); delay(200);
    shift_out_byte(1 << 6); delay(200);
    shift_out_byte(1 << 7); delay(200);

    // Start with lights off
    shift_out_byte(0x00);
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

#ifndef DEBUG
// TODO: move to triac-descriptor.c
struct light_set_descriptor* triac_get_light_set_descriptor() {
    return &light_set_descriptor;
}

struct light_descriptor* triac_get_light_descriptors() {
    return &light_descriptors;
}
#endif
