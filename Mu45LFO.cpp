//
//  Mu45LFO.cpp
//
//  Created by Luke Dahl on 3/4/15.


#include "Mu45LFO.h"
#include <cmath>

// constructor
Mu45LFO::Mu45LFO()
{
    float pi = 3.1415926535897932384626433832795;
    
    // initialize a table with a sine waveform
    for (int i = 0; i < N; i++) {
        table[i] = sin(2*pi*i/N);
    }
    
    // init other stuff
    setFreq(1.0, 44100);
    resetPhase();
}

// set the frequency of the LFO (freq) and how often it will be called (fs). Both values are in Hz.
void Mu45LFO::setFreq(float freq, float fs)
{
    phase_inc = N * freq / fs;
}

// reset the phase, so that the next output is at the beginning of the wavetable
void Mu45LFO::resetPhase()
{
    phase = 0.0;
}

// update the phase of the LFO and produce an output sample
float Mu45LFO::tick()
{
    // calc output
    float outSamp, frac;
    int idx1, idx2;         // the indeces to the two table entries we will interpolate between
    
    idx1 = floor(phase);    // the first index is the integer part of phase
    idx2 = idx1 + 1;        // the second index is the next table entry
    if (idx2 >= N) {        // unless that is greater than the table length...
        idx2 = 0;           // in which case wrap around
    }
    frac = phase - idx1;    // the fraction of the index, i.e. the amount to interpolate
    
    // linearly interpolate between adjacent table entries
    outSamp  = (1-frac)*table[idx1] + frac*table[idx2];
    
    // update phase
    phase += phase_inc;
    if (phase >= N) {    // wrap around
        phase -= N;
    }
    
    return outSamp;
}













