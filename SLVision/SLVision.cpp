/*
 * Copyright (C) 2011-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of SLVision
 *
 * SLVision is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/*
 *	Daniel Gallardo Grassot
 *	daniel.gallardo@upf.edu
 *	Barcelona 2011
 */

#include "Globals.h"
#include "MarkerFinder.h"
#include "TuioServer.h"
//#include "Calibrator.h"
#include "GlobalConfig.h"
#include "TouchFinder.h"
#include "HandFinder.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <sstream>


//#define USE_LIVE_VIDEO 1
#ifdef USE_LIVE_VIDEO
#define INFILE "C:\\Users\\daniel\\Desktop\\hand_gestures\\hand_gestures.avi"
#endif

//#define RECORD_VIDEO 1
#ifdef RECORD_VIDEO
#define OUTFILE "out.avi"
#define RECORDFPS 30
#endif

//Window tittles
#define MAIN_TITTLE					"SLVision"
#define CAMERA_TITTLE				"Camera view"

typedef std::vector<FrameProcessor*> Vector_processors;

//Helping Functions
void ToggleCalibrationMode();
void SwitchScreen();
void SetView(int view);
void Switchleft();
void Switchright();

Vector_processors	processors;
//Calibrator*			calibrator;

// main window properties
bool	is_running;
//Camera capture vars
int cameraID;
cv::VideoCapture VCapturer;
//Camera input
cv::Mat InputCamera;
cv::Mat InputCameraGrey;

//Window treatment
cv::Mat EmptyImage;
int enable_view_window;
int enable_marker_window;
int enable_touch_window;
int enable_hand_window;
std::string main_window_tittle;
std::string view_window_tittle;

//interface vars
bool guicreated = false;
int waitTime = 10;

//Frame Processors
std::pair<double,double> AvrgTime(0,0);//average time required for detection
MarkerFinder*	markerfinder;
TouchFinder*	touchfinder;
HandFinder*		handfinder;

///Function headers
void cvEnableView(int pos,void*);
void cvEnableMarker(int pos,void*);
void CreateGUI();

int& show_source = datasaver::GlobalConfig::getRef("MAIN:SHOW_CAMERA_INPUT",0);
int& show_fps = datasaver::GlobalConfig::getRef("MAIN:SHOW_FPS",0);

int main(int argc, char* argv[])
{	
	/******************************************************
	*Camera initialization and initialize frame
	*******************************************************/
	//Open camera
	cameraID = datasaver::GlobalConfig::getRef("MAIN:CAMERA_ID",CAMERA_ID);

#ifdef USE_LIVE_VIDEO
	VCapturer.open(INFILE);
#else
	VCapturer.open(cameraID);
#endif

	if(!VCapturer.isOpened())
	{
		std::cout << "Unable to open camera id: " << cameraID << std::endl;
		return -1;
	}
	is_running = true;
	//capture a frame
	VCapturer>>InputCamera;
	/******************************************************
	* retreive width and height and init camera matrices
	*******************************************************/
	Globals::LoadDefaultDistortionMatrix();
	Globals::width = InputCamera.cols;
	Globals::height = InputCamera.rows;
	std::stringstream s;
	s << Globals::width << "x" << Globals::height;
	Globals::dim = std::string(s.str());
	/******************************************************
	* Init views and Main screen
	*******************************************************/
	main_window_tittle = std::string(MAIN_TITTLE);
	view_window_tittle = std::string(CAMERA_TITTLE);
	EmptyImage = cv::Mat(200,500,CV_8UC1);
	//conf, thres fiducials, thres hands, thres fingers.
	cv::namedWindow(main_window_tittle,CV_WINDOW_AUTOSIZE);
	CreateGUI();
	if(show_source == 1)
	{
		cvEnableView(1,NULL);
		enable_view_window = 1;
	}
	/******************************************************
	* Init Frame Processors
	*******************************************************/
	markerfinder = new MarkerFinder();
	touchfinder = new TouchFinder();
	handfinder = new HandFinder();

	handfinder->SetTouchFinder(touchfinder);

	TuioServer::Instance().RegisterProcessor(markerfinder);
	TuioServer::Instance().RegisterProcessor(touchfinder);
	TuioServer::Instance().RegisterProcessor(handfinder);



	/******************************************************
	* Video recorder
	*******************************************************/
#ifdef RECORD_VIDEO
    int ex = static_cast<int>(CV_FOURCC('D','I','V','X'));     // Get Codec Type- Int form
    // Transform from int to char via Bitwise operators
    char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};

    cv::Size S = cv::Size((int) VCapturer.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) VCapturer.get(CV_CAP_PROP_FRAME_HEIGHT));

	cv::VideoWriter outputVideo;
	
	outputVideo.open(OUTFILE, 
               ex,
               RECORDFPS,
               S);
    if (!outputVideo.isOpened())
    {
        std::cout  << "Could not open the output video for write: " << OUTFILE << std::endl;
    }

	std::cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
         << " of nr#: " << RECORDFPS << std::endl;
    std::cout << "Input codec type: " << EXT << std::endl;

