#include "makers-party.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct makers_party* mp;

void usage(char* argv[]) {
	fprintf(stderr, "Usage: %s [light] [on|off]\n", argv[0]);
}

int main(int argc, char* argv[]) {
	if(argc != 3) {
		usage(argv);
		return EXIT_FAILURE;
	}

	int light = atoi(argv[1]);

	if(light == 0) {
		usage(argv);
		return EXIT_FAILURE;
	}

	bool on = strcmp("on", argv[2]) == 0;

	makers_party_alloc(&mp);

	struct light_set_descriptor* light_set = makers_get_light_set_descriptor(mp);

	if(light > light_set->count) {
		fprintf(stderr, "Light index out of range: %d, max=%d\n", light, light_set->count);
		return EXIT_FAILURE;
	}

	makers_party_set_light(mp, light - 1, on);

	makers_party_dealloc(mp);

	return EXIT_SUCCESS;
}
