#ifndef TRIAC_CONTROL_H
#define TRIAC_CONTROL_H

#ifdef DEBUG
typedef int BYTE;
#else
#include <fx2regs.h>
#endif

struct light_descriptor {
    BYTE dimmable;
};

struct light_set_descriptor {
    BYTE count;
};

void init_port_d();
void shift_out_byte(BYTE b);

struct light_set_descriptor* triac_get_light_set_descriptor();
struct light_descriptor* triac_get_light_descriptors();

#endif
