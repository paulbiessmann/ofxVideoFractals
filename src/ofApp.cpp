#include "ofApp.h"

float fps = 25;

float scene0 = 0; //frames, Sänger in Jumpsuit
float scene1 = 975; // Oszillation beginnt
float scene2 = 1650; // zoomen beginnt
float scene3 = 2925; // Zoomen ende, aber mit Fleisch
float scene4 = 3450; // Schnittfleisch

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(5);
	ofSetVerticalSync(true);
	frameByframe = false;
    
    ofDisableArbTex();

	// Uncomment this to show movies with alpha channels
	// fingerMovie.setPixelFormat(OF_PIXELS_RGBA);

	fractalMovie.load("movies/Winkhler-Profilbild-Nah.mp4");
	fractalMovie.setLoopState(OF_LOOP_NORMAL);
	fractalMovie.play();
    
    stepSize = 200;
    
    
    positions.resize(8);
    sizes.resize(8);
    
    positions[0] = ofVec2f(1369, 100 );
    positions[1] = ofVec2f(1279, 576 );
    positions[2] = ofVec2f(1113, 997 );
    positions[3] = ofVec2f(1223, 1268);
    positions[4] = ofVec2f(-304, 1096);
    positions[5] = ofVec2f(2039, 1105);
    positions[6] = ofVec2f(2687, 527);
    positions[7] = ofVec2f(2893, 1229);
    
    // sizes: y right - y left
    float ratio = fractalMovie.getHeight() / fractalMovie.getWidth() ;
    sizes[0] = ofVec2f(512, 512*ratio);
    sizes[1] = ofVec2f(192, 192*ratio);
    sizes[2] = ofVec2f(210, 210*ratio);
    sizes[3] = ofVec2f(267, 267*ratio);
    sizes[4] = ofVec2f(545, 545*ratio);
    sizes[5] = ofVec2f(208, 208*ratio);
    sizes[6] = ofVec2f(464, 464*ratio);
    sizes[7] = ofVec2f(526, 526*ratio);
    
    /************ Video Record *********/
    recordedFrame = 0;
    ofSetFrameRate((int) fps);
    ofSetLogLevel(OF_LOG_VERBOSE);
    fileName = "CamFractal";
    fileExt = ".mov";
    vidRecorder.setVideoCodec("prores");
    vidRecorder.setVideoBitrate("2000k");
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    recordFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    recordFboFlip.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    bRecording = false;
    
    //keyReleased('r');
    bEnd = false;
    bPause = false;
    
    useVid = false;

}

