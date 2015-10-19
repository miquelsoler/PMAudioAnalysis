//
//  PMAudioAnalyzer.hpp
//  ConcertParaules
//
//  Created by Miquel Àngel Soler on 25/9/15.
//
//

#pragma once

#ifndef PMDeviceAudioAnalyzer_hpp
#define PMDeviceAudioAnalyzer_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxAudioAnalyzer.h"
#include "PMAudioOutParams.h"

typedef enum
{
    PMDAA_CHANNEL_MULTI = 0,
    PMDAA_CHANNEL_MONO = 1
} PMDAA_ChannelMode;


class PMDeviceAudioAnalyzer : public ofBaseSoundInput
{
public:

    // Constructor just sets attributes. Calling it doesn't start the sound stream analysis.
    PMDeviceAudioAnalyzer(ofBaseApp *app, int deviceID, int inChannels, int outChannels, int sampleRate, int bufferSize);
    PMDeviceAudioAnalyzer();
    ~PMDeviceAudioAnalyzer();

    void setup(PMDAA_ChannelMode channelMode = PMDAA_CHANNEL_MULTI, int channelNumber = -1);

    void start();
    void stop();

    void audioIn(float *input, int bufferSize, int nChannels);

private:

    // Setup
    int                         deviceID;
    int                         inChannels;
    int                         outChannels;
    int                         sampleRate;
    int                         bufferSize;
    int                         numBuffers;

    // Channel mode
    PMDAA_ChannelMode           channelMode;
    int                         channelNumber;

    // Events for listeners
    ofEvent<pitchParams>        eventPitchChanged;
    ofEvent<onsetParams>        eventOnsetDetected;
    ofEvent<void>               eventSilenceDetected;
    ofEvent<fftBandsParams>     eventFFTBandsChanged;

    // Internals
    ofSoundStream               soundStream;
    vector<ofxAudioAnalyzer *>  audioAnalyzers;

    float                   **buffers; // buffers[ CHANNEL ][ CHANNEL BUFFER ]

    bool                    isSetup;
};

#endif /* PMDeviceAudioAnalyzer_h */
