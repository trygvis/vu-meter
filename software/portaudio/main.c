#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <portaudio.h>
#include "vu-meter.h"

void usage(char* argv0) {
    fprintf(stderr, "%s -l\n", argv0);
    fprintf(stderr, "%s -r -i <input device index>\n", argv0);
}

void init_pa_or_die() {
    /* -- initialize PortAudio -- */
    PaError err = Pa_Initialize();
    if( err != paNoError ) {
        fprintf(stderr, "Unable to initialize PortAudio: %s\n", Pa_GetErrorText(err));
        exit(1);
    }
}

void terminate_pa() {
    Pa_Terminate();
}

int list() {
    init_pa_or_die();

    PaDeviceIndex count = Pa_GetDeviceCount();
    printf("Device count: %d\n", count);
    printf("\n");

    for(PaDeviceIndex i = 0; i < count; i++) {
        const PaDeviceInfo * info = Pa_GetDeviceInfo(i);

        printf("#%d: %s\n", i, info->name);
        printf(" Default sample rate: %d\n", info->defaultSampleRate);
    }
    printf("\n");

    printf("Defaults\n");
    printf(" Input device: %d\n", Pa_GetDefaultInputDevice());
    printf(" Output device: %d\n", Pa_GetDefaultOutputDevice());

    terminate_pa();
    return 0;
}

int run() {
    init_pa_or_die();

    PaError err;

    PaStreamParameters inputParameters;
    PaStream *stream;
    int NUM_CHANNELS = 1;
    double SAMPLE_RATE = 44100;
    double time_per_buffer = 0.2;
    // int FRAMES_PER_BUFFER = time_per_buffer * SAMPLE_RATE;
    int FRAMES_PER_BUFFER = 100;
    PaSampleFormat PA_SAMPLE_TYPE = paFloat32;

    float sampleBlock[FRAMES_PER_BUFFER];

    /* -- setup input and output -- */
    bzero( &inputParameters, sizeof( inputParameters ) );
    inputParameters.device = 0; //Pa_GetDefaultInputDevice(); /* default input device */
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /*
    bzero( &outputParameters, sizeof( outputParameters ) );
    outputParameters.device = Pa_GetDefaultOutputDevice(); // default output device
    outputParameters.channelCount = NUM_CHANNELS;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    */

    printf("Input\n");
    printf("  device: %s (%d)\n", Pa_GetDeviceInfo(inputParameters.device)->name , inputParameters.device);
    printf("  # channels: %d\n", inputParameters.channelCount);

    /* -- setup stream -- */
    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        NULL, // &outputParameters,
        SAMPLE_RATE,
        0,
        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
        NULL, /* no callback, use blocking API */
        NULL ); /* no callback, so no callback userData */
    if( err != paNoError ) {
        fprintf(stderr, "Unable to open stream: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    /* -- start stream -- */
    err = Pa_StartStream( stream );
    if( err != paNoError ) {
        fprintf(stderr, "Unable to start stream: %s\n", Pa_GetErrorText(err));
        return 1;
    }
    printf("Wire on. Will run one minute.\n"); fflush(stdout);

    printf("FRAMES_PER_BUFFER=%d\n", FRAMES_PER_BUFFER);

    /* -- Here's the loop where we pass data from input to output -- */
    for( int i=0; i<(60*SAMPLE_RATE)/FRAMES_PER_BUFFER; ++i )
    {
        const char *read_error = "", *write_error = "";

        err = Pa_ReadStream( stream, sampleBlock, FRAMES_PER_BUFFER );
        read_error = Pa_GetErrorText(err);
        /*
        if( err != paNoError ) {
            fprintf(stderr, "Error reading stream: %s\n", Pa_GetErrorText(err));
            break;
        }
        */

        vu_meter_on_sample(inputParameters.channelCount, FRAMES_PER_BUFFER, (float *)sampleBlock);

        // return 0;

        /*
        printf("writing..\n");
        err = Pa_WriteStream( stream, sampleBlock, FRAMES_PER_BUFFER );
        //if( err ) goto xrun;
        write_error = Pa_GetErrorText(err);
        */
    }
    /* -- Now we stop the stream -- */
    err = Pa_StopStream( stream );
    if( err != paNoError ) {
        fprintf(stderr, "Error stopping stream: %s\n", Pa_GetErrorText(err));
    }

    /* -- don't forget to cleanup! -- */
    err = Pa_CloseStream( stream );
    if( err != paNoError ) {
        fprintf(stderr, "Error closing stream: %s\n", Pa_GetErrorText(err));
    }

    goto end;
xrun:
    fprintf(stderr, "Fail: %s\n", Pa_GetErrorText(err));

end:
    terminate_pa();
    return 0;
}

enum Mode {
    MODE_RUN,
    MODE_LIST,
    MODE_HELP
};

int main(int argc, char* argv[]) {
    PaDeviceIndex inIndex = -1;

    enum Mode mode = MODE_HELP;
    char c;
    while ( (c = getopt(argc, argv, "lhri:")) != -1) {
        switch(c) {
            case 'l':
                mode = MODE_LIST;
                break;
            case 'r':
                mode = MODE_RUN;
                break;
            case 'h':
                mode = MODE_HELP;
                break;
            case 'i':
                inIndex = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Unknown argument\n");
                return 0;
        }
    }

    vu_meter_init();

    switch(mode) {
        case MODE_LIST:
            return list();
        case MODE_RUN:
            return run();
        case MODE_HELP:
        default:
            usage(argv[0]);
            return 0;
    }
}

// vim: set ts=8 sw=4 sts=4 expandtab:
