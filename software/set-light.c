#include "makers-party.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static struct makers_party* makers_party;

void usage(char* argv[]) {
	fprintf(stderr, "Usage: %s [on|off]\n", argv[0]);
}

int main(int argc, char* argv[]) {
	makers_party_alloc(&makers_party);

	if(argc != 2) {
		usage(argv);
		return EXIT_FAILURE;
	}

	bool on = strcmp("on", argv[1]) == 0;
	printf("on=%s\n", on ? "true" : "false");

	for(int i = 0; i < 8; i++) {
		makers_party_set_light(makers_party, i, on);
	}

	makers_party_dealloc(makers_party);

	return EXIT_SUCCESS;
}