//--------------------------------------------------------------
void ofApp::update(){
    fractalMovie.update();
    
    if(!bPause){
        recordedFrame = vidRecorder.getNumVideoFramesRecorded();// + vidRecorder.getVideoQueueSize();
    }
    
    
    recordFbo.getTexture().readToPixels(recordPixels);
    if(bRecording){
        bool success = vidRecorder.addFrame(recordPixels);
        
        if (!success) {
            ofLogWarning("This frame was not added!");
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    relativeTimef = ofGetElapsedTimef();
    
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());


    recordFbo.begin();
    ofClear(0,0,0,255);
    
	//ofSetHexColor(0xFFFFFF);

    
    

   // ofSetHexColor(0x000000);
    // get target movie as pixel
    ofPixels & pixels = fractalMovie.getPixels();

    
    if(!bPause){

    
        // let's move through the "RGB(A)" char array
        // using the red pixel to control the size of a circle.
        int vidWidth = pixels.getWidth() ;
        int vidHeight = pixels.getHeight();
        int nChannels = pixels.getNumChannels();
        
        float vidRatio = (float) vidWidth / (float) vidHeight;
        int fractWidth = stepSize * vidRatio;
        
        if(useVid){
            
            for (int i = 4; i < vidWidth; i+=stepSize){
                for (int j = 4; j < vidHeight; j+=stepSize){
                    unsigned char r = pixels[(j * vidWidth + i)*nChannels];
                    float val = 1 - ((float)r / 255.0f);
                    ofSetColor(pixels.getColor(i, j));
                    //ofDrawRectangle(i, j, fractWidth*val, stepSize*val);
                    //ofSetColor(i, j, 100);
                    ofDrawBox(i,j, 0, fractWidth*val, stepSize*val, -val* 300 );
                }
            }
        }
        else{    //      try something else
            
            if ( stepSize < 8 ){
                ofSetColor(200);
                fractalMovie.draw(0,0);
            }
            else{
                for (int i = 4; i < vidWidth ; i+=fractWidth){
                    for (int j = 4; j < vidHeight; j+=stepSize){

                        ofLog(OF_LOG_NOTICE, "stepsize %d", stepSize);
                        
                        ofColor fractColor ;
                        fractColor = pixels.getColor(i, j);

                        ofSetColor(fractColor);
                        fractalMovie.draw( i,  j, fractWidth, stepSize);
                        
                        
                        for (int pi=0; pi< positions.size(); pi++){
                            if ( i > positions[pi].x && i < positions[pi].x + sizes[pi].x && j > positions[pi].y  && j < positions[pi].y + sizes[pi].y)
                            fractalMovie.draw( i, j, fractWidth , stepSize);
                        }
                        
                    }
                }
//
//                for (int pi=0; pi< positions.size(); pi++){
//                    //float stepSizeRatio =   stepSize / fractWidth;
//
//                    //ofSetColor(fractColor);
//                    fractalMovie.draw( positions[pi].x, positions[pi].y, sizes[pi].x , sizes[pi].y);
//                }
            }
        }

    }
    
    recordFbo.end();
    ofSetColor(255,255,255);
    recordFbo.draw(0,0, fullWidth/4, fullHeight/4);

    
    if(recordedFrame - vidRecorder.getVideoQueueSize() > scene4 + 50 && vidRecorder.getVideoQueueSize() < 5 ){
        keyReleased('r');
    }




    ofSetHexColor(0x000000);
	ofDrawBitmapString("press f to change",20,320);
    if(frameByframe) ofSetHexColor(0xCCCCCC);
    ofDrawBitmapString("mouse speed position",20,340);
    if(!frameByframe) ofSetHexColor(0xCCCCCC); else ofSetHexColor(0x000000);
    ofDrawBitmapString("keys <- -> frame by frame " ,190,340);
    ofSetHexColor(0x000000);

    ofDrawBitmapString("frame: " + ofToString(fractalMovie.getCurrentFrame()) + "/"+ofToString(fractalMovie.getTotalNumFrames()),20,380);
    ofDrawBitmapString("duration: " + ofToString(fractalMovie.getPosition()*fractalMovie.getDuration(),2) + "/"+ofToString(fractalMovie.getDuration(),2),20,400);
    ofDrawBitmapString("speed: " + ofToString(fractalMovie.getSpeed(),2),20,420);

    if(fractalMovie.getIsMovieDone()){
        ofSetHexColor(0xFF0000);
        ofDrawBitmapString("end of movie",20,440);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    switch(key){
        case 'f':
            frameByframe=!frameByframe;
            fractalMovie.setPaused(frameByframe);
        break;
        case OF_KEY_LEFT:
            fractalMovie.previousFrame();
        break;
        case OF_KEY_RIGHT:
            fractalMovie.nextFrame();
        break;
        case '0':
            fractalMovie.firstFrame();
        break;
        case '1':
            useVid = !useVid;
        break;
            
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='r'){
        bRecording = !bRecording;
        if(bRecording && !vidRecorder.isInitialized()) {
            //insert your path in the following row and your settings 1920x1080 60fps - the last 2 parms are zero because in my case I don't want to record audio..
            ofFilePath f;
            vidRecorder.setup( fileName+"_"+ofGetTimestampString()+".mov", recordFbo.getWidth(),recordFbo.getHeight(), (int) fps, 0, 0);
            
            //Start recording
            vidRecorder.start();
        }
        else if(!bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(true);
        }
        else if(bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(false);
        }
    }
    if(key=='c'){
        bRecording = false;
        vidRecorder.close();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	if(!frameByframe){
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        float speed = (2 * pct - 1) * 5.0f;
       // fractalMovie.setSpeed(speed);
        
        stepSize = abs(speed * 100) + 2;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if(!frameByframe){
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        fractalMovie.setPosition(pct);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if(!frameByframe){
        fractalMovie.setPaused(true);
	}
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if(!frameByframe){
        fractalMovie.setPaused(false);
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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

//--------------------------------------------------------------
void ofApp::exit(){
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();
}

//--------------------------------------------------------------
void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recoded video file is now complete." << endl;
}
