#include "ofApp.h"

//--------------------------------------------------------------

ofApp::ofApp(ConfigurationManager& c) : config(c), myThread(&pointCloud.imageArray, NUI::ImageArraySize)
{

}


//--------------------------------------------------------------
void ofApp::setup()
{
	//welcome.allocate(1824,541,ofImageType::OF_IMAGE_COLOR);
	welcome.loadImage("welcome.png");
	getready.loadImage("getready.png");
	getset.loadImage("getset.png");
	go.loadImage("go.png");
	hand0.loadImage("hand0.png");
	hand1.loadImage("hand1.png");
	hand2.loadImage("hand2.png");
	hand3.loadImage("hand3.png");
	hand4.loadImage("hand4.png");
	hand5.loadImage("hand5.png");
	holdhere.loadImage("holdtostart.png");
	progress_bar.loadImage("progress_red.png");
	recording.loadImage("recording.png");
	saving.loadImage("savingvideo.png");
	time = 0;
	// UI
	ofBackground(ofColor::black);
	userState = WAITING_USER;
	timer = START_COUNTDOWN;
	
	// Kinect
	pointCloud.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
	pointCloud.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{

	if (!pointCloud.connected())
	{
		ofSetColor(0,0,0);
		ofDrawBitmapString("Error: No kinect device detected!",10, 10);
	} else {
		switch(userState)
		{
			case WAITING_USER:
				pointCloud.positionArray = 0;
				pointCloud.draw_image();
				welcome.draw(486,10);
				if(pointCloud.hasUserInFrame() ) userState = WAITING_INTERACTION;
				break;

			case WAITING_INTERACTION:
				pointCloud.draw_image();
				holdhere.draw(173.5, 40);
				hand5.draw(1165,220);
				if( !pointCloud.hasUserInFrame() )
				{
					userState = WAITING_USER;
				} else {
					if( pointCloud.isUserPositioned() )
					{
						pointCloud.draw_image();
						switch(timer)
						{
							case START_COUNTDOWN:
								t1 = clock();
								hand5.draw(1165,220);
								timer = VERIFY_TIME;
								break;
							case VERIFY_TIME:
								t2 = clock();
								time = (int) (((float)t2)/CLOCKS_PER_SEC) - (((float)t1)/CLOCKS_PER_SEC);
								switch(time)
								{
								case 0:
									hand5.draw(1165,220);
									break;
								case 1:
									hand4.draw(1165,220);
									break;
								case 2:
									hand3.draw(1165,220);
									break;
								case 3:
									hand2.draw(1165,220);
									break;
								case 4:
									hand1.draw(1165,220);
									break;
								case 5:
									hand0.draw(1165,220);
									timer = START_COUNTDOWN;
									userState = PREPARE_TO_RECORDE;
									break;
								case 6:
									getready.draw(630,477);
									break;
								case 7:
									getset.draw(726,478);
									break;
								case 8:
									go.draw(836,474);
									break;
								case 9:
									break;
									pointCloud.ready2Save= true;

								}
								break;
						}
					} else timer = START_COUNTDOWN; 
				}
				break;

			case PREPARE_TO_RECORDE:
				pointCloud.draw_image();
				t2 = clock();
				time = (int) (((float)t2)/CLOCKS_PER_SEC) - (((float)t1)/CLOCKS_PER_SEC);
				switch (time)
				{
				case 5:
					getready.draw(630,477);
					break;
				case 6:
					getset.draw(726,478);
					break;
				case 7:
					go.draw(836,474);
					break;
				case 8:
					pointCloud.ready2Save= true;
					userState = RECORDING_IMAGES;
					break;
				}
			break;

			case RECORDING_IMAGES:
				pointCloud.draw_image();
				recording.draw(80,80);
				if(!pointCloud.ready2Save) { 
					myThread.startThread();
					userState = SAVING_FILES;
				}
			break;

			case SAVING_FILES:
				pointCloud.drawFromArray();
				saving.draw(80,80);
				float size = (myThread.getAtualFrame()/ pointCloud.getArraySize()) * 1686;
				progress_bar.drawSubsection(80,80, 1686, 146, size, 0);
				if( myThread.getProgress() >= 1.0f )
				{
					userState = WAITING_USER;
				}
				break;
		}
	}
}



//--------------------------------------------------------------
void ofApp::exit()
{

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 

}
