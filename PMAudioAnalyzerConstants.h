//
// Created by Miquel Àngel Soler on 8/11/15.
//

#ifndef PMCONCERTPARAULES_PMAUDIOANALYZERCONSTANTS_H
#define PMCONCERTPARAULES_PMAUDIOANALYZERCONSTANTS_H

// Pitch
static const float AUDIOANALYZER_PITCH_MINFREQ = 150.0f;
static const float AUDIOANALYZER_PITCH_MAXFREQ = 300.0f;
//static const float AUDIOANALYZER_PITCH_MINFREQ = 200.0f;
//static const float AUDIOANALYZER_PITCH_MAXFREQ = 300.0f;

// Energy
static const float AUDIOANALYZER_ENERGY_MIN = 0.0f;
static const float AUDIOANALYZER_ENERGY_MAX = 1.0f;

// Silence
static const float AUDIOANALYZER_SILENCE_THRESHOLD = -45.0f;

// Smoothing
static const float SMOOTHING_DELTA = 0.75f;


#endif //PMCONCERTPARAULES_PMAUDIOANALYZERCONSTANTS_H
