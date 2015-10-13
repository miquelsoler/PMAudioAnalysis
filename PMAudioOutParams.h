//
//  PMAudioOutParams.h
//  PMConcertParaules
//
//  Created by Miquel Àngel Soler on 13/10/15.
//
//

#ifndef PMAudioOutParams_h
#define PMAudioOutParams_h

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

struct fftBandsParams : public baseAudioOutParams
{
    float low;
    float mid;
    float high;
};


#endif /* PMAudioOutParams_h */
