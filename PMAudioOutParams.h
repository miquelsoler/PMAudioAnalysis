//
//  PMAudioOutParams.h
//  PMConcertParaules
//
//  Created by Miquel Àngel Soler on 13/10/15.
//
//

#ifndef PMAudioOutParams_h
#define PMAudioOutParams_h

struct pitchParams
{
    float freq;
    float midiNote;
    float midiNoteNoOctave;
};

struct fftBandsParams
{
    float low;
    float mid;
    float high;
};


#endif /* PMAudioOutParams_h */
