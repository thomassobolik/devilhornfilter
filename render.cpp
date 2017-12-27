/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\


Tom's Elektron Filter w/ modulation

User can control cutoff, Q's, and band size of the 'devil horn' filter 
A toggleable LFO (indicated by the LED) modulates the base frequency on command

POTS:
0: base frequency
1: width constant
2: Q of both filters 
3: LFO frequency 

SWITCHES:
P8_08: LFO on/off 
P8_07: LFO LED indicator

--- 
Tom Sobolik, Musi 4545, University of Virginia, September 2017.
*/


// Include any other files or libraries we need -- -- -- -- -- -- -- -- -- 
#include <Bela.h>
#include <stdlib.h>
#include <BiQuad.h>
#include <Mu45FilterCalc.h>
#include <Mu45LFO.h>

// Defines -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#define FCMIN_HZ 40.0
#define FCMAX_HZ 10000.0
#define FCMIN_NN 27.000
#define FCMAX_NN 123.0


// Declare global variables   -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
int gAudioFramesPerAnalogFrame;
int gBasePot = 0;
int gWidthPot = 1;
int gQPot = 2;

// instantiate filters	-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
stk::BiQuad hiPass, loPass;
Mu45LFO LFO;

float NN2Hz(float NN) {
	return 440.0 * powf(2, (NN-60.0) / 12.0);
}

// Setup and initialize stuff -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
bool setup(BelaContext *context, void *userData)
{
	gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
	
	// Set the digital pins to input or output
	pinMode(context, 0, P8_08, INPUT);
	pinMode(context, 0, P8_07, OUTPUT);
	
	return true;
}


// Render each block of input data  -- -- -- -- -- -- -- -- -- -- -- -- --
void render(BelaContext *context, void *userData)
{
	// Declare local variables
	float out_l, out_r;
	float baseFreq, width, LPFreq;
	float filtQ;
	float HPcoeffs[5];
	float LPcoeffs[5]; // an array of 2 floats for the filter coefficients, b0 and a1;
	
	// These calculations we can do once per buffer
	int lastDigFrame = context->digitalFrames - 1;					// use the last digital frame
	int switchStatus = digitalRead(context, lastDigFrame, P8_08);  	// read the value of the switch
	digitalWriteOnce(context, lastDigFrame, P8_07, switchStatus); 	// write the status to the LED
	if(switchStatus) {
	for(unsigned int n = 0; n < context->audioFrames; n++) 
	{
		if(!(n % gAudioFramesPerAnalogFrame)) 
		{
			// Read parameters from potentiometer values
			baseFreq = analogRead(context, n/gAudioFramesPerAnalogFrame, gBasePot);	
			baseFreq = map(baseFreq, 0.0001, 0.827, FCMIN_NN, FCMAX_NN);
			baseFreq = NN2Hz(baseFreq);
			
			filtQ = analogRead(context, n/gAudioFramesPerAnalogFrame, gQPot);	
			filtQ = map(filtQ, 0.0001, 0.827, 0.6, 5.0);
			
			width = analogRead(context, n/gAudioFramesPerAnalogFrame, gWidthPot);
			width = map(width, 0.0001, 0.827, 20, 3000);
			
			// make sure the low pass frequency is still in bounds
			if(baseFreq + width > 19900) {
				LPFreq = 19900;
			}
			else LPFreq = baseFreq + width;
			
			// calculate and set the coefficients for the hi pass filter
		    Mu45FilterCalc::calcCoeffsHPF (HPcoeffs, baseFreq, filtQ, context->audioSampleRate);
			hiPass.setCoefficients(HPcoeffs[0], HPcoeffs[1], HPcoeffs[2], HPcoeffs[3], HPcoeffs[4]);
			
			// calculate and set the coefficients for the lo pass filter
		    Mu45FilterCalc::calcCoeffsLPF (LPcoeffs, LPFreq, filtQ, context->audioSampleRate);
			loPass.setCoefficients(LPcoeffs[0], LPcoeffs[1], LPcoeffs[2], LPcoeffs[3], LPcoeffs[4]);
		}
		// Audio rate stuff here...
		// Get input audio samples
		out_l = audioRead(context,n,0);
        out_r = audioRead(context,n,1);
        
        // Process the audio
        out_l = hiPass.tick(out_l);
        out_l = loPass.tick(out_l);
        out_r = hiPass.tick(out_r);
        out_r = loPass.tick(out_r);
        
        // Write output audio samples
        audioWrite(context, n, 0, out_l);
        audioWrite(context, n, 1, out_r);
	}
	}
	else {
		
			// Simplest possible case: pass inputs through to outputs
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		for(unsigned int ch = 0; ch < context->audioInChannels; ch++){
			// Two equivalent ways to write this code

			// The long way, using the buffers directly:
			// context->audioOut[n * context->audioOutChannels + ch] =
			// 		context->audioIn[n * context->audioInChannels + ch];

			// Or using the macros:
			audioWrite(context, n, ch, audioRead(context, n, ch));
		}
	}
		
	}
}


void cleanup(BelaContext *context, void *userData)
{
	// Nothing to do here
}


