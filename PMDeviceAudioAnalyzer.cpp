//
//  PMDeviceAudioAnalyzer.cpp
//  ConcertParaules
//
//  Created by Miquel Àngel Soler on 25/9/15.
//
//

#include "PMDeviceAudioAnalyzer.hpp"
#include "PMAudioAnalyzerConstants.h"

PMDeviceAudioAnalyzer::PMDeviceAudioAnalyzer(int _deviceID, int _inChannels, int _outChannels, int _sampleRate, int _bufferSize)
{
    deviceID = _deviceID;
    inChannels = _inChannels;
    outChannels = _outChannels;
    sampleRate = _sampleRate;
    bufferSize = _bufferSize;
//    numBuffers = _numBuffers;
    numBuffers = bufferSize/64;

    soundStream.printDeviceList();

    soundStream.setDeviceID(deviceID);

    isSetup = false;
}

PMDeviceAudioAnalyzer::~PMDeviceAudioAnalyzer()
{
    for (int i=0; i<inChannels; ++i)
        delete buffers[i];
    delete []buffers;

    for (unsigned int i=0; i<audioAnalyzers.size(); ++i)
        delete audioAnalyzers[i];
    audioAnalyzers.clear();
}

void PMDeviceAudioAnalyzer::setup(PMDAA_ChannelMode _channelMode, int _channelNumber,
        bool _useMelBands, int _numMelBands,
        bool _useSilence, int _silenceThreshold)
{
    if (isSetup) return;

    // Channels
    channelMode = _channelMode;
    channelNumber = (channelMode == PMDAA_CHANNEL_MONO) ? _channelNumber : -1;

    // Mel bands
    useMelBands = _useMelBands;
    numMelBands = useMelBands ? _numMelBands : 0;

    // Silence
    useSilence = _useSilence;
    silenceThreshold = _silenceThreshold;

    int numUsedChannels = (channelMode == PMDAA_CHANNEL_MONO) ? 1 : inChannels;

    // Creation of audio in buffers
    // Buffer matrix:
    // - Rows: channels
    // - Cols: channel buffer

    buffers = new float *[numUsedChannels];
    for (int i=0; i<numUsedChannels; ++i)
        buffers[i] = new float[bufferSize];

    // ofxAudioAnalyzer(s) setup

    for (int i=0; i<numUsedChannels; ++i)
    {
        ofxAudioAnalyzer *analyzer = new ofxAudioAnalyzer();
        analyzer->setup(bufferSize, sampleRate,
                useMelBands, numMelBands,
                useSilence, silenceThreshold
        );

        audioAnalyzers.push_back(analyzer);
    }

    isSetup = true;
}

void PMDeviceAudioAnalyzer::start()
{
    soundStream.stop();

    soundStream.setup(outChannels, inChannels, sampleRate, bufferSize, numBuffers);
    soundStream.setInput(this);
}

void PMDeviceAudioAnalyzer::stop()
{
    soundStream.stop();
}

void PMDeviceAudioAnalyzer::clear()
{
    stop();

    // Delete internal audio analyzer stuff
    for (int i=0; i<audioAnalyzers.size(); ++i)
        audioAnalyzers[i]->exit();

    // Erase all audio analyzers from vector
    for (int i=0; i<audioAnalyzers.size(); ++i)
        delete audioAnalyzers[i];
    audioAnalyzers.clear();
}

void PMDeviceAudioAnalyzer::audioIn(float *input, int bufferSize, int nChannels)
{
    float pitchFreq;

//    cout << "audioIn" << endl;
    int numUsedChannels = (channelMode == PMDAA_CHANNEL_MONO) ? 1 : inChannels;

    // Parse input array
    for (int i=0; i<numUsedChannels; ++i)
        for (int j=0; j<bufferSize; ++j)
            buffers[i][j] = input[i + (nChannels * j)];

    // Init audio event params

    pitchParams pitchParams;
    pitchParams.deviceID = deviceID;

    onsetParams onsetParams;
    onsetParams.deviceID = deviceID;

    freqBandsParams freqBandsParams;
    freqBandsParams.deviceID = deviceID;

    silenceParams silenceParams;
    silenceParams.deviceID = deviceID;

    for (int i=0; i<numUsedChannels; ++i)
    {
        audioAnalyzers[i]->analyze(buffers[i], bufferSize);

        int channel = (channelMode == PMDAA_CHANNEL_MONO) ? channelNumber : i;

        // Pitch
        {
            pitchFreq = audioAnalyzers[i]->getPitchFreq();
            if ((pitchFreq > PITCH_MINFREQ) && (pitchFreq < PITCH_MAXFREQ)) // Skip ultra high or ultra low pitch frequencies
            {
                pitchParams.channel = channel;
                pitchParams.freq = audioAnalyzers[i]->getPitchFreq();
                pitchParams.confidence = audioAnalyzers[i]->getPitchConf();
                pitchParams.midiNote = 0;
                pitchParams.midiNoteNoOctave = 0;
                ofNotifyEvent(eventPitchChanged, pitchParams, this);
            }
        }

        // Frequency bands
        {
            freqBandsParams.channel = channel;
            freqBandsParams.melBands = audioAnalyzers[i]->getMelBands();
            freqBandsParams.numBands = numMelBands;
            ofNotifyEvent(eventFreqBandsParams, freqBandsParams, this);
        }

        // Onset
        {
            if (audioAnalyzers[i]->getIsOnset()) {
                onsetParams.channel = channel;

//                cout << "DV" << onsetParams.deviceID << " CH" << onsetParams.channel << " Onset -"
//                        << " HFC:" << audioAnalyzers[i]->getOnsetHfc()
//                        << " Complex:" << audioAnalyzers[i]->getOnsetHfc()
//                        << " Flux:" << audioAnalyzers[i]->getOnsetFlux()
//                        << endl;
                ofNotifyEvent(eventOnsetDetected, onsetParams, this);
            }
        }

        // Silence
        {
            bool silent = audioAnalyzers[i]->getIsSilent();
            string silentOutput = silent ? "YES" : "NO";
            cout << "DV" << silenceParams.deviceID << " CH" << silenceParams.channel << " Is silent? " << silentOutput << endl;
        }
    }
}
