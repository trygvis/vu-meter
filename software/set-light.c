#include "makers-party.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void usage(char* argv[]) {
	fprintf(stderr, "Usage: %s [on|off] [light]\n", argv[0]);
	fprintf(stderr, "light is an integer from 1 to 8.\n");
}

int main(int argc, char* argv[]) {
	makers_party_alloc();

	if(argc != 3) {
		usage(argv);
		return EXIT_FAILURE;
	}

	bool on = strcmp("on", argv[1]) == 0;
	int light = atoi(argv[2]);

	if(light < 1 || light > 8) {
		usage(argv);
		return EXIT_FAILURE;
	}

	printf("on=%s, light=%d\n", on ? "true" : "false", light);
	makers_party_set_light(on, light);

	makers_party_dealloc();

	return EXIT_SUCCESS;
}
