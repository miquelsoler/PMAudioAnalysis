//
//  PMDeviceAudioAnalyzer.cpp
//  ConcertParaules
//
//  Created by Miquel Àngel Soler on 25/9/15.
//
//

#include "PMDeviceAudioAnalyzer.hpp"

static const float SMOOTHING_INITIALVALUE = -999.0f;

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

void PMDeviceAudioAnalyzer::setup(unsigned int _audioInputIndex, PMDAA_ChannelMode _channelMode, unsigned int _channelNumber,
        bool _useMelBands, int _numMelBands,
        float _minPitchFreq, float _maxPitchFreq,
        bool _useSilence, int silenceThreshold, unsigned int silenceQueueLength, float _smoothingDelta)
{
    if (isSetup) return;

    audioInputIndex = _audioInputIndex;

    // Channels
    channelMode = _channelMode;
    channelNumber = (channelMode == PMDAA_CHANNEL_MONO) ? _channelNumber : -1;
    int numUsedChannels = (channelMode == PMDAA_CHANNEL_MONO) ? 1 : inChannels;

    // Mel bands
    useMelBands = _useMelBands;
    numMelBands = useMelBands ? _numMelBands : 0;

    // Pitch
    minPitchFreq = _minPitchFreq;
    maxPitchFreq = _maxPitchFreq;

    // Silence
    useSilence = _useSilence;
    wasSilent = false;

    // Smoothing

    smoothingDelta = _smoothingDelta;

    if (!oldPitchFreqValues.empty())
        oldPitchFreqValues.clear();

    oldPitchFreqValues.reserve((unsigned long)numUsedChannels);
    for (int i=0; i<numUsedChannels; ++i)
        oldPitchFreqValues[i] = SMOOTHING_INITIALVALUE;

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
                useSilence, silenceThreshold, silenceQueueLength);

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
    int numUsedChannels = (channelMode == PMDAA_CHANNEL_MONO) ? 1 : inChannels;
//    cout << "numUsedChannels: " << numUsedChannels << endl;

    // Parse input array
    for (int i=0; i<numUsedChannels; ++i)
        for (int j=0; j<bufferSize; ++j)
            buffers[i][j] = input[i + (nChannels * j)];

    // Init of audio event params struct
    pitchParams pitchParams;
    pitchParams.deviceID = deviceID;
    pitchParams.audioInputIndex = audioInputIndex;
    silenceParams silenceParams;
    silenceParams.deviceID = deviceID;
    silenceParams.audioInputIndex = audioInputIndex;
    energyParams energyParams;
    energyParams.deviceID = deviceID;
    energyParams.audioInputIndex = audioInputIndex;
    onsetParams onsetParams;
    onsetParams.deviceID = deviceID;
    onsetParams.audioInputIndex = audioInputIndex;
    freqBandsParams freqBandsParams;
    freqBandsParams.deviceID = deviceID;
    freqBandsParams.audioInputIndex = audioInputIndex;

    for (int i=0; i<numUsedChannels; ++i)
    {
        audioAnalyzers[i]->analyze(buffers[i], bufferSize);

        int channel = (channelMode == PMDAA_CHANNEL_MONO) ? channelNumber : i;

        bool isSilent = audioAnalyzers[i]->getIsSilent();

        // Silence

        if (wasSilent != isSilent) // Changes in silence (ON>OFF or OFF>ON)
        {
            wasSilent = isSilent;
            silenceParams.channel = channel;
            silenceParams.isSilent = isSilent;
            ofNotifyEvent(eventSilenceStateChanged, silenceParams, this);
        }

        // Process only when no silence detected
        if (!isSilent)
        {
            // Energy
            {
                energyParams.channel = channel;
                energyParams.energy = audioAnalyzers[i]->getEnergy();
                ofNotifyEvent(eventEnergyChanged, energyParams, this);
            }

            // Pitch
            {
                float smoothedPitchFreq, currentPitchFreq;

                currentPitchFreq = audioAnalyzers[i]->getPitchFreq();

                if ((currentPitchFreq > minPitchFreq) && (currentPitchFreq < maxPitchFreq)) // Skip ultra high or ultra low pitch frequencies
                {
                    if (oldPitchFreqValues[i] == SMOOTHING_INITIALVALUE)
                    {
                        smoothedPitchFreq = currentPitchFreq;
                    } else {
                        smoothedPitchFreq = (currentPitchFreq * smoothingDelta) + (oldPitchFreqValues[i] * (1.0f - smoothingDelta));
                    }

                    oldPitchFreqValues[i] = smoothedPitchFreq;

                    pitchParams.channel = channel;
                    pitchParams.freq = smoothedPitchFreq;
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
                    ofNotifyEvent(eventOnsetDetected, onsetParams, this);
                }
            }
        }
    }
}
