//
//  PMAudioOutParams.h
//  PMConcertParaules
//
//  Created by Miquel Àngel Soler on 13/10/15.
//

#ifndef PMAudioOutParams_h
#define PMAudioOutParams_h

#pragma once

struct baseAudioOutParams
{
    int deviceID;
    int channel;
};

struct pitchParams : public baseAudioOutParams
{
    float freq;
    float midiNote;
    float midiNoteNoOctave;
};

struct onsetParams : public baseAudioOutParams
{
};

struct freqBandsParams : public baseAudioOutParams
{
    float *melBands;
    int numBands;
};


#endif /* PMAudioOutParams_h */
