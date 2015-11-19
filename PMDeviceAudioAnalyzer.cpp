//
//  PMDeviceAudioAnalyzer.cpp
//  ConcertParaules
//
//  Created by Miquel Àngel Soler on 25/9/15.
//
//

#include "PMDeviceAudioAnalyzer.hpp"

static const float SMOOTHING_INITIALVALUE = -999.0f;

// TODO: Should be able to change to a custom number
static const int NUM_MELBANDS = 40;

PMDeviceAudioAnalyzer::PMDeviceAudioAnalyzer(int _deviceID, int _inChannels, int _outChannels, int _sampleRate, int _bufferSize)
{
    deviceID = _deviceID;
    inChannels = _inChannels;
    outChannels = _outChannels;
    sampleRate = _sampleRate;
    bufferSize = _bufferSize;
//    numBuffers = _numBuffers;
    numBuffers = bufferSize/64;

    soundStream.setDeviceID(deviceID);
    soundStream.printDeviceList();

    isSetup = false;
}

PMDeviceAudioAnalyzer::~PMDeviceAudioAnalyzer()
{
//    for (int i=0; i<inChannels; ++i)
//        delete buffers[i];
//    delete []buffers;

//    for (unsigned int i=0; i<audioAnalyzers.size(); ++i)
//        delete audioAnalyzers[i];
//    audioAnalyzers.clear();
}

