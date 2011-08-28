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

	struct light_descriptor* descriptors;
};

void makers_party_alloc();
struct light_set_descriptor* makers_get_light_set_descriptor();
void makers_party_set_light(uint8_t light, bool on);
void makers_party_dealloc();

#endif
