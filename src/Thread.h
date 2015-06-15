#pragma once

#include"ofThread.h"
#include"ofImage.h"

class MyThread : public ofThread {

public:
	MyThread(ofImage** imgArray, int length)
	{
		ImageArraySize = length;
		imageArray     = imgArray;
		progress       = 1.0;
		frameNumber	   = 0;
	}

	float getProgress() const
	{
		return progress;
	}

	float getAtualFrame() const
	{
		return frameNumber;
	}

private:
	void MyThread::threadedFunction(){

		for ( int numFiles = 0; numFiles < ImageArraySize; ++numFiles)
		{
			progress = numFiles/float(ImageArraySize);
			frameNumber = numFiles;
			(*imageArray)[numFiles].saveImage("output" + to_string(numFiles) +".png",ofImageQualityType::OF_IMAGE_QUALITY_MEDIUM);
		}

		progress= 1.0;
	}

	int frameNumber;
	int ImageArraySize;
	float progress;
	ofImage** imageArray;

};