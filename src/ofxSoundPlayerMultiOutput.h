#pragma once

#include "ofMain.h"
//#include "ofBaseSoundPlayer.h"

extern "C" {
#include "fmod.h"
#include "fmod_errors.h"
}

//		TO DO :
//		---------------------------
// 		-fft via fmod, as in the last time...
// 		-close fmod if it's up
//		-loadSoundForStreaming(char * fileName);
//		---------------------------

// 		interesting:
//		http://www.compuphase.com/mp3/mp3loops.htm


// ---------------------------------------------------------------------------- SOUND SYSTEM FMOD

// --------------------- global functions:
namespace ofModMulti {
    
    void    ofFmodSoundStopAll();
    void    ofFmodSoundSetVolume(float vol);
    void    ofFmodSoundUpdate();						// calls FMOD update.
    float * ofFmodSoundGetSpectrum(int nBands);		// max 512...
    void    ofFmodSelectDriver(int driver);
    void    ofFmodSetNumOutputs(int num);				// not sure of this
    int     ofFmodGetNumOutputs();				// not sure of this
    int     ofFmodGetNumDrivers();
    int     ofFmodGetDriverSelected();
    void    ofFmodErrorCheck(FMOD_RESULT result);
    
}


// --------------------- player functions:
class ofxSoundPlayerMultiOutput  {

	public:

		ofxSoundPlayerMultiOutput();

        bool    loadSound(std::string fileName, bool stream = false);
		void    unloadSound();
		void    play();
        void    setChannelLevels();
		void    stop();
    
        void    playTo(int speaker);
        void    playTo(int speaker0, int speaker1);
        void    playTo(int* speakers, int numSpeakers);
    
		void    setVolume(float vol);
        void    setVolumeAt(int index, float volume);
    
		void    setPan(float vol);
		void    setSpeed(float spd);
		void    setPaused(bool bP);
		void    setLoop(bool bLp);
		void    setMultiPlay(bool bMp);
		void    setPosition(float pct); // 0 = start, 1 = end;
		void    setPositionMS(int ms);

		float   getPosition();
		int     getPositionMS();
		bool    getIsPlaying();
		float   getSpeed();
		float   getPan();
		float   getVolume();
		bool    isLoaded();

		static void initializeFmod();
		static void closeFmod();

		bool            isStreaming;
		bool            bMultiPlay;
		bool            bLoop;
		bool            bLoadedOk;
		bool            bPaused;
		float           pan; // -1 to 1
		float           volume; // 0 - 1
		float           internalFreq; // 44100 ?
		float           speed; // -n to n, 1 = normal, -1 backwards
		unsigned int    length; // in samples;
    
        float           levels[8];
    
		FMOD_RESULT result;
		FMOD_CHANNEL * channel;
		FMOD_SOUND * sound;
};

