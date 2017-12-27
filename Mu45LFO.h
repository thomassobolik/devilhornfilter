//
//  Mu45LFO.h
//
//  Created by Luke Dahl on 3/4/15.

// Mu45LFO is a low-frequency oscillator with basic functionality.
// The LFO outputs a sinusoidal waveform, which is created by interpolating
// a 1024-sample wavetable.

#ifndef __mu45_Mu45LFO__
#define __mu45_Mu45LFO__
#include <stdio.h>
#endif /* defined(__mu45_Mu45LFO__) */


class Mu45LFO {
public:
    Mu45LFO();                              // Constructor
    void setFreq(float freq, float fs);     // Set the frequency of the oscillator.
    float tick();                           // Generate a sample of output and update the state
    void resetPhase();                      // Set the oscillator phase to 0
    
private:
    static const int N = 1024;      // size of the wavetable
    float table[N];                 // the wavetable
    float phase_inc;                // amount to increment phase each tick
    float phase;                    // current index into the wavetable
};
