#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#include "makers-party.h"

static struct makers_party* mp;

int main(int argc, char* argv[]) {
	assert(makers_party_alloc(&mp));

	struct light_set_descriptor* light_set = makers_get_light_set_descriptor(mp);

    for(int i = 0; i < light_set->count; i++) {
		makers_party_set_light(mp, i, true);
        usleep(500 * 1000);
		makers_party_set_light(mp, i, false);
    }

    for(int i = light_set->count - 1; i >= 0; i--) {
		makers_party_set_light(mp, i, true);
        usleep(500 * 1000);
		makers_party_set_light(mp, i, false);
    }

	makers_party_dealloc(mp);

	return EXIT_SUCCESS;
}
