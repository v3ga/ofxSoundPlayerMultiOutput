#include "ofxSoundPlayerMultiOutput.h"
#include "ofUtils.h"


namespace ofModMulti {
    
bool bFmodInitialized_ = false;
bool bUseSpectrum_ = false;
float fftValues_[8192];			//
float fftInterpValues_[8192];			//
float fftSpectrum_[8192];		// maximum #ofxSoundPlayerMultiOutput is 8192, in fmodex....


// ---------------------  static vars
static FMOD_CHANNELGROUP * channelgroup;
static FMOD_SYSTEM       * sys;
static int				 numDrivers=0;
static int 				 driverFMOD=0;
static int			     numOutputsFMOD=16;

// these are global functions, that affect every sound / channel:
// ------------------------------------------------------------
// ------------------------------------------------------------

//--------------------
void ofFmodSoundStopAll()
{
    ofxSoundPlayerMultiOutput::initializeFmod();
    FMOD_ChannelGroup_Stop(channelgroup);
}

//--------------------
void ofFmodSoundSetVolume(float vol)
{
    ofxSoundPlayerMultiOutput::initializeFmod();
    FMOD_ChannelGroup_SetVolume(channelgroup, vol);
}

//--------------------
void ofFmodSoundUpdate()
{
    if (bFmodInitialized_)
    {
        FMOD_System_Update(sys);
    }
}

//--------------------
void ofFmodErrorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        ofLog(OF_LOG_ERROR) << "FMOD error! " << ofToString( FMOD_ErrorString(result) );
    }
}

//--------------------
void ofFmodSetNumOutputs(int num)
{
	numOutputsFMOD = num;
}

//--------------------
int  ofFmodGetNumOutputs()
{
	return numOutputsFMOD;
}


//--------------------
void ofFmodSelectDriver(int driver)
{
	driverFMOD = driver;
}

//--------------------
int ofFmodGetNumDrivers()
{
	return numDrivers;
}

//--------------------
int ofFmodGetDriverSelected()
{
	return driverFMOD;
}


//--------------------
float * ofFmodSoundGetSpectrum(int nBands)
{

    ofxSoundPlayerMultiOutput::initializeFmod();


    // 	set to 0
    for (int i = 0; i < 8192; i++)
    {
        fftInterpValues_[i] = 0;
    }

    // 	check what the user wants vs. what we can do:
    if (nBands > 8192)
    {
        ofLogWarning("ofxSoundPlayerMultiOutput") << "ofFmodGetSpectrum(): requested number of bands " << nBands << ", using maximum of 8192";
        nBands = 8192;
    }
    else if (nBands <= 0)
    {
        ofLogWarning("ofxSoundPlayerMultiOutput") << "ofFmodGetSpectrum(): requested number of bands " << nBands << ", using minimum of 1";
        nBands = 1;
        return fftInterpValues_;
    }

    // 	FMOD needs pow2
    int nBandsToGet = ofNextPow2(nBands);
    if (nBandsToGet < 64) nBandsToGet = 64;  // can't seem to get fft of 32, etc from fmodex

    // 	get the fft
    FMOD_System_GetSpectrum(sys, fftSpectrum_, nBandsToGet, 0, FMOD_DSP_FFT_WINDOW_HANNING);

    // 	convert to db scale
    for(int i = 0; i < nBandsToGet; i++)
    {
        fftValues_[i] = 10.0f * (float)log10(1 + fftSpectrum_[i]) * 2.0f;
    }

    // 	try to put all of the values (nBandsToGet) into (nBands)
    //  in a way which is accurate and preserves the data:
    //

    if (nBandsToGet == nBands)
    {

        for(int i = 0; i < nBandsToGet; i++)
        {
            fftInterpValues_[i] = fftValues_[i];
        }

    }
    else
    {

        float step 		= (float)nBandsToGet / (float)nBands;
        //float pos 		= 0;
        // so for example, if nBands = 33, nBandsToGet = 64, step = 1.93f;
        int currentBand = 0;

        for(int i = 0; i < nBandsToGet; i++)
        {

            // if I am current band = 0, I care about (0+1) * step, my end pos
            // if i > endPos, then split i with me and my neighbor

            if (i >= ((currentBand+1)*step))
            {

                // do some fractional thing here...
                float fraction = ((currentBand+1)*step) - (i-1);
                float one_m_fraction = 1 - fraction;
                fftInterpValues_[currentBand] += fraction * fftValues_[i];
                currentBand++;
                // safety check:
                if (currentBand >= nBands)
                {
                    ofLogWarning("ofxSoundPlayerMultiOutput") << "ofFmodGetSpectrum(): currentBand >= nBands";
                }

                fftInterpValues_[currentBand] += one_m_fraction * fftValues_[i];

            }
            else
            {
                // do normal things
                fftInterpValues_[currentBand] += fftValues_[i];
            }
        }

        // because we added "step" amount per band, divide to get the mean:
        for (int i = 0; i < nBands; i++)
        {
            fftInterpValues_[i] /= step;
            if (fftInterpValues_[i] > 1)fftInterpValues_[i] = 1; 	// this seems "wrong"
        }

    }

    return fftInterpValues_;
}

    
}
// ------------------------------------------------------------
// ------------------------------------------------------------


