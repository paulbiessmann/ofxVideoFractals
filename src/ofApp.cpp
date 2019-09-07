#include "ofApp.h"

float fps = 25;

float scene0    = 0;    //frames, Sänger in Jumpsuit
float scene1    = 25;//25   // zoomen beginnt
float scene2    = 975;  // Zoomen ende, aber mit Fleisch
float scene3    = 1275; // Schnittfleisch

float vidPart   = 500;  //frames in one vid till pause


//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(255);
	ofSetVerticalSync(true);
	frameByframe = false;
    
    ofEnableAlphaBlending();
    
    ofDisableArbTex();

	// Uncomment this to show movies with alpha channels
	// fingerMovie.setPixelFormat(OF_PIXELS_RGBA);

	targetMovie.load("movies/Winkhler-Profilbild-Nah.mp4");
	targetMovie.setLoopState(OF_LOOP_NORMAL);
	targetMovie.play();
    targetMovie.setPaused(true);
    
    fractalMovie.setPixelFormat(OF_PIXELS_RGBA);
    fractalMovie.load("movies/singlePers_cut.mov");
    fractalMovie.setLoopState(OF_LOOP_NORMAL);
    fractalMovie.play();
    fractalMovie.setPaused(true);
    
    maxStep = fullHeight; //520
    minStep = 9; //30
    stepSize = maxStep + 1;
    stepFloat = stepSize;
    fractWidth = maxStep;
   
    // let's move through the "RGB(A)" char array
    // using the red pixel to control the size of a circle.
    vidWidth = targetMovie.getWidth();
    vidHeight = targetMovie.getHeight();
   
    
    screenGrab.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    
    center.x = fullWidth/2;
    center.y = fullHeight/2;
    
    vidRatio = (float) vidWidth / (float) vidHeight;
    
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
    fileName = "CamFractal_v3";
    fileExt = ".mov";
    vidRecorder.setVideoCodec("prores");
    vidRecorder.setVideoBitrate("2000k");
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    recordFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    recordFboFlip.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    bRecording = false;
    
    keyReleased('r');
    bEnd = false;
    bPause = false;
    
    useVid = false; //should be renamed

}

