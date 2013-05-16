/*
	Daniel Gallardo Grassot
	daniel.gallardo@upf.edu
	Barcelona 2011

	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
*/

#include "Globals.h"
#include "MarkerFinder.h"
#include "TuioServer.h"
//#include "Calibrator.h"
#include "GlobalConfig.h"
#include "TouchFinder.h"
//#include "HandFinder.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <sstream>

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

//Window treatment
cv::Mat EmptyImage;
int enable_view_window;
int enable_marker_window;
int enable_touch_window;
std::string main_window_tittle;
std::string view_window_tittle;

//interface vars
bool guicreated = false;
int waitTime = 10;

//Frame Processors
std::pair<double,double> AvrgTime(0,0);//average time required for detection
MarkerFinder*	markerfinder;
TouchFinder*	touchfinder;
//HandFinder*			handfinder;

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
	VCapturer.open(cameraID);
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
	TuioServer::Instance().RegisterProcessor(markerfinder);
	TuioServer::Instance().RegisterProcessor(touchfinder);
	/******************************************************
	* Main loop app
	*******************************************************/
	while(is_running)
	{
		VCapturer.retrieve( InputCamera);
		double tick = (double)cv::getTickCount();
		Globals::CameraFrame = InputCamera.clone();
		/******************************************************
		* Process Video
		*******************************************************/
		markerfinder->ProcessFrame(InputCamera);
		touchfinder->ProcessFrame(InputCamera);
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


void CreateGUI()
{
	if(!guicreated)
	{
		cv::createTrackbar("ShowCam", main_window_tittle,&enable_view_window, 1, cvEnableView);
		cv::createTrackbar("ShowMarker", main_window_tittle,&enable_marker_window, 1, cvEnableMarker);
		cv::createTrackbar("ShowTouch", main_window_tittle,&enable_touch_window, 1, cvEnableTouch);
		guicreated = true;
	}
}