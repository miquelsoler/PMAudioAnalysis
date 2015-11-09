//
//  PMAudioOutParams.h
//  PMConcertParaules
//
//  Created by Miquel Àngel Soler on 13/10/15.
//

#ifndef PMAudioOutParams_h
#define PMAudioOutParams_h

#pragma once

struct baseAudioInParams
{
    int deviceID;
    int channel;
};

struct pitchParams : public baseAudioInParams
{
    float freq;
    float confidence;
    float midiNote;
    float midiNoteNoOctave;
};

struct freqBandsParams : public baseAudioInParams
{
    float *melBands;
    int numBands;
};

struct onsetParams : public baseAudioInParams
{
};

struct silenceParams : public baseAudioInParams
{
    bool started;
    bool ended;
};


#endif /* PMAudioOutParams_h */
