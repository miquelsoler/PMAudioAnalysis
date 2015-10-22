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

    static vector<ofSoundDevice> getDevices();

private:

    bool useMelBands;
    int numMelBands;

    vector<PMDeviceAudioAnalyzer *> audioAnalyzers;
};


#endif /* PMAudioAnalyzer_hpp */
