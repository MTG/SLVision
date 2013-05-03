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

//#define NEWMETHOD 2

#ifndef NEWMETHOD
#include "Globals.h"
#include "MarkerFinder.h"
//#include "TuioServer.h"
//#include "Calibrator.h"
#include "GlobalConfig.h"
//#include "TouchFinder.h"
//#include "HandFinder.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <sstream>


//Window tittles
#define MAIN_TITTLE					"6dof reacTIVision"
#define FIDUCIAL_TITTLE				"Fiducial view"
#define SIX_DOF_THRESHOLD			"Threshold 6 DoF"
#define TOUCH_THRESHOLD				"Threshold Touch"
#define HAND_THRESHOLD				"Threshold hand"

typedef std::vector<FrameProcessor*> Vector_processors;

//Helping Functions
void ToggleCalibrationMode();
void SwitchScreen();
void SetView(int view);
void Switchleft();
void Switchright();


Vector_processors	processors;
//Calibrator*			calibrator;

//TouchFinder*		touchfinder;
//HandFinder*			handfinder;

// main window properties
int64	process_time;
bool	claibrateMode;
bool	is_running;
bool	bg_substraction;
bool	process_bg;

int screen_to_show; //0 source, 1 thresholds, 2 Nothing

bool show_options;
int selected_processor;

//Camera capture vars
int cameraID;
cv::VideoCapture VCapturer;
//Camera input
cv::Mat InputCamera;

//Window treatment
cv::Mat EmptyImage;
int enable_view_window;
int enable_marker_window;
std::string main_window_tittle;
std::string view_window_tittle;

//interface vars
bool guicreated = false;
int waitTime = 10;

//Frame Processors
std::pair<double,double> AvrgTime(0,0);//average time required for detection
MarkerFinder*	markerfinder;

///Function headers
void cvEnableView(int pos,void*);
void cvEnableMarker(int pos,void*);
void CreateGUI();

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

	/******************************************************
	* Init views and Main screen
	*******************************************************/
	main_window_tittle = std::string("SLVision");
	view_window_tittle = std::string("Camera View");
	EmptyImage = cv::Mat(200,500,CV_8UC1);
	//conf, thres fiducials, thres hands, thres fingers.
	cv::namedWindow(main_window_tittle,CV_WINDOW_AUTOSIZE);
	CreateGUI();
	
	/******************************************************
	* Init Frame Processors
	*******************************************************/
	markerfinder = new MarkerFinder();

	/******************************************************
	* Main loop app
	*******************************************************/
	while(is_running)
	{
		VCapturer.retrieve( InputCamera);
		/******************************************************
		* Process Video
		*******************************************************/
		double tick = (double)cv::getTickCount();
		//////
		markerfinder->ProcessFrame(InputCamera);
		//////
		AvrgTime.first+=((double)cv::getTickCount()-tick)/cv::getTickFrequency();
        AvrgTime.second++;
		//std::stringstream s;
		//s << "(" << 1000*AvrgTime.first/AvrgTime.second << ") fps";
		//cv::putText(TheInputImage,s.str(), cv::Point(10,50),cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,255,255),2);
 //       std::cout<<"Time detection="<<1000*AvrgTime.first/AvrgTime.second<<" milliseconds"<<std::endl;
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
		}
		if(enable_view_window == 1)
		{
			cv::imshow(view_window_tittle,InputCamera);
		}
		/******************************************************
		* Draw GUI
		*******************************************************/
		CreateGUI();
		cv::imshow(main_window_tittle,EmptyImage);
	}

#ifdef old_core
	///init objects
//	calibrator = new Calibrator();
	
	markerfinder = new MarkerFinder();
	processors.push_back(markerfinder);
//	TuioServer::Instance().RegisterProcessor(markerfinder);
	
//	touchfinder = new TouchFinder();
//	processors.push_back(touchfinder);
//	TuioServer::Instance().RegisterProcessor(touchfinder);

