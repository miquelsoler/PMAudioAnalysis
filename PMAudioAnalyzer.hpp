//
//  PMAudioAnalyzer.hpp
//  PMConcertParaules
//
//  Created by Miquel Àngel Soler on 21/10/15.
//
//

#ifndef PMAudioAnalyzer_hpp
#define PMAudioAnalyzer_hpp

#include <stdio.h>
#include "PMDeviceAudioAnalyzer.hpp"


class PMAudioAnalyzer
{
public:

    /**
     * getInstance()
     * Returns singleton instance
     */
    static PMAudioAnalyzer &getInstance()
    {
        static PMAudioAnalyzer instance;
        return instance;
    }

    void init(bool useMelBands, int numMelBands,
            float minPitchFreq, float maxPitchFreq,
            bool useSilence, int silenceThreshold, unsigned int silenceQueueLength,
            float smoothingDelta);

    PMDeviceAudioAnalyzer * addDeviceAudioAnalyzer(unsigned int audioInputIndex, int deviceID, int inChannels, int outChannels,
            int sampleRate, int bufferSize,
            PMDAA_ChannelMode channelMode, unsigned int channelNumber);

    vector<PMDeviceAudioAnalyzer *> *getAudioAnalyzers();

    void start();
    void stop();

    void clear();

    static vector<ofSoundDevice> getInputDevices();

private:

    // Mel Bands
    bool            useMelBands;
    int             numMelBands;

    // Pitch
    float           minPitchFreq;
    float           maxPitchFreq;

    // Silence
    bool            useSilence;
    int             silenceThreshold;
    unsigned int    silenceQueueLength;

    // Smoothing
    float           smoothingDelta;

    vector<PMDeviceAudioAnalyzer *> deviceAudioAnalyzers;
};


#endif /* PMAudioAnalyzer_hpp */
