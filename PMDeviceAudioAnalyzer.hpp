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

typedef enum {
    PMDAA_CHANNEL_MULTI = 0,
    PMDAA_CHANNEL_MONO = 1
} PMDAA_ChannelMode;


class PMDeviceAudioAnalyzer : public ofBaseSoundInput {
public:

    /**
     * PMDeviceAudioAnalyzer(...)
     * Constructor just sets sound stream attributes. Calling it doesn't start the sound stream analysis.
     */
    PMDeviceAudioAnalyzer(int deviceID, int inChannels, int outChannels, int sampleRate, int bufferSize);

    PMDeviceAudioAnalyzer() {};

    ~PMDeviceAudioAnalyzer();

    /**
     * setup:
     * - channelMode: mono or multichannel
     * - channelNumber: 0..N (ignored when in multichannel mode)
     * - useMelBands: true if it needs obtaining mel bands
     * - numMelBands: number of mel bands (ignored when useMelBands=false)
     */
    void setup(unsigned int audioInputIndex, PMDAA_ChannelMode channelMode, unsigned int channelNumber,
            float energyThreshold,
            bool useSilence, float silenceThreshold, unsigned int silenceQueueLength,
            float onsetsThreshold, float onsetsAlpha,
            float smoothingDelta, int ascDescAnalysisSize);

    void start();
    void stop();
    void clear();
    void audioIn(float *input, int bufferSize, int nChannels);

    unsigned int getInputIndex() { return audioInputIndex; };

    int getDeviceID() { return deviceID; };
    int getChannelNumber() { return channelNumber; };
    int getSamplerate();
    int getNumChannels();

    // Events for listeners
    ofEvent<pitchParams> eventPitchChanged;
    ofEvent<silenceParams> eventSilenceStateChanged;
    ofEvent<energyParams> eventEnergyChanged;
    ofEvent<onsetParams> eventOnsetStateChanged;
    ofEvent<freqBandsParams> eventFreqBandsParams;
    ofEvent<shtParams> eventShtStateChanged;
    ofEvent<pauseParams> eventPauseStateChanged;
    ofEvent<melodyDirectionParams> eventMelodyDirection;

    // Setters
    void setSilenceThreshold(float _f) {silenceThreshold=_f;};
    void setSilenceQueueLength(float _f) {silenceTimeTreshold=_f;};
    void setPauseTimeTreshold(float _f) {pauseTimeTreshold=_f;};
    void setOnsetsThreshold(float _f) {onsetsThreshold = _f;};
    void setOnsetsAlpha(float _f) {onsetsAlpha = _f;};
        
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
    int channelNumber;

    // Pitch
    float minPitchMidiNote;
    float maxPitchMidiNote;

    // Energy
    float energyThreshold;

    // Silence
    bool useSilence;
    bool wasSilent;
    float silenceThreshold;

    // Onsets
    float onsetsThreshold;
    float onsetsAlpha;
    vector<bool> oldOnsetState;

    // Smoothing
    float smoothingDelta;
    vector<float> oldMidiNotesValues;
    vector<deque<float> > midiNoteHistory;

    // Sound analysis

    ofSoundStream soundStream;

    vector<ofxAubioPitch *> vAubioPitches;
    vector<ofxAubioOnset *> vAubioOnsets;
    vector<ofxAubioMelBands *> vAubioMelBands;

    bool isSetup;

    vector<bool> isInSilence;
    vector<bool> isInPause;
    vector<float> silenceBeginTime;
    float silenceTimeTreshold;
    float pauseTimeTreshold;

    float getEnergy(unsigned int channel);
    float getRms(float *input, int bufferSize, int channel);
    float getAbsMean(float *input, int bufferSize, int channel);
    void detectedSilence(int channel);
    void updateSilenceTime(int channel);
    void detectedEndSilence(int channel);
    void checkMelodyDirection(int channel);

    int ascDescAnalysisSize;

    // sshht
    vector<bool> isShtSounding;
    vector<float> shtBeginTime;
    float shtTimeTreshold;
    vector<bool> isShtTrueSent;
    vector<bool> isShtFalseSent;

    void checkShtSound(int channel);
};

#endif /* PMDeviceAudioAnalyzer_h */
