#pragma once

#include <memory>

#include "ofMain.h"
#include "time.h"
#include "ConfigurationManager.h"
#include "KinectPointCloud.h"
#include "Thread.h"

class ofApp : public ofBaseApp{

	public:

		enum MachineState
		{
			WAITING_USER = 0,
			WAITING_INTERACTION = 1,
			PREPARE_TO_RECORDE = 2,
			RECORDING_IMAGES = 3,
			SAVING_FILES = 4
		};

		enum COUNTDOWN
		{
			START_COUNTDOWN = 0,
			VERIFY_TIME = 1,
			FINISH_COUNTDOWN = 2
		};

		ofApp(ConfigurationManager& c);

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
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		int time;
		clock_t t1, t2;
		MyThread myThread;
		ofImage getready, getset, welcome, go, hand0, hand1, hand2, hand3, hand4, hand5, holdhere, progress_bar, recording, saving;
		
	private:
		ConfigurationManager& config; // < configuration data

		MachineState userState;
		COUNTDOWN timer;
		// Kinect
		NUI::KinectPointCloud pointCloud;

};
