#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "makers-party.h"

static struct makers_party* mp;

volatile bool stop;

void on_signal(int sig) {
	fprintf(stderr, "stopping\n");
	stop = true;
}

int main(int argc, char* argv[]) {
	assert(makers_party_alloc(&mp));

	signal(SIGINT, on_signal);

	struct light_set_descriptor* light_set = makers_get_light_set_descriptor(mp);

	while(!stop) {
		for(int i = 0; i < light_set->count - 0; i++) {
			makers_party_set_light(mp, i, true);
			usleep(100 * 1000);
			makers_party_set_light(mp, i, false);
		}

		for(int i = light_set->count - 2; i >= 1; i--) {
			makers_party_set_light(mp, i, true);
			usleep(100 * 1000);
			makers_party_set_light(mp, i, false);
		}
	}

	for(int i = 0; i < light_set->count; i++) {
		makers_party_set_light(mp, i, false);
	}
	makers_party_dealloc(mp);

	return EXIT_SUCCESS;
}
