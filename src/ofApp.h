#pragma once

#include "ofMain.h"
#include "ofxVideoRecorder.h"


class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
        void exit();

		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

		ofVideoPlayer 		fractalMovie;
		bool                frameByframe;
    
    ofFbo   videoFbo;
    
    ofVideoPlayer   targetMovie;
    
    int stepSize = 1;
    int fractWidth;
    float stepFloat;
    float maxStep;
    float minStep;
    bool useVid = false;
    
    float vidRatio;
    int vidWidth;
    int vidHeight;
    
    vector <ofVec2f> positions;
    vector <ofVec2f> sizes;
    
    ofPixels  pixels;
    
    /* Recording */
    ofxVideoRecorder    vidRecorder;
    bool                bRecording;
    ofFbo               recordFbo;
    ofFbo               recordFboFlip;
    ofPixels            recordPixels;
    string              fileName;
    string              fileExt;
    float               recordedFrame;
    float               waitCounter     = 0;
    bool                bPause = false;
    bool                bEnd;
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
    int fullWidth   = 3840;
    int fullHeight  = 2160;
    
    float   relativeTimef;
};

