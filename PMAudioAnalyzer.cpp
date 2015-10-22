//
//  PMAudioAnalyzer.cpp
//  PMConcertParaules
//
//  Created by Miquel Àngel Soler on 21/10/15.
//
//

#include "PMAudioAnalyzer.hpp"

///--------------------------------------------------------------
void PMAudioAnalyzer::init(bool _useMelBands, int _numMelBands)
{
    useMelBands = _useMelBands;
    numMelBands = useMelBands ? _numMelBands : 0;
}

///--------------------------------------------------------------
void PMAudioAnalyzer::addDeviceAudioAnalyzer(int deviceID,
                                             int inChannels,
                                             int outChannels,
                                             int sampleRate,
                                             int bufferSize,
                                             PMDAA_ChannelMode channelMode,
                                             int channelNumber)
{
    PMDeviceAudioAnalyzer *deviceAudioAnalyzer = new PMDeviceAudioAnalyzer(deviceID, inChannels, outChannels, sampleRate, bufferSize);

    deviceAudioAnalyzer->setup(channelMode, channelNumber, useMelBands, numMelBands);

    audioAnalyzers.push_back(deviceAudioAnalyzer);
}

///--------------------------------------------------------------
void PMAudioAnalyzer::start()
{
    for (int i=0; i<audioAnalyzers.size(); i++)
        audioAnalyzers[i]->start();
}

///--------------------------------------------------------------
void PMAudioAnalyzer::stop()
{
    for (int i=0; i<audioAnalyzers.size(); i++)
        audioAnalyzers[i]->stop();
}

///--------------------------------------------------------------
vector<ofSoundDevice> PMAudioAnalyzer::getDevices()
{
    ofSoundStream soundStream;
    return soundStream.getDeviceList();
}