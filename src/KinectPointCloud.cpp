#include "KinectPointCloud.h"
#include "ScopedRelease.h"
#include "HResultHandler.h"
#include <ofLog.h>

using namespace NUI;

const string MODULE="kinect";

struct logger
{
	void operator()(const char *filename, int line, const char *log ) const {
		ofLogError(MODULE, "%s:%d - %s",filename, line, log);
	}
};



KinectPointCloud::KinectPointCloud(void)
{
	kinectFailed = false;
	kinectSensor = NULL;
	coordinateMapper = NULL;
	depthCoordinates = NULL;
	m_pBodyFrameReader = NULL;
	multiSourceFrameReader = NULL;
	positionArray = 0;
	imageArrayIdx = 0;

	depthCoordinates = new DepthSpacePoint[ColorWidth * ColorHeight];
	tmpColorBuffer   = new RGBQUAD[ColorWidth * ColorHeight];
	texture.allocate(ColorWidth, ColorHeight, ofImageType::OF_IMAGE_COLOR_ALPHA);
	imageArray = new ofImage[ImageArraySize];
	for (int i=0; i < ImageArraySize; ++i)
	{
		imageArray[i].allocate(ColorWidth, ColorHeight, ofImageType::OF_IMAGE_COLOR_ALPHA);
		cout << i << endl;
	}

}


KinectPointCloud::~KinectPointCloud(void)
{

	if (imageArray != NULL)
		delete[] imageArray;

	if (kinectSensor != NULL)
	{
		kinectSensor->Close();
	}

	SafeRelease(coordinateMapper);
	SafeRelease(multiSourceFrameReader);

	if (depthCoordinates != NULL)
	{
		delete[] depthCoordinates;
	}

	if (tmpColorBuffer != NULL)
	{
		delete[] tmpColorBuffer;
	}

	SafeRelease(m_pBodyFrameReader);

	SafeRelease(kinectSensor);
}


void KinectPointCloud::setup()
{
	ready2Save = false;	

	if (kinectSensor != NULL)
	{
		ofLogWarning(MODULE,"Trying to setup a kinect device that was already set");
		return;
	}

	bool isOpen = false;

	HRESULT result;
	result = GetDefaultKinectSensor(&kinectSensor);
	kinectFailed = true;
	
	if (FAILED(result))
	{
		ofLogError(MODULE,"Couldn't get DefaultKinectSensor");
		kinectSensor = NULL;
		return;
	}


	if (kinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the frame reader
		if (SUCCEEDED(result))
			result = kinectSensor->get_CoordinateMapper(&coordinateMapper);
			
		result = kinectSensor->Open();

		if (SUCCEEDED(result))
		{
			isOpen = true;
			result = kinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex,
				&multiSourceFrameReader);
		} else {
			ofLogError(MODULE,"Couldn't open KinectSensor");
		}
		kinectFailed = false;
	} else {
		ofLogError(MODULE,"Couldn't initialize DefaultKinectSensor");
	}

	if (FAILED(result))
	{
		ofLogError(MODULE,"Couldn't open MultiSourceFrameReader");

		if (isOpen)
			kinectSensor->Close();

		multiSourceFrameReader = NULL;
		coordinateMapper = NULL;
		kinectSensor = NULL;
	}

}