//	handfinder = new HandFinder();
//	HandFinder::instance = handfinder;
//	processors.push_back(handfinder);
//	TuioServer::Instance().RegisterProcessor(handfinder);

	//SetView
	screen_to_show = datasaver::GlobalConfig::getRef("MAIN:VIEW",0);
	SetView(screen_to_show);

	show_options = false;
	selected_processor = 0;

	// initializes font
	Globals::Font::InitFont();
	char text[255] = "";
	
	//ToggleDisplayFIDProcessor();
	
	while(is_running)
	{
		//Frame update
		process_time = cvGetTickCount();
		captured_image=cvQueryFrame(cv_camera_capture);
		//cvCopy(Globals::captured_image,Globals::main_image);

		cvCvtColor(captured_image, gray_image, CV_BGR2GRAY); 		

		if(!claibrateMode)
		{
			//bg substraction:
			if(process_bg)
			{
				cvCopy(gray_image,bgs_image);	
				process_bg = false;
			}
			if(bg_substraction && bgs_image != NULL)
			{
				cvAbsDiff(bgs_image,gray_image,gray_image);
			}

			cvSmooth(gray_image,gray_image,CV_GAUSSIAN,3);

			//******processors****
			six_dof_output = markerfinder->ProcessFrame(gray_image);
//			touch_finder_output = touchfinder->ProcessFrame(gray_image);
//			hand_finder_output = handfinder->ProcessFrame(gray_image);
			//***********
			//******OSC SEND DATA****
			markerfinder->SendOSCData();
//			touchfinder->SendOSCData();
//			handfinder->SendOSCData();
			
		}
		else
		{
//			calibrator->ProcessFrame(gray_image);
		}
		
		process_time = cvGetTickCount()-process_time;
		if(Globals::is_view_enabled )
		{
			if(screen_to_show == VIEW_RAW || screen_to_show == VIEW_THRESHOLD )
			{
				for(Vector_processors::iterator it = processors.begin(); it != processors.end(); it++)
					(*it)->DrawMenu(Globals::screen);

				sprintf_s(text,"process_time %gms", process_time/(cvGetTickFrequency()*1000.)); 
				Globals::Font::Write(Globals::screen,text,cvPoint(10, 40),FONT_HELP,0,255,0);
				cvShowImage(MAIN_TITTLE,Globals::screen);

			}
			
			if(screen_to_show == VIEW_THRESHOLD)
			{
				if(markerfinder->IsEnabled()) cvShowImage(SIX_DOF_THRESHOLD,six_dof_output);
//				if(touchfinder->IsEnabled()) cvShowImage(TOUCH_THRESHOLD,touch_finder_output);
//				if(handfinder->IsEnabled()) cvShowImage(HAND_THRESHOLD,hand_finder_output);
			}
		}

		//sends OSC bundle or update it
//		TuioServer::Instance().SendBundle();
		//Key Check
		presskey=cvWaitKey (10);
		switch(presskey)
		{
			case KEY_EXIT:
				is_running = false;
				break;
			case KEY_CHANGE_VIEW:
				SwitchScreen();
				break;
			case KEY_CALIBRATION:
				ToggleCalibrationMode();
				break;
			case KEY_CALIBRATION_GRID:
			case KEY_RESET:
			case KEY_PREVIOUS_OPTION_1:
			case KEY_PREVIOUS_OPTION_2:
			case KEY_PREVIOUS_OPTION_3:
				
				if(claibrateMode)
				{
//					calibrator->ProcessKey(presskey);
				}
				else if(show_options)
					Switchleft();
				break;
			case KEY_NEXT_OPTION_1:
			case KEY_NEXT_OPTION_2:
			case KEY_NEXT_OPTION_3:
				if(claibrateMode)
				{
//					calibrator->ProcessKey(presskey);
				}
				else if(show_options)
					Switchright();
				break;
			case KEY_SHOW_OPTIONS_1:
			case KEY_SHOW_OPTIONS_2:
				if(processors.size() != 0)
				{
					show_options = !show_options;
					processors[selected_processor]->EnableKeyProcessor(show_options);
				}
				break;
			case KEY_MENU_UP_1:
			case KEY_MENU_UP_2:
			case KEY_MENU_UP_3:
			case KEY_MENU_DOWN_1:
			case KEY_MENU_DOWN_2:
			case KEY_MENU_DOWN_3:
			case KEY_MENU_INCR_1:
			case KEY_MENU_INCR_2:
			case KEY_MENU_INCR_3:
			case KEY_MENU_DECR_1:
			case KEY_MENU_DECR_2:
			case KEY_MENU_DECR_3:
				if(show_options)
				{
					for(Vector_processors::iterator it = processors.begin(); it != processors.end(); it++)
						(*it)->ProcessKey(presskey);
				}
				else if(claibrateMode)
				{
//					calibrator->ProcessKey(presskey);
				}
				break;
				//test
		}
	} ///end while bucle

	//finishing and erasing data:
