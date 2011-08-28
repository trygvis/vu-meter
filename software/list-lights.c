#include "makers-party.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static struct makers_party* mp;

void usage(char* argv[]) {
	fprintf(stderr, "Usage: %s [on|off]\n", argv[0]);
}

int main(int argc, char* argv[]) {
	assert(makers_party_alloc(&mp));

	const struct light_set_descriptor* descriptor = makers_get_light_set_descriptor(mp);
	const struct light_descriptor* lights = makers_get_light_descriptors(mp);

	printf("Number of lights: %d\n", descriptor->count);
	for(int i = 0; i < descriptor->count; i++) {
		printf(" %d: dimmable=%s\n", i, lights[i].dimmable ? "yes" : "no");
	}

	makers_party_dealloc(mp);

	return EXIT_SUCCESS;
}