using namespace ofModMulti;
// now, the individual sound player:
//------------------------------------------------------------
ofxSoundPlayerMultiOutput::ofxSoundPlayerMultiOutput()
{
    bLoop 			= false;
    bLoadedOk 		= false;
    pan 			= 0.0f; // range for oF is -1 to 1
    volume 			= 1.0f;
    internalFreq 	= 44100;
    speed 			= 1;
    bPaused 		= false;
    isStreaming		= false;
}


//---------------------------------------
// this should only be called once
void ofxSoundPlayerMultiOutput::initializeFmod()
{
    FMOD_SPEAKERMODE  	speakermode;
    
   

    if(!bFmodInitialized_)
    {
		ofLog() << "FMOD initializing";
     
	 	// Create FMOD system
		ofFmodErrorCheck( FMOD_System_Create(&sys) );
        
        unsigned int bsTmp;
        int nbTmp;
        FMOD_System_GetDSPBufferSize(sys, &bsTmp, &nbTmp);
        FMOD_System_SetDSPBufferSize(sys, 1024, nbTmp);

        // FMOD_System_SetDriver(sys, 7); // setup driver 7.1 with index 7
				
		// Get FMOD Version
		unsigned int version = 0;
		ofFmodErrorCheck( FMOD_System_GetVersion(sys, &version) );
		ofLog() << " - version = " << ofToString(version);

		// Drivers info
		numDrivers = 0;
		ofFmodErrorCheck( FMOD_System_GetNumDrivers(sys, &numDrivers) );
		ofLog() << " - found "<< ofToString(numDrivers) << " sound drivers ";


		// Drivers info / caps print
		char driverName[256];
		FMOD_GUID driverGuid;
		FMOD_CAPS driverCaps;
		int driverMinFreq = 0;
		int driverMaxFreq = 0;
		FMOD_SPEAKERMODE driverSpeakerMode;

		for (int id=0; id<numDrivers; id++)
		{
			// Driver info
			// TODO : check link error ?
			FMOD_System_GetDriverInfo(sys, id, driverName, 256, &driverGuid);

			// Driver Caps
			// ofFmodErrorCheck( FMOD_System_GetDriverCaps(sys, id, &driverCaps, &driverMinFreq, &driverMaxFreq, &driverSpeakerMode) );

			// log
			ofLog() << "   - [" << driverName << " " << ofToString(id) << "] speakerMode=" << ofToString((int)driverSpeakerMode) << "; minFreq = " << ofToString(driverMinFreq) << "; maxFreq = " <<  ofToString(driverMaxFreq);
		}

		// Setting driver
		if (driverFMOD >= numDrivers)
		{
			driverFMOD = 0;
			ofLog(OF_LOG_WARNING) << " - chosen driver ["<< driverFMOD <<"] is invalid, setting to default [0]";
		}
        
        
#ifdef TARGET_OSX
		ofFmodErrorCheck( FMOD_System_SetDriver(sys, driverFMOD) );
		ofFmodErrorCheck( FMOD_System_SetSoftwareFormat(sys, 44100, FMOD_SOUND_FORMAT_NONE, numOutputsFMOD, 0,FMOD_DSP_RESAMPLER_LINEAR) );
#endif
		// Speaker mode
        ofFmodErrorCheck( FMOD_System_SetSpeakerMode(sys, FMOD_SPEAKERMODE_5POINT1) );




#ifdef TARGET_LINUX
        ofFmodErrorCheck( FMOD_System_SetOutput(sys,FMOD_OUTPUTTYPE_ALSA) );
#endif

        ofFmodErrorCheck( FMOD_System_Init(sys, 32, FMOD_INIT_NORMAL, NULL) );  //do we want just 32 channels?
        ofFmodErrorCheck( FMOD_System_GetMasterChannelGroup(sys, &channelgroup) );
        bFmodInitialized_ = true;
    }
}


//---------------------------------------
void ofxSoundPlayerMultiOutput::closeFmod()
{
    if(bFmodInitialized_)
    {
        FMOD_System_Close(sys);
        bFmodInitialized_ = false;
    }
}

//------------------------------------------------------------
bool ofxSoundPlayerMultiOutput::loadSound(std::string fileName, bool stream)
{

    fileName = ofToDataPath(fileName);

    // fmod uses IO posix internally, might have trouble
    // with unicode paths...
    // says this code:
    // http://66.102.9.104/search?q=cache:LM47mq8hytwJ:www.cleeker.com/doxygen/audioengine__fmod_8cpp-source.html+FSOUND_Sample_Load+cpp&hl=en&ct=clnk&cd=18&client=firefox-a
    // for now we use FMODs way, but we could switch if
    // there are problems:

    bMultiPlay = false;

    // [1] init fmod, if necessary

    initializeFmod();

    // [2] try to unload any previously loaded sounds
    // & prevent user-created memory leaks
    // if they call "loadSound" repeatedly, for example

    unloadSound();

    // [3] load sound

    //choose if we want streaming
    int fmodFlags =  FMOD_SOFTWARE;
    if(stream)fmodFlags =  FMOD_SOFTWARE | FMOD_CREATESTREAM;

    result = FMOD_System_CreateSound(sys, fileName.c_str(),  fmodFlags, NULL, &sound);

    if (result != FMOD_OK)
    {
        bLoadedOk = false;
        ofLogError("ofxSoundPlayerMultiOutput") << "loadSound(): could not load \"" << fileName << "\"";
    }
    else
    {
        bLoadedOk = true;
        FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_PCM);
        isStreaming = stream;
    }
    
    // set levels
    for(int i =0; i<8; i++) {
        levels[i] = 1.0;
    }
    

    return bLoadedOk;
}

void ofxSoundPlayerMultiOutput::setChannelLevels() {
    
    FMOD_Channel_SetSpeakerMix(channel, levels[0], levels[1],levels[2],levels[3],levels[4],levels[5],levels[6],levels[7]);

}

void ofxSoundPlayerMultiOutput::playTo(int speaker) {
    for(int i =0; i<8; i++) {
        levels[i] = 0;
    }
    setVolumeAt(speaker, 1.0);
    play();
    
}

void ofxSoundPlayerMultiOutput::playTo(int speaker0, int speaker1) {
    for(int i =0; i<8; i++) {
        levels[i] = 0;
    }
    setVolumeAt(speaker0, 1.0);
    setVolumeAt(speaker1, 1.0);

    play();
}

void ofxSoundPlayerMultiOutput::playTo(int* speakers, int numSpeakers) {
    
    for(int i =0; i<8; i++) {
        levels[i] = 0;
    }
    for(int i =0; i<numSpeakers; i++) {
        levels[speakers[i]] = 1.0;
    }
    play();
}

//------------------------------------------------------------
void ofxSoundPlayerMultiOutput::unloadSound()
{
    if (bLoadedOk)
    {
        stop();						// try to stop the sound
        if(!isStreaming)FMOD_Sound_Release(sound);
        bLoadedOk = false;
    }
}

//------------------------------------------------------------
bool ofxSoundPlayerMultiOutput::getIsPlaying()
{

    if (!bLoadedOk) return false;

    int playing = 0;
    FMOD_Channel_IsPlaying(channel, &playing);
    return (playing != 0 ? true : false);
}

//------------------------------------------------------------
float ofxSoundPlayerMultiOutput::getSpeed()
{
    return speed;
}

//------------------------------------------------------------
float ofxSoundPlayerMultiOutput::getPan()
{
    return pan;
}

//------------------------------------------------------------
float ofxSoundPlayerMultiOutput::getVolume()
{
    return volume;
}

//------------------------------------------------------------
bool ofxSoundPlayerMultiOutput::isLoaded()
{
    return bLoadedOk;
}