//	TuioServer::Instance().SendEmptyBundle(); //sends clear empty tuioMessage
	cvReleaseCapture(&cv_camera_capture);
	cvReleaseImage(&gray_image);
	delete (markerfinder);
//	delete (&TuioServer::Instance());
//	delete (calibrator);
//	delete (touchfinder);
//	delete (handfinder);
	//destroy windows
	cvDestroyWindow(MAIN_TITTLE);
	cvDestroyWindow(SIX_DOF_THRESHOLD);
#endif
#endif
    return 0;
}


void cvEnableView(int pos,void* name)
{
	if(pos == 0)
	{
		cv::destroyWindow(view_window_tittle);
		Globals::is_view_enabled = false;
	}
	else
	{
		cv::namedWindow(view_window_tittle,CV_WINDOW_AUTOSIZE);
		Globals::is_view_enabled = true;
	}
}

void cvEnableMarker(int pos,void* name)
{
	markerfinder->ShowScreen(pos);
}

void CreateGUI()
{
	if(!guicreated)
	{
		cv::createTrackbar("ShowCam", main_window_tittle,&enable_view_window, 1, cvEnableView);
		cv::createTrackbar("ShowMarker", main_window_tittle,&enable_marker_window, 1, cvEnableMarker);
		guicreated = true;
	}
}




#ifndef NEWMETHOD
//void SwitchScreen()
//{
//	screen_to_show ++;
//	if(screen_to_show > VIEW_NONE) screen_to_show = 0;
//		SetView(screen_to_show);
//}
//
//void SetView(int view)
//{
//	int &glob_view = datasaver::GlobalConfig::getRef("MAIN:VIEW",0);
//	if(view < 0 || view >= VIEW_NONE)
//	{
//		screen_to_show = VIEW_NONE;
//		Globals::is_view_enabled = false;
//		cvDestroyWindow(SIX_DOF_THRESHOLD);
//		cvDestroyWindow(TOUCH_THRESHOLD);
//		cvDestroyWindow(MAIN_TITTLE);
//		cvDestroyWindow(HAND_THRESHOLD);
//		cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
//	}
//	else
//	{
//		screen_to_show = view;
//		Globals::is_view_enabled = true;
//		if(screen_to_show == VIEW_RAW)
//		{
//			cvDestroyWindow(HAND_THRESHOLD);
//			cvDestroyWindow(TOUCH_THRESHOLD);
//			cvDestroyWindow(SIX_DOF_THRESHOLD);
//			cvDestroyWindow(MAIN_TITTLE);
//			cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
//		}
//		else if(screen_to_show == VIEW_THRESHOLD)
//		{
//			cvDestroyWindow(HAND_THRESHOLD);
//			cvDestroyWindow(TOUCH_THRESHOLD);
//			cvDestroyWindow(SIX_DOF_THRESHOLD);
//			cvDestroyWindow(MAIN_TITTLE);
//			//frame processors windows
//			if(markerfinder->IsEnabled()) cvNamedWindow (SIX_DOF_THRESHOLD, CV_WINDOW_AUTOSIZE);
////			if(touchfinder->IsEnabled()) cvNamedWindow (TOUCH_THRESHOLD, CV_WINDOW_AUTOSIZE);
////			if(handfinder->IsEnabled()) cvNamedWindow (HAND_THRESHOLD, CV_WINDOW_AUTOSIZE);
//			cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
//		}
//	}
//	glob_view = screen_to_show;
//}

//
//void ToggleCalibrationMode()
//{
//	if(screen_to_show != VIEW_RAW) return;
//	claibrateMode = !claibrateMode;
//	if(claibrateMode)
//	{
////		calibrator->StartCalibration();
//		if(processors.size() != 0)
//		{
//			show_options = false;
//			processors[selected_processor]->EnableKeyProcessor(show_options);
//		}
//
//	}
//	else
//	{
////		calibrator->EndCalibration();
//	}
//}
//
//void Switchleft()
//{
//	if(processors.size() > 1)
//	{
//		processors[selected_processor]->EnableKeyProcessor(false);
//		selected_processor --;
//		if(selected_processor < 0) selected_processor = processors.size() - 1;
//		processors[selected_processor]->EnableKeyProcessor();
//	}
//}
//
//void Switchright()
//{
//	if(processors.size() > 1)
//	{
//		processors[selected_processor]->EnableKeyProcessor(false);
//		selected_processor ++;
//		if(selected_processor >= (int)processors.size()) 
//			selected_processor = 0;
//		processors[selected_processor]->EnableKeyProcessor();
//	}
//}
#endif