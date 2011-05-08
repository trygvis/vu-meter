#include <math.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <portaudio.h>

int main(int argc, char* argv[]) {
	PaError err;

	PaStreamParameters inputParameters, outputParameters;
	PaStream *stream;
	int NUM_CHANNELS = 1;
	double SAMPLE_RATE = 44100;
	double time_per_buffer = 0.2;
	// int FRAMES_PER_BUFFER = time_per_buffer * SAMPLE_RATE;
	int FRAMES_PER_BUFFER = 100;
	PaSampleFormat PA_SAMPLE_TYPE = paFloat32;

	float sampleBlock[FRAMES_PER_BUFFER];

    /* -- initialize PortAudio -- */
    err = Pa_Initialize();
    if( err != paNoError ) {
		fprintf(stderr, "Unable to initialize PortAudio: %s\n", Pa_GetErrorText(err));
		return 1;
	}

    /* -- setup input and output -- */
	bzero( &inputParameters, sizeof( inputParameters ) );
    inputParameters.device = 0; //Pa_GetDefaultInputDevice(); /* default input device */
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
    inputParameters.hostApiSpecificStreamInfo = NULL;

	bzero( &outputParameters, sizeof( outputParameters ) );
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    outputParameters.channelCount = NUM_CHANNELS;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

	printf("Input\n");
	printf("  device: %s (%d)\n", Pa_GetDeviceInfo(inputParameters.device)->name , inputParameters.device);
	printf("  # channels: %d\n", inputParameters.channelCount);

    /* -- setup stream -- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              &outputParameters,
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

		printf("reading..\n");
		err = Pa_ReadStream( stream, sampleBlock, FRAMES_PER_BUFFER );
		read_error = Pa_GetErrorText(err);
		/*
		if( err != paNoError ) {
			fprintf(stderr, "Error reading stream: %s\n", Pa_GetErrorText(err));
			break;
		}
		*/

		float K = 0;
		float sum = 0;
		float volume = 0;

		for(int j = 0; j < FRAMES_PER_BUFFER; j++) {
			sum += pow(sampleBlock[j], 2);
		}
		printf("sum = %f\n", sum);
		volume = 20 * log10(sqrt(sum / FRAMES_PER_BUFFER)) + K;

		// return 0;

		/* */
		printf("writing..\n");
		err = Pa_WriteStream( stream, sampleBlock, FRAMES_PER_BUFFER );
		//if( err ) goto xrun;
		write_error = Pa_GetErrorText(err);
		/* */

		printf("volume = % .02f, read=%s, write=%s\n", volume, read_error, write_error);
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
    Pa_Terminate();
    return 0;
}