void KinectPointCloud::update()
{
#define IfFailThenLogErrorAndReturn( op ) result = op; if (FAILED(result)) { logHRESULT(logger(),__FILE__,__LINE__,(HRESULT) result); kinectFailed =  true;	return; }

#define TestResultAndExecute( op ) if (SUCCEEDED(result)) { result = op; }

	if (!multiSourceFrameReader || kinectFailed)
    {
        return;
    }

    ScopedRelease<IMultiSourceFrame> multiSourceFrame;
    ScopedRelease<IDepthFrame>       depthFrame;
    ScopedRelease<IColorFrame>       colorFrame;
    ScopedRelease<IBodyIndexFrame>   bodyIndexFrame;

	HRESULT result = multiSourceFrameReader->AcquireLatestFrame(&multiSourceFrame.get());

	// depth stream
    ScopedRelease<IDepthFrameReference> depthFrameReference;
    TestResultAndExecute(multiSourceFrame->get_DepthFrameReference(&depthFrameReference.get()));
	TestResultAndExecute(depthFrameReference->AcquireFrame(&depthFrame.get()));

	// color stream
    ScopedRelease<IColorFrameReference> colorFrameReference;
    TestResultAndExecute(multiSourceFrame->get_ColorFrameReference(&colorFrameReference.get()));
    TestResultAndExecute(colorFrameReference->AcquireFrame(&colorFrame.get()));


	// data to complement depth stream
	// it allows us to tell whether a pixel belongs to a body
    ScopedRelease<IBodyIndexFrameReference> bodyIndexFrameReference;
    TestResultAndExecute(multiSourceFrame->get_BodyIndexFrameReference(&bodyIndexFrameReference.get()))
    TestResultAndExecute(bodyIndexFrameReference->AcquireFrame(&bodyIndexFrame.get()));

	if (FAILED(result))
		return;

	// if acquiring one depth frame, one color frame and one bodyIndex frame
	// went well, then we can handle their data
    ScopedRelease<IFrameDescription> depthFrameDescription;
    int depthWidth = 0;
    int depthHeight = 0;
    UINT depthBufferSize = 0;
    UINT16 *depthBuffer = NULL;

    ScopedRelease<IFrameDescription> colorFrameDescription;
    int colorWidth = 0;
    int colorHeight = 0;
    ColorImageFormat imageFormat = ColorImageFormat_None;
    UINT colorBufferSize = 0;
    RGBQUAD *colorBuffer = NULL;

    ScopedRelease<IFrameDescription> bodyIndexFrameDescription;
    int bodyIndexWidth = 0;
    int bodyIndexHeight = 0;
    UINT bodyIndexBufferSize = 0;
    BYTE *bodyIndexBuffer = NULL;
	
    // get depth frame data
    IfFailThenLogErrorAndReturn(depthFrame->get_FrameDescription(&depthFrameDescription.get()));
    IfFailThenLogErrorAndReturn(depthFrameDescription->get_Width(&depthWidth));
    IfFailThenLogErrorAndReturn(depthFrameDescription->get_Height(&depthHeight));
    IfFailThenLogErrorAndReturn(depthFrame->AccessUnderlyingBuffer(&depthBufferSize, &depthBuffer));            
 

    // get color frame data
    IfFailThenLogErrorAndReturn(colorFrame->get_FrameDescription(&colorFrameDescription.get()));
    IfFailThenLogErrorAndReturn(colorFrameDescription->get_Width(&colorWidth));
    IfFailThenLogErrorAndReturn(colorFrameDescription->get_Height(&colorHeight));
    IfFailThenLogErrorAndReturn(colorFrame->get_RawColorImageFormat(&imageFormat));
   

	if (imageFormat == ColorImageFormat_Rgba)
    {
        IfFailThenLogErrorAndReturn(colorFrame->AccessRawUnderlyingBuffer(&colorBufferSize, reinterpret_cast<BYTE**>(&colorBuffer)));
    }
    else if (tmpColorBuffer)
    {
		colorBuffer = tmpColorBuffer;
		colorBufferSize  = ColorWidth * ColorHeight * sizeof(RGBQUAD);
		IfFailThenLogErrorAndReturn(colorFrame->CopyConvertedFrameDataToArray(colorBufferSize, reinterpret_cast<BYTE*>(colorBuffer), ColorImageFormat_Rgba));
    }
	else
    {
        logger()(__FILE__,__LINE__,"Invalid image format");
		return;
    }


    // get body index frame data
    IfFailThenLogErrorAndReturn(bodyIndexFrame->get_FrameDescription(&bodyIndexFrameDescription.get()));
    IfFailThenLogErrorAndReturn(bodyIndexFrameDescription->get_Width(&bodyIndexWidth));
    IfFailThenLogErrorAndReturn(bodyIndexFrameDescription->get_Height(&bodyIndexHeight));
    IfFailThenLogErrorAndReturn(bodyIndexFrame->AccessUnderlyingBuffer(&bodyIndexBufferSize, &bodyIndexBuffer));            

	// if update() reached here, that means that we have valid RGB, depth and skeleton data to be processed
    ProcessFrame(depthBuffer, depthWidth, depthHeight, 
                    colorBuffer, colorWidth, colorHeight,
                    bodyIndexBuffer, bodyIndexWidth, bodyIndexHeight);


}

