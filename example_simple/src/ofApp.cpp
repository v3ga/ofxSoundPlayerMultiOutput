#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //---------------------------------- gui
    
    mainParams.setName("settings");
    
    bassParams.setName("Bass");
    for(int i =0; i<8; i++) {
        ofParameter<float> bassLevel;
        bassParams.add(bassLevel.set("Speaker "+ofToString(i),0.0, 0.0, 1.0));
        bassLevels.push_back(bassLevel);
    }
    
    beatParams.setName("Beats");
    for(int i =0; i<8; i++) {
        ofParameter<float> beatLevel;
        beatParams.add(beatLevel.set("Speaker "+ofToString(i),0.0, 0.0, 1.0));
        beatLevels.push_back(beatLevel);;
    }
    
    mainParams.add(bassParams);
    mainParams.add(beatParams);
    gui.setup(mainParams);

    //---------------------------------- fmod
    
    // Select driver (see log)
    ofFmodSelectDriver(0);
	
	   // Load sound
    bassSound.loadSound("bass.wav", true);
    bassSound.setLoop(true);
    
    beatSound.loadSound("beat.wav", true);
    beatSound.setLoop(true);
    
    // Play it together
    bassSound.play();
    beatSound.play();
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    for(int i =0; i<8; i++) {
        bassSound.setVolumeAt(i, bassLevels[i]);
    }

    for(int i =0; i<8; i++) {
        beatSound.setVolumeAt(i, bassLevels[i]);
    }

	ofFmodSoundUpdate();
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    gui.draw();
}

