#pragma once

#include "ofMain.h"
#include "ofxSoundPlayerMultiOutput.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
    
        void setup();
		void update();
		void draw();
		
		ofxSoundPlayerMultiOutput bassSound;
        ofxSoundPlayerMultiOutput beatSound;
    
        ofxPanel         gui;
        ofParameterGroup mainParams;
        ofParameterGroup bassParams;
        ofParameterGroup beatParams;
    
        vector<ofParameter<float>> bassLevels;
        vector<ofParameter<float>> beatLevels;
	
};
