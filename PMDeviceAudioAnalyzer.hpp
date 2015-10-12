//
//  PMAudioAnalyzer.hpp
//  ConcertParaules
//
//  Created by Miquel Àngel Soler on 25/9/15.
//
//

// Added for testing purposes
// Added (again) for testing purposes
// Added (again and again) for testing purposes

#ifndef PMSoundAnalyzer_hpp
#define PMSoundAnalyzer_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxAudioAnalyzer.h"

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

    int                         deviceID;
    int                         inChannels;
    int                         outChannels;
    int                         sampleRate;
    int                         bufferSize;
    int                         numBuffers;

    PMDAA_ChannelMode           channelMode;
    int                         channelNumber;

    ofSoundStream               soundStream;
    vector<ofxAudioAnalyzer *>  audioAnalyzers;

    float                   **buffers; // buffers[ CHANNEL ][ CHANNEL BUFFER ]

    bool                    isSetup;
};

#endif /* PMSoundAnalyzer_h */
