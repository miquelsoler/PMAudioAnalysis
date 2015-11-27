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

    void init(float silenceThreshold, unsigned int silenceQueueLength,
            float onsetsThreshold,
            float smoothingDelta, int ascDescAnalysisSize);

    PMDeviceAudioAnalyzer * addDeviceAudioAnalyzer(unsigned int audioInputIndex, int deviceID, int inChannels, int outChannels,
            int sampleRate, int bufferSize,
            PMDAA_ChannelMode channelMode, unsigned int channelNumber);

    vector<PMDeviceAudioAnalyzer *> *getAudioAnalyzers();

    void start();
    void stop();

    void clear();

    static vector<ofSoundDevice> getInputDevices();
    
    float   getMidiNote();
    float   getEnergy();

private:

    // Silence
    bool            useSilence;
    float           silenceThreshold;
    unsigned int    silenceQueueLength;

    // Onsets
    float           onsetsThreshold;
    float           onsetsAlpha;

    // Smoothing
    float           smoothingDelta;
    
    // Ascendent Descendent Melody analysis
    int             ascDescAnalysisSize;

    vector<PMDeviceAudioAnalyzer *> deviceAudioAnalyzers;
};


#endif /* PMAudioAnalyzer_hpp */
