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

    void init(bool useMelBands, int numMelBands);

    void addDeviceAudioAnalyzer(int deviceID,
                                int inChannels,
                                int outChannels,
                                int sampleRate,
                                int bufferSize,
                                PMDAA_ChannelMode channelMode,
                                int channelNumber);

    void start();
    void stop();

    void clear();

    static vector<ofSoundDevice> getInputDevices();

private:

    bool useMelBands;
    int numMelBands;

    vector<PMDeviceAudioAnalyzer *> deviceAudioAnalyzers;
};


#endif /* PMAudioAnalyzer_hpp */