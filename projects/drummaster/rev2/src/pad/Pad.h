#ifndef PAD_H
#define PAD_H

#include <math.h>

#include <ADC.h>

#include "../Sample.h"
#include "../hardware.h"

#define MUX_0		0
#define MUX_1		1
#define MUX_2		2
#define MUX_3		3
#define MUX_4		4
#define MUX_5		5
#define MUX_6		6
#define MUX_7		7
#define MUX_8		8
#define MUX_9		9
#define MUX_10		10
#define MUX_11		11
#define MUX_12		12
#define MUX_13		13
#define MUX_14		14
#define MUX_15		15

//Before a signal is considered stable, there must be STABILIZATION_COUNT samples
// all within +/- STABILIZATION_DELTA of the previous one.
#define STABILIZATION_COUNT			5
#define STABILIZATION_DELTA			2

//The maximum time (in ms) between a new hit being detected and when we return
// the value.
#define MAX_RESPONSE_TIME			3

//Minimum ADC value to register as a hit
#define MIN_VALUE					8

namespace digitalcave {

	class Pad {
		private:
			//ADC Object
			static ADC adc;
			
			//Set this to 1 once the ramdom number generator has been seeded.  We seed it on the first 
			// pad strike with the system time; this should allow for random numbers, as the timing of 
			// the first pad strike will differ each time.
			static uint8_t randomSeedCompleted;
			
			//Index to keep track of current index (for pad constructor).
			static uint8_t currentIndex;

			/*** Variables used in determining proper file name ***/
			//The number of sample files which can be played at each volume interval.
			// We support 16 volumes (represented by a single hex digit 0..F in the filename).
			// At each volume we can have up to 255 samples, which will be chosen randomly.
			//This array is instantiated by calling updateSamples() method.
			//Most of the time the value will be either 0 or 1.
			uint8_t fileCountByVolume[16];

			//Variables used internally for getting sample filenames
			char filenamePrefix[3];
			char filenameResult[16];
			

			/*** Variables used in reading the pizeo value ***/
// 			//Stabilization counter and last value, to ensure accurate and stable readings
// 			uint8_t stabilizationCounter;
// 			int16_t stabilizationValue;

			//The time at which this hit was first read.  We must return a value within
			// at most MAX_RESPONSE_TIME ms from this time.
			uint32_t strikeTime;
			//The peak value read from the ADC for this particular strike.
			// We repeatedly read the ADC, keeping track of the peak value, until the 
			// readings have stabilized or the maximum time has passed.
			int16_t peakValue;
			//uint32_t peakValueTime;
			//The time at which the last value was considered stable and returned
			uint32_t playTime;
			
			//Maximum time in ms after a sample has been played before we can play another one.
			uint8_t doubleHitThreshold;

			/*** Internal state ***/
			//The per-pad volume gain.  This number is divided by 64.0 and multiplied against the requested
			// volume when playing a sample.  A value of 64 results in no change (multiplier of 1); less than
			// 64 will reduce the volume, and greater than 64 will increase it.
			uint8_t padVolume;
			
			/*** Private methods ***/
			//Looks on the SPI flash chip for files according to the sample naming convention,
			// and updates the fileCountByVolume array, which indicates which files are 
			// available.
			void updateSamples();
			
			//Find the correct sample, given a volume.
			char* lookupFilename(uint8_t volume);
			
		protected:
			//The last Sample which was used to play this pad.  Used in the event that we need
			// to adjust the playback on the last Sample
			Sample* lastSample;

			//The index into the pads array for this instance
			uint8_t padIndex;
			
			//Returns the strike velocity.  Handles the ADC, draining, etc.
			uint8_t readPiezo(uint8_t muxIndex);
			
			//Returns the switch state: 0 for open (not pressed), 1 for closed (pressed)
			uint8_t readSwitch(uint8_t muxIndex);

			
		public:
			//All pads in the system.
			static Pad* pads[PAD_COUNT];

			//Initialize the ADC
			static void init();
		
			//Calls updateSamples() for each Pad object defined.
			static void updateAllSamples();
			
			//Constructor
			Pad(const char* filenamePrefix, uint8_t doubleHitThreshold);
			
			//Method called repeatedly from main code
			virtual void poll() = 0;
			
			//Start plating this pad at the specified volume.
			void play(uint8_t volume);
			
			//Stops all samples which were started from this pad
			void stop();
			
			//Get the per-pad volume.
			uint8_t getPadVolume();

			//Set the per-pad volume.
			void setPadVolume(uint8_t padVolume);
	};
	
}

#endif