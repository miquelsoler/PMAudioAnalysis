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

typedef enum
{
    PMDAA_CHANNEL_MULTI = 0,
    PMDAA_CHANNEL_MONO = 1
} PMDAA_ChannelMode;


class PMDeviceAudioAnalyzer : public ofBaseSoundInput
{
public:

    PMDeviceAudioAnalyzer(int deviceID, int inChannels, int outChannels, int sampleRate, int bufferSize);
    PMDeviceAudioAnalyzer() {};
    ~PMDeviceAudioAnalyzer();

    void setup(unsigned int audioInputIndex, PMDAA_ChannelMode channelMode, vector<unsigned int> channelNumbers,
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

    // Channel mode
    PMDAA_ChannelMode channelMode;
    vector<unsigned int> channelNumbers;

    // Pitch
    float minPitchMidiNote;
    float maxPitchMidiNote;

    // Silence
    bool wasSilent;
    float silenceThreshold;

    // Onsets
    float onsetsThreshold;
    // TODO: Deixa de ser vector
    vector<bool> oldOnsetState;

    // Smoothing
    float smoothingDelta;
    // TODO: Deixa de ser vector
    vector<float> oldMidiNotesValues;
    // TODO: Deixa de ser vector
    vector<deque<float> > midiNoteHistory;

    // Sound analysis

    ofSoundStream soundStream;

    // TODO: Deixa de ser vector
    vector<ofxAubioPitch *> vAubioPitches;
    // TODO: Deixa de ser vector
    vector<ofxAubioOnset *> vAubioOnsets;
    // TODO: Deixa de ser vector
    vector<ofxAubioMelBands *> vAubioMelBands;

    bool isSetup;

    // TODO: Deixa de ser vector
    vector<bool> isInSilence;
    // TODO: Deixa de ser vector
    vector<bool> isInPause;
    // TODO: Deixa de ser vector
    vector<float> silenceBeginTime;
    float silenceTimeTreshold;
    float pauseTimeTreshold;

    // TODO: Fora canal
    float getEnergy(unsigned int channel);
    // TODO: Fora canal, es manté input perque no ve directament d'Aubio
    float getRms(float *input, int bufferSize, int channel);
    // TODO: Fora canal, es manté input perque no ve directament d'Aubio
    float getAbsMean(float *input, int bufferSize, int channel);
    // TODO: Fora canal
    void detectedSilence(int channel);
    // TODO: Fora canal
    void updateSilenceTime(int channel);
    // TODO: Fora canal
    void detectedEndSilence(int channel);
    // TODO: Fora canal
    void checkMelodyDirection(int channel);

    int ascDescAnalysisSize;

    // sshht
    // TODO: Deixa de ser vector
    vector<bool> isShtSounding;
    // TODO: Deixa de ser vector
    vector<float> shtBeginTime;
    float shtTimeTreshold;
    // TODO: Deixa de ser vector
    vector<bool> isShtTrueSent;
    // TODO: Deixa de ser vector
    vector<bool> isShtFalseSent;

    void checkShtSound(int channel);
};

#endif /* PMDeviceAudioAnalyzer_h */