void KinectPointCloud::ProcessFrame(const UINT16* depthBuffer, int depthHeight, int depthWidth,
	const RGBQUAD* colorBuffer, int colorWidth, int colorHeight,
	const BYTE* bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight)
{
	RGBQUAD *output = reinterpret_cast<RGBQUAD*> (texture.getPixels());
	

	HRESULT hr = coordinateMapper->MapColorFrameToDepthSpace(depthWidth * depthHeight, (UINT16*)depthBuffer, colorWidth * colorHeight, depthCoordinates);
	if (SUCCEEDED(hr))
	{
		userInFrame = false;
		checkUserPosition( depthHeight, depthWidth, bodyIndexBuffer);

		for (int colorIndex = 0; colorIndex < (colorWidth * colorHeight); ++colorIndex)
		{
			const RGBQUAD* source = &invisible;
			DepthSpacePoint depth = depthCoordinates[colorIndex];

			if (depth.X != -std::numeric_limits<float>::infinity() && depth.Y != -std::numeric_limits<float>::infinity())
			{
				int depthX = static_cast<int>(depth.X + 0.5f);
				int depthY = static_cast<int>(depth.Y + 0.5f);
				if ((depthX >= 0 && depthX < depthWidth) && (depthY >= 0 && depthY < depthHeight))
				{
					BYTE player = bodyIndexBuffer[depthX + (depthY * DepthWidth)];
					if (player != 0xff)
					{
						if (!userInFrame){ userInFrame = true; }
						source = colorBuffer + colorIndex;
					}
				}
			}
			output[colorIndex] = *source;
		}

		if(ready2Save)
		{
			if(positionArray < ImageArraySize)
			{
				memcpy(imageArray[positionArray].getPixels(), output, colorWidth*colorHeight*sizeof(RGBQUAD));
				imageArray[positionArray].update();
				positionArray++;
				memcpy(texture.getPixels(), colorBuffer, colorWidth*colorHeight*sizeof(RGBQUAD));
			}
			else
			{
				positionArray = 0;
				ready2Save = false;
			}
		}else memcpy(texture.getPixels(), colorBuffer, colorWidth*colorHeight*sizeof(RGBQUAD));

		texture.update();
	}
}

void KinectPointCloud::save()
{
	for ( int numFiles = 0; numFiles < ImageArraySize; ++numFiles)
	{
		imageArray[numFiles].saveImage("output" + to_string(numFiles) +".png",ofImageQualityType::OF_IMAGE_QUALITY_MEDIUM);
	}
	positionArray = 0;
}



void KinectPointCloud::checkUserPosition( int depthHeight, int depthWidth,	const BYTE* bodyIndexBuffer )
{
	RGBQUAD *output = reinterpret_cast<RGBQUAD*> (texture.getPixels());
		
	int pos = 0;
	userPositioned = false;
	int checkedPixels = 1000;
	for( int x = x1; x < x2 ; x++)
	{
		for(int y = y1; y < y2; y++)
		{	
			pos = x + 1920*y;
			DepthSpacePoint depth = depthCoordinates[pos];
			int depthX = static_cast<int>(depth.X);
			int depthY = static_cast<int>(depth.Y);
			if ((depthX >= 0 && depthX < depthWidth) && (depthY >= 0 && depthY < depthHeight))
			{
				BYTE player = bodyIndexBuffer[depthX + (depthY * DepthWidth)];
				if(player != 0xff)
					checkedPixels--;
			}
		}
	}
	if (checkedPixels <= 0){
		userPositioned = true;
	}
}

