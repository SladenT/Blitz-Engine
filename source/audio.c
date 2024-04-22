/*******************************************************************************************
*	Atempted audio instlation
*	Work in progress
*
*   Created by Aaron Hartle
********************************************************************************************/
/* #include <openal-soft/AL.h>
#include <openal-soft/ALC.h>
#include <openal-soft/sndfile.h>
#define BUFFER_SIZE 4096 */


/* int soundInit() {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <sounds_file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // Initialize OpenAL
    ALCdevice *device = alcOpenDevice(NULL);
    if (!device) {
        fprintf(stderr, "Failed to open OpenAL device\n");
        return 1;
    }

    ALCcontext *context = alcCreateContext(device, NULL);
    if (!context) {
        fprintf(stderr, "Failed to create OpenAL context\n");
        return 1;
    } */

    /* alcMakeContextCurrent(context);

    // Initialize audio source and buffer
    ALuint source;
    alGenSources(1, &source);
    //checkOpenALError("Failed to generate audio source");

    ALuint buffer;
    alGenBuffers(1, &buffer);
    //checkOpenALError("Failed to generate audio buffer");

    // Load audio file
    SF_INFO info;
    SNDFILE *sndfile = sf_open("\\Blitz-Engine\\res\\sounds\\awesomeness.wav", SFM_READ, &info);
    if (!sndfile) {
        fprintf(stderr, "Failed to open audio file %s\n", "awesomeness.wav");
        return 1;
    }

    int format;
    if (info.channels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (info.channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        fprintf(stderr, "Unsupported number of channels: %d\n", info.channels);
        sf_close(sndfile);
        return 1;
    } */

    // Read audio data
   /*  ALsizei size = info.frames * info.channels * sizeof(short);
    short *data = malloc(size);
    sf_read_short(sndfile, data, info.frames * info.channels);

    // Upload audio data to buffer
    alBufferData(buffer, format, data, size, info.samplerate);
    //checkOpenALError("Failed to load audio data into buffer");

    // Set audio source parameters
    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE); */

    // Play audio
    /* alSourcePlay(source);
    //checkOpenALError("Failed to play audio");

    // Wait for audio to finish playing
    ALint source_state;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    } while (source_state == AL_PLAYING);

    // Clean up
    free(data);
    sf_close(sndfile);
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
} */
