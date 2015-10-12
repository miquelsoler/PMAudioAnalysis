//
//  PMDeviceAudioAnalyzer.cpp
//  ConcertParaules
//
//  Created by Miquel Àngel Soler on 25/9/15.
//
//

#include "PMDeviceAudioAnalyzer.hpp"

///--------------------------------------------------------------
PMDeviceAudioAnalyzer::PMDeviceAudioAnalyzer(ofBaseApp *app, int _deviceID, int _inChannels, int _outChannels, int _sampleRate, int _bufferSize)
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
    cout << "Device Id: " << deviceID << endl;
    cout << "Setup: " << app << ", " << outChannels << ", " << inChannels << ", " << sampleRate << ", " << bufferSize << ", " << numBuffers << endl;

    isSetup = false;
}

///--------------------------------------------------------------
PMDeviceAudioAnalyzer::~PMDeviceAudioAnalyzer()
{
    for (int i=0; i<inChannels; i++)
        delete buffers[i];
    delete []buffers;

    for (unsigned int i=0; i<audioAnalyzers.size(); i++)
        delete audioAnalyzers[i];
    audioAnalyzers.clear();
}

///--------------------------------------------------------------
void PMDeviceAudioAnalyzer::setup(PMDAA_ChannelMode _channelMode, int _channelNumber)
{
    if (isSetup) return;

    // Buffer matrix:
    // - Rows: channels
    // - Cols: channel buffer

    buffers = new float *[inChannels];
    for (int i=0; i<inChannels; i++)
    {
        buffers[i] = new float[bufferSize];
    }

    channelMode = _channelMode;
    channelNumber = (channelMode == PMDAA_CHANNEL_MONO) ? _channelNumber : -1;

    // ofxAudioAnalyzer setup

    for (int i=0; i<inChannels; i++)
    {
        ofxAudioAnalyzer *analyzer = new ofxAudioAnalyzer();
        analyzer->setup(bufferSize, sampleRate);

        audioAnalyzers.push_back(analyzer);
    }

    isSetup = true;
}

///--------------------------------------------------------------
void PMDeviceAudioAnalyzer::start()
{
    soundStream.stop();

    soundStream.setup(outChannels, inChannels, sampleRate, bufferSize, numBuffers);
    soundStream.setInput(this);
}

///--------------------------------------------------------------
void PMDeviceAudioAnalyzer::stop()
{
    soundStream.stop();
}

///--------------------------------------------------------------
void PMDeviceAudioAnalyzer::audioIn(float *input, int bufferSize, int nChannels)
{
    for (int i=0; i<nChannels; ++i)
    {
        for (int j=0; j<bufferSize; j++)
        {
            buffers[i][j] = input[i + (nChannels * j)];
        }
    }

    // For testing purposes
    {
        audioAnalyzers[0]->analyze(buffers[0], bufferSize);
        cout << "RMS 0: " << audioAnalyzers[0]->getPitchFreq() << endl;
        audioAnalyzers[1]->analyze(buffers[1], bufferSize);
        cout << "RMS 1: " << audioAnalyzers[1]->getPitchFreq() << endl;
    }
}
