//
//  PMAudioAnalyzer.cpp
//  PMConcertParaules
//
//  Created by Miquel Àngel Soler on 21/10/15.
//
//

#include "PMAudioAnalyzer.hpp"

void PMAudioAnalyzer::init(bool _useMelBands, int _numMelBands,
        float _minPitchFreq, float _maxPitchFreq,
        bool _useSilence, int _silenceThreshold, unsigned int _silenceQueueLength,
        float _smoothingDelta)
{
    useMelBands = _useMelBands;
    numMelBands = useMelBands ? _numMelBands : 0;

    minPitchFreq = _minPitchFreq;
    maxPitchFreq = _maxPitchFreq;

    useSilence = _useSilence;
    silenceThreshold = useSilence ? _silenceThreshold : 0;
    silenceQueueLength = _silenceQueueLength;

    smoothingDelta = _smoothingDelta;
}

PMDeviceAudioAnalyzer *PMAudioAnalyzer::addDeviceAudioAnalyzer(int deviceID, int inChannels, int outChannels,
        int sampleRate, int bufferSize,
        PMDAA_ChannelMode channelMode, int channelNumber)
{
    PMDeviceAudioAnalyzer *deviceAudioAnalyzer = new PMDeviceAudioAnalyzer(deviceID, inChannels, outChannels, sampleRate, bufferSize);
    deviceAudioAnalyzer->setup(channelMode, channelNumber,
            useMelBands, numMelBands,
            minPitchFreq, maxPitchFreq,
            useSilence, silenceThreshold, silenceQueueLength, smoothingDelta);

    deviceAudioAnalyzers.push_back(deviceAudioAnalyzer);

    return deviceAudioAnalyzer;
}

vector<PMDeviceAudioAnalyzer *> *PMAudioAnalyzer::getAudioAnalyzers()
{
    return &deviceAudioAnalyzers;
}

void PMAudioAnalyzer::start()
{
    for (int i=0; i< deviceAudioAnalyzers.size(); i++)
        deviceAudioAnalyzers[i]->start();
}

void PMAudioAnalyzer::stop()
{
    for (int i=0; i< deviceAudioAnalyzers.size(); i++)
        deviceAudioAnalyzers[i]->stop();
}

void PMAudioAnalyzer::clear()
{
    stop();

    // Delete all device audio analyzers
    for (int i=0; i<deviceAudioAnalyzers.size(); i++)
        deviceAudioAnalyzers[i]->clear();

    // Erase all device audio analyzers from vector
//    for (int i=0; i<deviceAudioAnalyzers.size(); ++i)
//        delete deviceAudioAnalyzers[i];
    deviceAudioAnalyzers.clear();
}

vector<ofSoundDevice> PMAudioAnalyzer::getInputDevices()
{
    ofSoundStream soundStream;

    vector<ofSoundDevice> allDevices = soundStream.getDeviceList();

#ifdef OF_DEBUG
    soundStream.printDeviceList();
#endif

    vector<ofSoundDevice> inputDevices;
    for (int i=0; i< allDevices.size(); ++i)
        if (allDevices[i].inputChannels > 0)
            inputDevices.push_back(allDevices[i]);

    return inputDevices;
}
