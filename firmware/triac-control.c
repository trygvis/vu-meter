#include "triac-control.h"

#include <delay.h>

extern __code struct light_set_descriptor light_set_descriptor;
extern __code struct light_descriptor light_descriptors;

const int PIN_DS = 1 << 4;
const int PIN_OE_N = 1 << 5;
const int PIN_ST_CP = 1 << 6;
const int PIN_SH_CP = 1 << 7;
const int PIN_MR_N = 1 << 3;

// The states of all the lights.
BOOL light_state[LIGHT_COUNT];

void set_port_d(BYTE b) {
    IOD = b;
#if 0
//    printf("D: 0x%02x\n", b);
    printf("D:  %c  %c  %c  %c  %c\n",
            (b & PIN_MR_N) ? '1' : '0',
            (b & PIN_OE_N) ? '1' : '0',
            (b & PIN_DS) ? '1' : '0',
            (b & PIN_ST_CP) ? '1' : '0',
            (b & PIN_SH_CP) ? '1' : '0');
#endif
}

static void shift_out_byte(BYTE b) {
    BYTE i;

    for(i = 0; i < 8; i++) {
        BYTE bt = b & (1 << i);
        set_port_d(PIN_MR_N | (bt ? PIN_DS : 0x00));
        set_port_d(PIN_MR_N | (bt ? PIN_DS : 0x00) | PIN_SH_CP);
    }

    set_port_d(PIN_MR_N);
    set_port_d(PIN_MR_N | PIN_ST_CP);
}

static void update_lights() {
    BYTE i, j, b;
    BOOL x;

    for(i = 0; i < LIGHT_REGISTER_COUNT; i++) {
        b = 0x00;
        for(j = 0; j < 8; j++) {
            x = light_state[(i * LIGHT_REGISTER_COUNT) + j];
            b |= x << j;
        }

        shift_out_byte(b);
    }
}

void lights_init() {
    BYTE i;

    set_port_d(0x00);
    OED = 0xff;         // port D[0:7] = out
#if 0
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

    for(i = 0; i < LIGHT_COUNT; i++) {
        light_state[i] = FALSE;
    }
    light_state[0] = TRUE;
    update_lights();
}

// TODO: move to triac-descriptor.c
struct light_set_descriptor* triac_get_light_set_descriptor() {
    return &light_set_descriptor;
}

struct light_descriptor* triac_get_light_descriptors() {
    return &light_descriptors;
}

BOOL light_set_light(BYTE light_index, BOOL on) {
    if(light_index >= light_set_descriptor.count) {
        return FALSE;
    }

    light_state[light_index] = on;

    update_lights();

    return TRUE;
}