//--------------------------------------------------------------
void ofApp::update(){
        //fractalMovie.setSpeed(vidRecorder.sp)
    
    if(!bPause){
        recordedFrame = vidRecorder.getNumVideoFramesRecorded();// + vidRecorder.getVideoQueueSize();
        fractalMovie.nextFrame();
        fractalMovie.update();
        
        targetMovie.nextFrame();
        targetMovie.update();
        
        if(recordedFrame > scene1 && recordedFrame < scene2 && (int)recordedFrame % 1 == 0){
            stepFloat *=   0.995;
            
            //stepFloat -= 5;
            
            if(stepFloat < minStep){ stepFloat=minStep;}
            
        }
        
        
        if(recordedFrame > scene1 + 100 && (int)recordedFrame % 50 == 0){
            
            repetitions++;
            
            
        }
        if(recordedFrame > scene1 && recordedFrame < scene2 && (int)recordedFrame % 5 == 0  && positions.size() < 600){

            if(recordedFrame > scene2/2){   repetitions = 10;  }
            if(repetitions > 10){  repetitions = 10;  }

            for(int i=0; i<repetitions; i++){

                int posX = ofRandom(-50, fullWidth );
                int posY = ofRandom(-50, fullHeight);

//                posX = center.x - (posX - center.x);
//                posY = center.y - (posY - center.y);

                positions.push_back(ofVec2f(posX, posY));
                int randWidth = ofRandom(300, 800);
                int randHeight = randWidth / vidRatio;
                sizes.push_back(ofVec2f(randWidth, randHeight));

                if(positions.size() > 5000){
                    positions.erase(positions.begin());
                    sizes.erase(sizes.begin());
                }
            }
            cout << "size: " << positions.size() << "\n";

        }


//        if(recordedFrame > scene1 && recordedFrame < scene2 ){
//            for(int i=0; i<positions.size(); i++){
//                float dirX =  (center.x + positions[i].x )  * 0.001;
//                float dirY =  (center.y + positions[i].y )  * 0.001; //0.003
//
//                positions[i].x += dirX;
//                positions[i].y += dirY;
//
//
//            }
//        }

        
        if(recordedFrame > scene2){
            
        }
        stepSize = (int)stepFloat;

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
    
    bool smallFract = false;

    relativeTimef = ofGetElapsedTimef();
    
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());


    recordFbo.begin();
    ofClear(255,255,255,255);
    ofSetBackgroundColor(255,255,255,255);



    
    if(!bPause){


        // get target movie as pixel
        pixels = targetMovie.getPixels();
        
        //ofLog(OF_LOG_NOTICE, "stepsize %d", stepSize);

        
        fractWidth = stepSize * vidRatio;
        //stepSize = fractWidth / vidRatio;
        
        
        if(1){    //      try something else
            screenGrab.begin();
            ofClear(255);
            float picFact = 1;//maxStep/stepFloat  ;
            float bereich = (50 * (int)maxStep/stepSize);
            
            for (int pi = positions.size()-1; pi >= 0; pi--)
            {
                if(pi<positions.size()-1){
                    ofSetColor(0, 200*picFact-pi);
                    ofSetLineWidth(6);
                    ofDrawLine(positions[pi].x+(picFact*sizes[pi].x/2), positions[pi].y+(picFact*sizes[pi].y/2), positions[pi+1].x+(picFact*sizes[pi+1].x/2), positions[pi+1].y+(picFact*sizes[pi+1].y/2));
                }
            }
            for (int pi = positions.size()-1; pi >= 0; pi--)
            {
                
                ofColor fractColor = pixels.getColor(positions[pi].x, positions[pi].y);
                fractColor += 100; //100
                
                ofSetColor(fractColor);
                
                int tmpWidth = sizes[pi].x * picFact;
                int tmpHeight = sizes[pi].y * picFact;
                
                if(tmpWidth < 100){ tmpWidth = 100; }
                if(tmpHeight < 56){ tmpWidth = 56;  }
                
                fractalMovie.draw( positions[pi].x , positions[pi].y , tmpWidth , tmpHeight );
                
            }
            screenGrab.end();
           // screenGrab.draw( 0,0);
        
            if ( stepSize < 8 ){
                ofSetColor(255);
                targetMovie.draw(0,0);
            }
            else{
                if(recordedFrame <= scene1){
                    for (int pi=0; pi< positions.size(); pi++){
                        ofSetColor(255);
                        screenGrab.draw(0,0);
                    }
                }
                for (int i = 4; i < vidWidth ; i+=fractWidth){
                    for (int j = 4; j < vidHeight; j+=stepSize){


                        ofColor fractColor ;
                        fractColor = pixels.getColor(i, j);
                        fractColor += 50;

                        ofSetColor(fractColor);

                        if (recordedFrame > scene1)
                        {
                            float picFact = stepSize/maxStep;
                            float bereich = (50 * (int)maxStep/stepSize);
                            screenGrab.draw( i,  j, fractWidth*1, stepSize*1);
                        }
                    }
                }
            }
        }
        
        if ( stepSize < 8 ){
            ofSetColor(255);
            targetMovie.draw(0,0);
        }
        else if(0){
                    
            if (recordedFrame > scene0)
                {
                    float picFact = stepSize/maxStep ;
                    float bereich = (50 * (int)maxStep/stepSize);
                    // fractalMovie.draw( i,  j, fractWidth, stepSize);
                    
                    for (int pi = positions.size()-1; pi >= 0; pi--)
                    {
                        if(pi<positions.size()-1){
                            ofSetColor(0, 200*picFact-pi);
                            ofSetLineWidth(6);
                            ofDrawLine(positions[pi].x+(picFact*sizes[pi].x/2), positions[pi].y+(picFact*sizes[pi].y/2), positions[pi+1].x+(picFact*sizes[pi+1].x/2), positions[pi+1].y+(picFact*sizes[pi+1].y/2));
                        }
                    }
                    for (int pi = positions.size()-1; pi >= 0; pi--)
                    {
           
                        ofColor fractColor = pixels.getColor(positions[pi].x, positions[pi].y);
                        fractColor += 80; //100
                        
                        ofSetColor(fractColor);
                        
                        int tmpWidth = sizes[pi].x * picFact;
                        int tmpHeight = sizes[pi].y * picFact;
                        
                        if(tmpWidth < 14){tmpWidth = 14;}
                        if(tmpHeight < 8){tmpWidth = 8;}
                        
                        fractalMovie.draw( positions[pi].x , positions[pi].y , sizes[pi].x * picFact + 10, sizes[pi].y * picFact + 10 );
                        //fractalMovie.draw( positions[pi].x, positions[pi].y, sizes[pi].x  , sizes[pi].y  );

                    }
                }
        
        }
    }
    
    recordFbo.end();
    ofSetColor(255,255,255);
    recordFbo.draw(0,0, fullWidth/4, fullHeight/4);
    
    /* recording pause */
    if((int)recordedFrame % (int)vidPart == 0 && !bPause && recordedFrame > 1){
        bPause = true;
        keyReleased('r');
    }

    
    /* recording fortsetzen */
    if(vidRecorder.getVideoQueueSize() < 5 && bPause && recordedFrame < scene3){
        bPause = false;
        keyReleased('r');
    }
    
    /** END Recording **/
    if(recordedFrame - vidRecorder.getVideoQueueSize() > scene3 && !bEnd  && !bPause){
        keyReleased('r');
        bEnd = true;
        bPause = true;
    }
    /* Close Recordign */
    if(vidRecorder.getVideoQueueSize() < 1 && recordedFrame > scene3 && bEnd){
        exit();
    }


//    ofSetHexColor(0x000000);
//    ofDrawBitmapString("press f to change",20,320);
//    if(frameByframe) ofSetHexColor(0xCCCCCC);
//    ofDrawBitmapString("mouse speed position",20,340);
//    if(!frameByframe) ofSetHexColor(0xCCCCCC); else ofSetHexColor(0x000000);
//    ofDrawBitmapString("keys <- -> frame by frame " ,190,340);
//    ofSetHexColor(0x000000);
//
//    ofDrawBitmapString("frame: " + ofToString(fractalMovie.getCurrentFrame()) + "/"+ofToString(fractalMovie.getTotalNumFrames()),20,380);
//    ofDrawBitmapString("duration: " + ofToString(fractalMovie.getPosition()*fractalMovie.getDuration(),2) + "/"+ofToString(fractalMovie.getDuration(),2),20,400);
//    ofDrawBitmapString("speed: " + ofToString(fractalMovie.getSpeed(),2),20,420);

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
        
       // stepSize = abs(speed * 100) + 2;
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
