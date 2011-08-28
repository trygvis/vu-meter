#ifndef TRIAC_CONTROL_H
#define TRIAC_CONTROL_H

#ifndef LIGHT_REGISTER_COUNT
#error LIGHT_REGISTER_COUNT has to be defined
#endif
#define LIGHT_COUNT (LIGHT_REGISTER_COUNT * 8)

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

void lights_init();

BOOL light_set_light(BYTE light_index, BOOL on);
struct light_set_descriptor* triac_get_light_set_descriptor();
struct light_descriptor* triac_get_light_descriptors();

#endif
