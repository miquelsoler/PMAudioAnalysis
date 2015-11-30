//
//  PMAudioAnalyzer.hpp
//  ConcertParaules
//
//  Created by Miquel Àngel Soler on 25/9/15.
//
//

#pragma once

#ifndef PMDeviceAudioAnalyzer_hpp
#define PMDeviceAudioAnalyzer_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxAubio.h"
#include "PMAudioInParams.h"



class PMDeviceAudioAnalyzer : public ofBaseSoundInput
{
public:

    PMDeviceAudioAnalyzer(int deviceID, int inChannels, int outChannels, int sampleRate, int bufferSize);
    PMDeviceAudioAnalyzer() {};
    ~PMDeviceAudioAnalyzer();

    void setup(unsigned int audioInputIndex, vector<unsigned int> channelNumbers,
            float silenceThreshold, unsigned int silenceQueueLength,
            float onsetsThreshold,
            float smoothingDelta,
            int ascDescAnalysisSize);

    void start();
    void stop();
    void clear();

    void audioIn(float *input, int bufferSize, int nChannels);

    unsigned int            getInputIndex()     { return audioInputIndex; };
    int                     getDeviceID()       { return deviceID; };
    int                     getSampleRate()     { return sampleRate; };
    int                     getNumChannels()    { return inChannels; };
    vector<unsigned int>    getChannelNumbers() { return channelNumbers; };

    void                    setSilenceThreshold(float value)   { silenceThreshold = value; };
    void                    setSilenceQueueLength(float value) { silenceTimeTreshold = value; };
    void                    setPauseTimeTreshold(float value)  { pauseTimeTreshold = value; };

    void                    setOnsetsThreshold(float value);


    // Events for listeners
    ofEvent<pitchParams> eventPitchChanged;
    ofEvent<silenceParams> eventSilenceStateChanged;
    ofEvent<energyParams> eventEnergyChanged;
    ofEvent<onsetParams> eventOnsetStateChanged;
    ofEvent<freqBandsParams> eventFreqBandsParams;
    ofEvent<shtParams> eventShtStateChanged;
    ofEvent<pauseParams> eventPauseStateChanged;
    ofEvent<melodyDirectionParams> eventMelodyDirection;

private:

    // Setup
    unsigned int audioInputIndex;
    int deviceID;
    int inChannels;
    int outChannels;
    int sampleRate;
    int bufferSize;
    int numBuffers;

    vector<unsigned int> channelNumbers;

    // Silence
    bool wasSilent;
    float silenceThreshold;

    // Onsets
    float onsetsThreshold;
    // TODO: Deixa de ser vector
    bool oldOnsetState;

    // Smoothing
    float smoothingDelta;
    // TODO: Deixa de ser vector (però manté deque)
    deque<float> midiNoteHistory;

    // Sound analysis

    ofSoundStream soundStream;

    // TODO: Deixa de ser vector
    ofxAubioPitch *aubioPitch;
    // TODO: Deixa de ser vector
    ofxAubioOnset *aubioOnset;
    // TODO: Deixa de ser vector
    ofxAubioMelBands *aubioMelBands;

    bool isSetup;

    // TODO: Deixa de ser vector
    bool isInSilence;
    // TODO: Deixa de ser vector
    bool isInPause;
    // TODO: Deixa de ser vector
    float silenceBeginTime;
    float silenceTimeTreshold;
    float pauseTimeTreshold;

    // TODO: Fora canal
    float getEnergy();
    // TODO: Fora canal, es manté input perque no ve directament d'Aubio
    float getRms(float *input, int bufferSize);
    // TODO: Fora canal, es manté input perque no ve directament d'Aubio
    float getAbsMean(float *input, int bufferSize);
    // TODO: Fora canal
    void detectedSilence();
    // TODO: Fora canal
    void updateSilenceTime();
    // TODO: Fora canal
    void detectedEndSilence();
    // TODO: Fora canal
    void checkMelodyDirection();

    int ascDescAnalysisSize;

    // sshht
    // TODO: Deixa de ser vector
    bool isShtSounding;
    // TODO: Deixa de ser vector
    float shtBeginTime;
    float shtTimeTreshold;
    // TODO: Deixa de ser vector
    bool isShtTrueSent;
    // TODO: Deixa de ser vector
    bool isShtFalseSent;

    // TODO: Fora canal
    void checkShtSound();
};

#endif /* PMDeviceAudioAnalyzer_h */
