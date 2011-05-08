#include "vu-meter.h"

#include <math.h>
#include <stdio.h>

static const float start_threshold = -3;
static const int lights_per_dB = 3;
static const int light_count = 10;
static char stars[10 + 1]; // must match light_count

void vu_meter_init() {
    int i;
    for(i = 0; i < light_count; i++) {
        stars[i] = '*';
    }
    stars[light_count] = 0;
}

void vu_meter_on_sample(int channels, int count, float *buffer) {
    float K = 0;
    float sum = 0;
    float volume = 0;

    for(int i = 0; i < count; i++) {
        sum += pow(buffer[i], 2);
    }

    volume = 20 * log10(sqrt(sum / count)) + K;

    // printf("volume = % .02f\n", volume);
    // char *s[light_count] = "";

    const float origVolume = volume;

    // Make volume a number from 0 to number of dB
    volume += (light_count * lights_per_dB);
    volume = fmax(0, volume);
    volume /= lights_per_dB;
    int v = fmin(volume, light_count);
    printf("%6.2f %d %s\n", origVolume, v, stars + (light_count - v));
}

// vim: set ts=8 sw=4 sts=4 expandtab:
