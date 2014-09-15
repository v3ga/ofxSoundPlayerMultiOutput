ofxSoundPlayerMultiOutput
=========================
Addon for Openframeworks to play sounds on selected speakers on a given soundcard. It uses an implementation of `ofSoundPlayer` with Fmod.

You can select an output with the function 

    void ofFmodSelectDriver(int driver);

Then you can use the method `playTo` to play the loaded sound on different speakers:

	soundPlayer.load("sound.mp3");
	...
	soundPlayer.playTo(1); 			// Plays sound to speaker 1
	soundPlayer.playTo(3,4); 		// Plays sound to speakers 3,4
	int speakers[4] = {0,1,4,5};	
	soundPlayer.playTo(speakers,4); // Plays sound to speakers 0,1,4,5


## References
https://raw.githubusercontent.com/vcuculo/ofxMultiSpeakerSoundPlayer

## Platform
Developped with Openframeworks 0.8+ on MacOS 10.9.4 with a Motu 828mk2 device connected through Firewire.
