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
#include "PMAudioInParams.h"

typedef enum
{
    PMDAA_CHANNEL_MULTI = 0,
    PMDAA_CHANNEL_MONO = 1
} PMDAA_ChannelMode;


class PMDeviceAudioAnalyzer : public ofBaseSoundInput
{
public:

    /**
     * PMDeviceAudioAnalyzer(...)
     * Constructor just sets sound stream attributes. Calling it doesn't start the sound stream analysis.
     */
    PMDeviceAudioAnalyzer(int deviceID, int inChannels, int outChannels, int sampleRate, int bufferSize);

    PMDeviceAudioAnalyzer() {};

    ~PMDeviceAudioAnalyzer();

    /**
     * setup:
     * - channelMode: mono or multichannel
     * - channelNumber: 0..N (ignored when in multichannel mode)
     * - useMelBands: true if it needs obtaining mel bands
     * - numMelBands: number of mel bands (ignored when useMelBands=false)
     */
    void setup(unsigned int audioInputIndex, PMDAA_ChannelMode channelMode, unsigned int channelNumber,
            bool useMelBands, int numMelBands,
            float minPitchFreq, float maxPitchFreq,
            bool useSilence, int silenceThreshold, unsigned int silenceQueueLength, float smoothingDelta);

    void start();
    void stop();

    void clear();

    void audioIn(float *input, int bufferSize, int nChannels);

    unsigned int getInputIndex() { return audioInputIndex; };
    int getDeviceID() { return deviceID; };
    int getChannelNumber() { return channelNumber; };

    // Events for listeners
    ofEvent<pitchParams>        eventPitchChanged;
    ofEvent<silenceParams>      eventSilenceStateChanged;
    ofEvent<energyParams>       eventEnergyChanged;
    ofEvent<onsetParams>        eventOnsetDetected;
    ofEvent<freqBandsParams>    eventFreqBandsParams;

private:

    // Setup
    unsigned int                audioInputIndex;
    int                         deviceID;
    int                         inChannels;
    int                         outChannels;
    int                         sampleRate;
    int                         bufferSize;
    int                         numBuffers;

    // Channel mode
    PMDAA_ChannelMode           channelMode;
    int                         channelNumber;

    // Mel bands
    bool                        useMelBands;
    int                         numMelBands;

    // Pitch
    float                       minPitchFreq;
    float                       maxPitchFreq;

    // Silence
    bool                        useSilence;
    bool                        wasSilent;

    // Smoothing
    float                       smoothingDelta;
    vector<float>               oldPitchFreqValues;

    // Internals

    ofSoundStream               soundStream;
    vector<ofxAudioAnalyzer *>  audioAnalyzers;


    float                   **buffers; // buffers[ CHANNEL ][ CHANNEL BUFFER ]

    bool                    isSetup;
};

#endif /* PMDeviceAudioAnalyzer_h */
