#include "makers-party.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void usage(char* argv[]) {
	fprintf(stderr, "Usage: %s [on|off]\n", argv[0]);
}

int main(int argc, char* argv[]) {
	makers_party_alloc();

	const struct light_set_descriptor* descriptor = makers_get_light_set_descriptor();

	printf("Number of lights: %d\n", descriptor->count);
	for(int i = 0; i < descriptor->count; i++) {
		printf(" %d: dimmable=%s\n", i, descriptor->descriptors[i].dimmable ? "yes" : "no");
	}

	makers_party_dealloc();

	return EXIT_SUCCESS;
}
