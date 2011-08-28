#include "makers-party.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void usage(char* argv[]) {
	fprintf(stderr, "Usage: %s [on|off]\n", argv[0]);
}

static struct makers_party* mp;

int main(int argc, char* argv[]) {
	if(argc != 2) {
		usage(argv);
		return EXIT_FAILURE;
	}

	bool on = strcmp("on", argv[1]) == 0;
	printf("on=%s\n", on ? "true" : "false");

	assert(makers_party_alloc(&mp));

	for(int i = 0; i < 8; i++) {
		makers_party_set_light(mp, i, on);
	}

	makers_party_dealloc(mp);

	return EXIT_SUCCESS;
}