//------------------------------------------------------------
void ofxSoundPlayerMultiOutput::setVolume(float vol)
{
    if (getIsPlaying() == true)
    {
        FMOD_Channel_SetVolume(channel, vol);
    }
    volume = vol;
    
    for(int i=0; i<8; i++) {
        setVolumeAt(i, volume);
    }
}

//------------------------------------------------------------

void ofxSoundPlayerMultiOutput::setVolumeAt(int index, float volume) {
    
    levels[index] = volume;
    setChannelLevels();
    
}

//------------------------------------------------------------
void ofxSoundPlayerMultiOutput::setPosition(float pct)
{
    if (getIsPlaying() == true)
    {
        int sampleToBeAt = (int)(length * pct);
        FMOD_Channel_SetPosition(channel, sampleToBeAt, FMOD_TIMEUNIT_PCM);
    }
}

void ofxSoundPlayerMultiOutput::setPositionMS(int ms)
{
    if (getIsPlaying() == true)
    {
        FMOD_Channel_SetPosition(channel, ms, FMOD_TIMEUNIT_MS);
    }
}

//------------------------------------------------------------
float ofxSoundPlayerMultiOutput::getPosition()
{
    if (getIsPlaying() == true)
    {
        unsigned int sampleImAt;

        FMOD_Channel_GetPosition(channel, &sampleImAt, FMOD_TIMEUNIT_PCM);

        float pct = 0.0f;
        if (length > 0)
        {
            pct = sampleImAt / (float)length;
        }
        return pct;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------
int ofxSoundPlayerMultiOutput::getPositionMS()
{
    if (getIsPlaying() == true)
    {
        unsigned int sampleImAt;

        FMOD_Channel_GetPosition(channel, &sampleImAt, FMOD_TIMEUNIT_MS);

        return sampleImAt;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------
void ofxSoundPlayerMultiOutput::setPan(float p)
{
    pan = p;
    p = ofClamp(p, -1, 1);
    if (getIsPlaying() == true)
    {
        FMOD_Channel_SetPan(channel,p);
    }
}


//------------------------------------------------------------
void ofxSoundPlayerMultiOutput::setPaused(bool bP)
{
    if (getIsPlaying() == true)
    {
        FMOD_Channel_SetPaused(channel,bP);
        bPaused = bP;
    }
}


//------------------------------------------------------------
void ofxSoundPlayerMultiOutput::setSpeed(float spd)
{
    if (getIsPlaying() == true)
    {
        FMOD_Channel_SetFrequency(channel, internalFreq * spd);
    }
    speed = spd;
}


//------------------------------------------------------------
void ofxSoundPlayerMultiOutput::setLoop(bool bLp)
{
    if (getIsPlaying() == true)
    {
        FMOD_Channel_SetMode(channel,  (bLp == true) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
    }
    bLoop = bLp;
}

// ----------------------------------------------------------------------------
void ofxSoundPlayerMultiOutput::setMultiPlay(bool bMp)
{
    bMultiPlay = bMp;		// be careful with this...
}

// ----------------------------------------------------------------------------
void ofxSoundPlayerMultiOutput::play()
{

    // if it's a looping sound, we should try to kill it, no?
    // or else people will have orphan channels that are looping
    if (bLoop == true)
    {
        FMOD_Channel_Stop(channel);
    }

    // if the sound is not set to multiplay, then stop the current,
    // before we start another
    if (!bMultiPlay)
    {
        FMOD_Channel_Stop(channel);
    }

    FMOD_System_PlaySound(sys, FMOD_CHANNEL_FREE, sound, bPaused, &channel);

    FMOD_Channel_GetFrequency(channel, &internalFreq);
    FMOD_Channel_SetVolume(channel,volume);
    setPan(pan);
    FMOD_Channel_SetFrequency(channel, internalFreq * speed);
    FMOD_Channel_SetMode(channel,  (bLoop == true) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    //fmod update() should be called every frame - according to the docs.
    //we have been using fmod without calling it at all which resulted in channels not being able
    //to be reused.  we should have some sort of global update function but putting it here
    //solves the channel bug
    FMOD_System_Update(sys);

}





// ----------------------------------------------------------------------------
void ofxSoundPlayerMultiOutput::stop()
{
    FMOD_Channel_Stop(channel);
}

