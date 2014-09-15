#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	   // Selecte driver (see log)
	   ofFmodSelectDriver(0);
	
	   // Load sound
	   soundPlayer.loadSound("1085.mp3", true);

	   // Play it to speaker 0 and speaker 1
	   // soundPlayer.playTo(0,1);

	   // Play it to a list of speakers
	   //int speakers[3] = {0,1,2};
	   //soundPlayer.playTo(speakers,3);

	   // Play it to speaker 1
	   soundPlayer.playTo(1);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofFmodSoundUpdate();
}

//--------------------------------------------------------------
void ofApp::draw(){

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
