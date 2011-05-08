#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <portaudio.h>

int main(int argc, char* argv[]) {
	int err;

    /* -- initialize PortAudio -- */
    err = Pa_Initialize();
    if( err != paNoError ) {
		fprintf(stderr, "Unable to initialize PortAudio\n");
		return 0;
	}

	PaDeviceIndex count = Pa_GetDeviceCount();
	printf("device count: %d\n", count);

	for(PaDeviceIndex i = 0; i < count; i++) {
		const PaDeviceInfo * info = Pa_GetDeviceInfo(i);

		printf("#%d: %s\n", i, info->name);
		printf(" Default sample rate: %d\n", info->defaultSampleRate);
	}

	printf("Default input device: %d\n", Pa_GetDefaultInputDevice());
	printf("Default output device: %d\n", Pa_GetDefaultOutputDevice());

    Pa_Terminate();
    return 0;
}

