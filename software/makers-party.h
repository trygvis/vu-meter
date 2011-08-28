#ifndef MAKERS_PARTY_C
#define MAKERS_PARTY_C

#include <stdbool.h>
#include <stdint.h>

struct light_descriptor {
	uint8_t dimmable;
};

struct light_set_descriptor {
	// This section overlays the data from the USB device
	uint8_t count;
};

struct makers_party;

bool makers_party_alloc(struct makers_party**);
struct light_set_descriptor* makers_get_light_set_descriptor(struct makers_party*);
struct light_descriptor* makers_get_light_descriptors(struct makers_party*);
void makers_party_set_light(struct makers_party*, uint8_t light, bool on);
void makers_party_dealloc(struct makers_party*);

#endif