void PMDeviceAudioAnalyzer::setup(unsigned int _audioInputIndex, PMDAA_ChannelMode _channelMode, unsigned int _channelNumber,
        float _minPitchMidiNote, float _maxPitchMidiNote,
        float _energyThreshold,
        bool _useSilence, int silenceThreshold, unsigned int silenceQueueLength,
        float _onsetsThreshold, float _onsetsAlpha,
        float _smoothingDelta)
{
    if (isSetup) return;

    audioInputIndex = _audioInputIndex;

    // Channels
    channelMode = _channelMode;
    channelNumber = (channelMode == PMDAA_CHANNEL_MONO) ? _channelNumber : -1;
    unsigned int numUsedChannels = (unsigned int)((channelMode == PMDAA_CHANNEL_MONO) ? 1 : inChannels);

    // Pitch
    minPitchMidiNote = _minPitchMidiNote;
    maxPitchMidiNote = _maxPitchMidiNote;

    // Energy
    energyThreshold = _energyThreshold;

    // Silence
    useSilence = _useSilence;
    wasSilent = false;
    silenceTimeTreshold=silenceQueueLength;
    pauseTimeTreshold=1000;
    isInSilence.resize((unsigned long)numUsedChannels);
    isInPause.resize((unsigned long)numUsedChannels);
    silenceBeginTime.resize((unsigned long) numUsedChannels);

    //sht
    shtTimeTreshold=150;
    isShtSounding.resize((unsigned long) numUsedChannels);
    shtBeginTime.resize((unsigned long)numUsedChannels);

    // Onsets
    onsetsThreshold = _onsetsThreshold;
    onsetsAlpha = _onsetsAlpha;

    if (!oldOnsetState.empty())
        oldOnsetState.clear();
    oldOnsetState.assign(numUsedChannels, false);

    // Smoothing

    smoothingDelta = _smoothingDelta;

    if (!oldMidiNotesValues.empty())
        oldMidiNotesValues.clear();

    oldMidiNotesValues.assign(numUsedChannels, SMOOTHING_INITIALVALUE);

//    // Creation of audio in buffers
//    // Buffer matrix:
//    // - Rows: channels
//    // - Cols: channel buffer
//
//    buffers = new float *[numUsedChannels];
//    for (int i=0; i<numUsedChannels; ++i)
//        buffers[i] = new float[bufferSize];
//
//    // ofxAudioAnalyzer(s) setup
//
//    for (int i=0; i<numUsedChannels; ++i)
//    {
//        ofxAudioAnalyzer *analyzer = new ofxAudioAnalyzer();
//        analyzer->setup(bufferSize, sampleRate,
//                useMelBands, numMelBands,
//                useSilence, silenceThreshold, silenceQueueLength);
//        analyzer->setOnsetTreshold(onsetsThreshold);
//        analyzer->setOnsetAlpha(onsetsAlpha);
//
//        audioAnalyzers.push_back(analyzer);
//    }

//    aubioPitch.setup();

    for (int i=0; i<numUsedChannels; ++i)
    {
        ofxAubioPitch *aubioPitch = new ofxAubioPitch();
        aubioPitch->setup();
        vAubioPitches.push_back(aubioPitch);

        ofxAubioOnset *aubioOnset = new ofxAubioOnset();
        aubioOnset->setup();
        aubioOnset->setThreshold(onsetsThreshold);
        vAubioOnsets.push_back(aubioOnset);

        ofxAubioMelBands *aubioBands = new ofxAubioMelBands();
        aubioBands->setup();
        vAubioMelBands.push_back(aubioBands);
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

//    // Delete internal audio analyzer stuff
//    for (int i=0; i<audioAnalyzers.size(); ++i)
//        audioAnalyzers[i]->exit();
//
//    // Erase all audio analyzers from vector
//    for (int i=0; i<audioAnalyzers.size(); ++i)
//        delete audioAnalyzers[i];
//    audioAnalyzers.clear();
}

void PMDeviceAudioAnalyzer::audioIn(float *input, int bufferSize, int nChannels)
{
    int numUsedChannels = (channelMode == PMDAA_CHANNEL_MONO) ? 1 : inChannels;
    
//    for (int i=0; i<bufferSize*nChannels; i++){
//        input[i]*=5;
//    }

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
    

    for (unsigned int i =0; i <numUsedChannels; ++i)
    {
        // Compute aubio
        {
            vAubioPitches[i]->audioIn(input, bufferSize, nChannels);
            vAubioOnsets[i]->audioIn(input, bufferSize, nChannels);
            vAubioMelBands[i]->audioIn(input, bufferSize, nChannels);
        }

        int channel = (channelMode == PMDAA_CHANNEL_MONO) ? channelNumber : i;

        float currentMidiNote = vAubioPitches[i]->latestPitch;
        float currentPitchConfidence = vAubioPitches[i]->pitchConfidence;
        float modifiedSmoothingDelta=smoothingDelta*ofMap(currentPitchConfidence, 0.5, 1, 0, 1, true);
//        bool isSilent = (currentMidiNote == 0);
        bool isSilent = (getEnergy(i) < 0.001);

        // Silence
        if (wasSilent != isSilent) // Changes in silence (ON>OFF or OFF>ON)
        {
            wasSilent = isSilent;
            if(isSilent){
                detectedSilence(channel);
            }else{
                detectedEndSilence(channel);
            }
        }
        
//        cout<<"-----------------------"<<isInPause[channel]<<endl;
        if(isInSilence[channel])
            updateSilenceTime(channel);

        if (!isSilent)
        {
            // Pitch
            {
                if (currentMidiNote)
                {
                    if ((currentMidiNote > minPitchMidiNote) && (currentMidiNote < maxPitchMidiNote))
                    {
                        float smoothedMidiNote;

                        if (oldMidiNotesValues[i] == SMOOTHING_INITIALVALUE) {
                            smoothedMidiNote = currentMidiNote;
                        }  else {
                            smoothedMidiNote = (currentMidiNote * modifiedSmoothingDelta) + (oldMidiNotesValues[i] * (1.0f - modifiedSmoothingDelta));
                        }

                        pitchParams.channel = channel;
                        pitchParams.midiNote = smoothedMidiNote;
                        pitchParams.confidence = vAubioPitches[i]->pitchConfidence;
                        ofNotifyEvent(eventPitchChanged, pitchParams, this);
                    }
                    oldMidiNotesValues[i]=currentMidiNote;
                }
            }

            // Mel bands
            {
                energyParams.channel = channel;
                energyParams.energy = getEnergy(channel);
                ofNotifyEvent(eventEnergyChanged, energyParams, this);
            }
            
            // ssshhhht
            checkShtSound(channel);
            
            
        }

        bool isOnset = vAubioOnsets[i]->received();
        if (oldOnsetState[i] != isOnset)
        {
            oldOnsetState[i] = isOnset;
            onsetParams.channel = channel;
            onsetParams.isOnset = isOnset;
            ofNotifyEvent(eventOnsetStateChanged, onsetParams, this);
        }
    }


//    int numUsedChannels = (channelMode == PMDAA_CHANNEL_MONO) ? 1 : inChannels;
////    cout << "numUsedChannels: " << numUsedChannels << endl;
//
//    // Parse input array
//    for (int i=0; i<numUsedChannels; ++i)
//        for (int j=0; j<bufferSize; ++j)
//            buffers[i][j] = input[i + (nChannels * j)];
//
//    // Init of audio event params struct
//    pitchParams pitchParams;
//    pitchParams.deviceID = deviceID;
//    pitchParams.audioInputIndex = audioInputIndex;
//    silenceParams silenceParams;
//    silenceParams.deviceID = deviceID;
//    silenceParams.audioInputIndex = audioInputIndex;
//    energyParams energyParams;
//    energyParams.deviceID = deviceID;
//    energyParams.audioInputIndex = audioInputIndex;
//    onsetParams onsetParams;
//    onsetParams.deviceID = deviceID;
//    onsetParams.audioInputIndex = audioInputIndex;
//    freqBandsParams freqBandsParams;
//    freqBandsParams.deviceID = deviceID;
//    freqBandsParams.audioInputIndex = audioInputIndex;
//
//    for (int i=0; i<numUsedChannels; ++i)
//    {
//        audioAnalyzers[i]->analyze(buffers[i], bufferSize);
//
//        int channel = (channelMode == PMDAA_CHANNEL_MONO) ? channelNumber : i;
//
//        bool isSilent = audioAnalyzers[i]->getIsSilent();
//
//        // Silence
//
//        if (wasSilent != isSilent) // Changes in silence (ON>OFF or OFF>ON)
//        {
//            wasSilent = isSilent;
//            silenceParams.channel = channel;
//            silenceParams.isSilent = isSilent;
//            ofNotifyEvent(eventSilenceStateChanged, silenceParams, this);
//        }
//
//        // Process only when no silence detected
//        if (!isSilent)
//        {
//            // Energy
//            {
//                energyParams.channel = channel;
//                energyParams.energy = audioAnalyzers[i]->getEnergy();
//                ofNotifyEvent(eventEnergyChanged, energyParams, this);
//            }
//
//            // Pitch
//            {
//                float smoothedPitchFreq, currentPitchFreq;
//
//                currentPitchFreq = audioAnalyzers[i]->getPitchFreq();
//
//                if ((currentPitchFreq > minPitchFreq) && (currentPitchFreq < maxPitchFreq)) // Skip ultra high or ultra low pitch frequencies
//                {
//                    if (oldPitchFreqValues[i] == SMOOTHING_INITIALVALUE)
//                    {
//                        smoothedPitchFreq = currentPitchFreq;
//                    } else {
//                        smoothedPitchFreq = (currentPitchFreq * smoothingDelta) + (oldPitchFreqValues[i] * (1.0f - smoothingDelta));
//                    }
//
//                    oldPitchFreqValues[i] = smoothedPitchFreq;
//
//                    pitchParams.channel = channel;
//                    pitchParams.freq = smoothedPitchFreq;
//                    pitchParams.confidence = audioAnalyzers[i]->getPitchConf();
//                    pitchParams.midiNote = 0;
//                    pitchParams.midiNoteNoOctave = 0;
//                    ofNotifyEvent(eventPitchChanged, pitchParams, this);
//                }
//            }
//
//            // Frequency bands
//            {
//                freqBandsParams.channel = channel;
//                freqBandsParams.melBands = audioAnalyzers[i]->getMelBands();
//                freqBandsParams.numBands = numMelBands;
//                ofNotifyEvent(eventFreqBandsParams, freqBandsParams, this);
//            }
//
//            // Onsets
//            {
//                bool isOnset = audioAnalyzers[i]->getIsOnset();
//                if (oldOnsetState[i] != isOnset)
//                {
//                    oldOnsetState[i] = isOnset;
//                    onsetParams.channel = channel;
//                    onsetParams.isOnset = isOnset;
//                    ofNotifyEvent(eventOnsetStateChanged, onsetParams, this);
//                }
//            }
//        }
//    }
}

float PMDeviceAudioAnalyzer::getEnergy(unsigned int channel)
{
    float *energies = vAubioMelBands[channel]->energies;

    float result = 0.0f;

    for (int i=0; i<NUM_MELBANDS; i++)
    {
        result+=energies[i];
    }
    
    result/=NUM_MELBANDS; //Applied vector aritmetic mean https://en.wikipedia.org/wiki/Weighted_arithmetic_mean
    return result;
}

void PMDeviceAudioAnalyzer::detectedSilence(int channel)
{
    silenceBeginTime[channel]=ofGetElapsedTimeMillis();
    isInSilence[channel]=true;
}

void PMDeviceAudioAnalyzer::updateSilenceTime(int channel)
{
    float timeOfSilence = ofGetElapsedTimeMillis()-silenceBeginTime[channel];
    if(timeOfSilence > silenceTimeTreshold){
        silenceParams silenceParams;
        silenceParams.deviceID = deviceID;
        silenceParams.audioInputIndex = audioInputIndex;
        silenceParams.channel = channel;
        silenceParams.isSilent = true;
        silenceParams.silenceTime = 0;
        ofNotifyEvent(eventSilenceStateChanged, silenceParams, this);
    }
    if(timeOfSilence > pauseTimeTreshold){
        pauseParams pauseParams;
        pauseParams.deviceID = deviceID;
        pauseParams.audioInputIndex = audioInputIndex;
        pauseParams.channel = channel;
        pauseParams.isPause = true;
        pauseParams.pauseTime = 0;
        ofNotifyEvent(eventPauseStateChanged, pauseParams, this);
        isInPause[channel]=true;
    }
}

void PMDeviceAudioAnalyzer::detectedEndSilence(int channel)
{
    silenceParams silenceParams;
    silenceParams.deviceID = deviceID;
    silenceParams.audioInputIndex = audioInputIndex;
    float timeOfSilence = ofGetElapsedTimeMillis()-silenceBeginTime[channel];
    if(timeOfSilence > silenceTimeTreshold){
        silenceParams.channel = channel;
        silenceParams.isSilent = false;
        silenceParams.silenceTime = timeOfSilence;
        ofNotifyEvent(eventSilenceStateChanged, silenceParams, this);
    }
    if(isInPause[channel]){
        pauseParams pauseParams;
        pauseParams.deviceID = deviceID;
        pauseParams.audioInputIndex = audioInputIndex;
        pauseParams.channel = channel;
        pauseParams.isPause = true;
        pauseParams.pauseTime = timeOfSilence;
        ofNotifyEvent(eventPauseStateChanged, pauseParams, this);
    }
    isInPause[channel]=false;
    isInSilence[channel]=false;
}

void PMDeviceAudioAnalyzer::checkShtSound(int channel)
{
    float *melbands = vAubioMelBands[channel]->energies;
    float lowBands=0.0f;
    float highBands=0.0f;
    int high_low_limit=NUM_MELBANDS*2/3;
    for(int i=0; i<high_low_limit; i++){
        lowBands+=melbands[i];
    }
    lowBands/=high_low_limit;
    for(int i=high_low_limit; i<NUM_MELBANDS; i++){
        highBands+=melbands[i];
    }
    highBands/=(NUM_MELBANDS-high_low_limit);
    
    
    if(highBands > 3*lowBands && !isShtSounding[channel]){
        shtBeginTime[channel]=ofGetElapsedTimeMillis();
        isShtSounding[channel]=true;
    }else if(highBands<3*lowBands){
        isShtSounding[channel]=false;
    }
    
    
    float timeOfSht=ofGetElapsedTimeMillis()-shtBeginTime[channel];
    if(isShtSounding[channel] &&  timeOfSht > shtTimeTreshold){
        shtParams shtParams;
        shtParams.deviceID=deviceID;
        shtParams.audioInputIndex=audioInputIndex;
        shtParams.channel=channel;
        shtParams.time=timeOfSht;
        ofNotifyEvent(eventShtHappened, shtParams, this);
//        cout<<"-------------------SHT-----------------"<<endl;
    }
    
}
