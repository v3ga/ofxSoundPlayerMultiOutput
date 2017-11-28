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
    
    float levels[8];
    for(int i =0; i<8; i++) {
        levels[i] = bassLevels[i];
    }

    
    bassSound.setChannelLevels(levels);
    
    for(int i =0; i<8; i++) {
        levels[i] = beatLevels[i];
    }
    
    
    beatSound.setChannelLevels(levels);
    
	ofFmodSoundUpdate();
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