#endif
	

	/******************************************************
	* Main loop app
	*******************************************************/
	while(is_running)
	{
#ifdef USE_LIVE_VIDEO
		if(VCapturer.get(CV_CAP_PROP_POS_FRAMES) >= VCapturer.get(CV_CAP_PROP_FRAME_COUNT))
			VCapturer.set(CV_CAP_PROP_POS_FRAMES,0);
		VCapturer.read( InputCamera);
#else
		VCapturer.retrieve( InputCamera);
#endif
		double tick = (double)cv::getTickCount();
#ifdef RECORD_VIDEO
		if (outputVideo.isOpened())
		outputVideo << InputCamera;
#endif
		/******************************************************
		* Convert image to graycsale
		*******************************************************/
		
		if ( InputCamera.type() ==CV_8UC3 )   cv::cvtColor ( InputCamera,InputCameraGrey,CV_BGR2GRAY );
		Globals::CameraFrame = InputCamera.clone();
		/******************************************************
		* Process Video
		*******************************************************/
		//Find SLFiducials and calculates its pose
		markerfinder->ProcessFrame(InputCameraGrey);
	
		//noise reduction
		cv::GaussianBlur(InputCameraGrey,InputCameraGrey,cv::Size(3,3),0);
		cv::medianBlur(InputCameraGrey,InputCameraGrey,11);
	
		//Find Touches 
		touchfinder->ProcessFrame(InputCameraGrey);
		
		//Find Hands
		handfinder->ProcessFrame(InputCameraGrey);
		/******************************************************
		* GetFramerate
		*******************************************************/
		AvrgTime.first+=((double)cv::getTickCount()-tick)/cv::getTickFrequency();
        AvrgTime.second++;
		/******************************************************
		* Key check
		*******************************************************/
		int key = cv::waitKey(waitTime);//wait for key to be pressed
		switch(key)
		{
		case 27:
			is_running = false;
			break;
		case 'o':
			guicreated = false;
			break;
		case 'f':
			show_fps = !show_fps;
			break;
		}
		/******************************************************
		* Show capture screen
		*******************************************************/
		if(enable_view_window == 1)
		{
			if(Globals::is_view_enabled && show_fps)
			{
				std::stringstream s;
				s << "(" << 1000*AvrgTime.first/AvrgTime.second << ") fps";
				cv::putText(Globals::CameraFrame,s.str(), cv::Point(10,50),cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255,0,0,255),2);
			}
			cv::imshow(view_window_tittle,Globals::CameraFrame);
		}
		/******************************************************
		* Draw GUI
		*******************************************************/
		CreateGUI();
		cv::imshow(main_window_tittle,EmptyImage);
		/******************************************************
		* Sends OSC Data
		*******************************************************/
		markerfinder->RepportOSC();
		touchfinder->RepportOSC();
		handfinder->RepportOSC();
		TuioServer::Instance().SendBundle();
	}
	/******************************************************
	* Sends an empty OSC bundle message
	*******************************************************/
	TuioServer::Instance().SendEmptyBundle();
	/******************************************************
	* Delete finders
	*******************************************************/
	delete(touchfinder);
	delete(markerfinder);
	delete(handfinder);
    return 0;
}

void cvEnableView(int pos,void* name)
{
	if(pos == 0)
	{
		cv::destroyWindow(view_window_tittle);
		Globals::is_view_enabled = false;
		show_source = 0;
	}
	else
	{
		cv::namedWindow(view_window_tittle,CV_WINDOW_AUTOSIZE);
		Globals::is_view_enabled = true;
		show_source= 1;
	}
}

void cvEnableMarker(int pos,void* name)
{
	markerfinder->ShowScreen(pos);
}

void cvEnableTouch(int pos,void* name)
{
	touchfinder->ShowScreen(pos);
}

void cvEnableHand(int pos,void* name)
{
	handfinder->ShowScreen(pos);
}

void CreateGUI()
{
	if(!guicreated)
	{
		cv::createTrackbar("ShowCam", main_window_tittle,&enable_view_window, 1, cvEnableView);
		cv::createTrackbar("ShowMarker", main_window_tittle,&enable_marker_window, 1, cvEnableMarker);
		cv::createTrackbar("ShowTouch", main_window_tittle,&enable_touch_window, 1, cvEnableTouch);
		cv::createTrackbar("ShowHand", main_window_tittle,&enable_hand_window, 1, cvEnableHand);
		guicreated = true;
	}
}
