#pragma once

#include <Kinect.h>
#include <ofImage.h>

/**
 * Basic Kinect v2 point cloud generator
 * (Fairly based on CoordinateMappingBasics-D2D)
 */
namespace NUI
{
	static const int        DepthWidth  = 512;
    static const int        DepthHeight = 424;
    static const int        ColorWidth  = 1920;
    static const int        ColorHeight = 1080;
	static const int		ImageArraySize = 30;
	static const int		x1=1200, y1=300, x2=1240, y2=340;
	static const RGBQUAD	invisible = {0};
	static const RGBQUAD	blue = {0, 0, 255, 255};
	static const RGBQUAD	red = {255, 0, 0, 255};
	

	class KinectPointCloud
	{
	public:
		KinectPointCloud(void);
		~KinectPointCloud(void);

		void setup();
		void update();
		void save();
		void ProcessFrame(const UINT16* depthBuffer, int depthHeight, int depthWidth, 
						  const RGBQUAD* colorBuffer, int colorWidth, int colorHeight,
						  const BYTE* bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight);
		bool connected() { return kinectSensor != NULL; }
		bool failed() { return kinectFailed; }
		int getArraySize() { return ImageArraySize; }

		void checkUserPosition( int depthHeight, int depthWidth, const BYTE* bodyIndexBuffer );
		bool isUserPositioned(){ return userPositioned; }
		bool hasUserInFrame(){ return userInFrame; }
		void repeatVideoRecorded( int colorWidth, int colorHeight);


		void draw_image(int x=0, int y=0, int w=ColorWidth, int h=ColorHeight) {  texture.draw(x,y,w,h); };
		void KinectPointCloud::drawFromArray(int x=0, int y=0, int w=ColorWidth, int h=ColorHeight)
		{
			imageArray[imageArrayIdx].draw(x,y,w,h);
			imageArrayIdx = (imageArrayIdx + 1) % ImageArraySize;
		}


		int positionArray;
		bool ready2Save;
		ofImage* imageArray;
		int imageArrayIdx;



	private:
		// Current Kinect
		IKinectSensor*     kinectSensor;
		ICoordinateMapper* coordinateMapper;
		DepthSpacePoint*   depthCoordinates;
		IBodyFrameReader* m_pBodyFrameReader;

		// Frame reader
		IMultiSourceFrameReader* multiSourceFrameReader;

		// Buffer used when the frame doesn't come in RGBA
		// and needs to be converted
		RGBQUAD* tmpColorBuffer;
		
		// Resulting texture
		ofImage texture;


		//
		bool userInFrame;
		bool userPositioned;
		bool kinectFailed;
	};
}

