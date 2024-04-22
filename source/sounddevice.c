/*******************************************************************************************
*	Atempted sound effect
*	Work in progress
*
*   Created by Aaron Hartle
********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <AL.h>
#include <ALC.h>


// Function to initialize OpenAL
bool initOpenAL() {
    ALCdevice* device = alcOpenDevice(NULL);
    if (!device) {
        printf("Failed to open OpenAL device\n");
        return false;
    }

    ALCcontext* context = alcCreateContext(device, NULL);
    if (!context) {
        printf("Failed to create OpenAL context\n");
        alcCloseDevice(device);
        return false;
    }

    alcMakeContextCurrent(context);
    return true;
}

// Function to play sound effect
void playSoundEffect(const char* filename) {
    ALuint buffer, source;
    
    // Generate buffer
    alGenBuffers(1, &buffer);
    checkALError("alGenBuffers");

    // Load WAV file
    ALsizei size, frequency;
    ALenum format;
    ALvoid* data;
    alutLoadWAVFile(filename, &format, &data, &size, &frequency);
    checkALError("alutLoadWAVFile");

    // Set buffer data
    alBufferData(buffer, format, data, size, frequency);
    checkALError("alBufferData");

    // Generate source
    alGenSources(1, &source);
    checkALError("alGenSources");

    // Attach buffer to source
    alSourcei(source, AL_BUFFER, buffer);
    checkALError("alSourcei");

    // Play source
    alSourcePlay(source);
    checkALError("alSourcePlay");

    // Wait for sound to finish playing
    ALint source_state;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    } while (source_state == AL_PLAYING);

    // Clean up
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
}

// Function to handle button click
void handleButtonClick() {
    // 'click_sound_1.wav' is the filename of the sound effect
    playSoundEffect("click_sound_1.wav");
}

int main() {
    // Initialize OpenAL
    if (!initOpenAL()) {
        return EXIT_FAILURE;
    }

    // Game loop
    bool running = true;
    while (running) { 
        handleButtonClick();
    }

    // Clean up OpenAL
    cleanupOpenAL();

    return EXIT_SUCCESS;
}

