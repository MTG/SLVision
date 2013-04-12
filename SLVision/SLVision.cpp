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
//#include <cv.h>
//#include <cxcore.h>
//#include <highgui.h>
#include "Globals.h"
#include "MarkerFinder.h"
#include "TuioServer.h"
#include "Calibrator.h"
#include "GlobalConfig.h"
#include "TouchFinder.h"
#include "HandFinder.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <cstdio>
#include <conio.h>

//VIEW constants
#define VIEW_RAW					0
#define VIEW_THRESHOLD				1
#define VIEW_NONE					2

//Window tittles
#define MAIN_TITTLE					"6dof reacTIVision"
#define FIDUCIAL_TITTLE				"Fiducial view"
#define SIX_DOF_THRESHOLD			"Threshold 6 DoF"
#define TOUCH_THRESHOLD				"Threshold Touch"
#define HAND_THRESHOLD				"Threshold hand"
//#define THRESHOLD_SURFACE_TITTLE	"Threshold surface"

typedef std::vector<FrameProcessor*> Vector_processors;

//Helping Functions
void ToggleCalibrationMode();
void SwitchScreen();
void SetView(int view);
void Switchleft();
void Switchright();


Vector_processors	processors;
Calibrator*			calibrator;
MarkerFinder*		markerfinder;
TouchFinder*		touchfinder;
HandFinder*			handfinder;

// main window properties
int64	process_time;
bool	claibrateMode;
bool	is_running;
bool	bg_substraction;
bool	process_bg;

int screen_to_show; //0 source, 1 thresholds, 2 Nothing

bool show_options;
int selected_processor;

#else
//new_Method
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <sstream>

cv::VideoCapture TheVideoCapturer;
cv::Mat TheInputImage;

std::pair<double,double> AvrgTime(0,0) ;//determines the average time required for detection
int waitTime=10;


cv::Mat out;
cv::Mat grey,thres,thres2,reduced;

double _minSize=0.04;
double _maxSize=0.5;

int perimeter ( std::vector<cv::Point2f> &a );

int a,b;
void cvTackBarEvents(int pos,void*);
//int thresholda=7, thresholdb=7;
#endif

int main(int argc, char* argv[])
{	
#ifdef NEWMETHOD
	
	//Globals::LoadDefaultDistortionMatrix();
	
	//open capture device or  TheVideoCapturer.open(std::string);
	TheVideoCapturer.open(0);
	
	//capture a frame
	TheVideoCapturer>>TheInputImage;

	//SetWindows
	cv::namedWindow("thres",1);
    cv::namedWindow("in",1);
	a = 8;
	b = 12;

	cv::createTrackbar("ThresParam1", "in",&a, 13, cvTackBarEvents);
    cv::createTrackbar("ThresParam2", "in",&b, 13, cvTackBarEvents);

	char key=0;
    int index=0;
	while ( key!=27 && TheVideoCapturer.grab())
    {
		TheVideoCapturer.retrieve( TheInputImage);
		double tick = (double)cv::getTickCount();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//do something
		if ( TheInputImage.type() ==CV_8UC3 )   cv::cvtColor ( TheInputImage,grey,CV_BGR2GRAY );
		else     grey=TheInputImage;

		cv::Mat imgToBeThresHolded=grey;

		//adaptive threshold
		if ( a<3 ) a=3;
        else if ( ( ( int ) a ) %2 !=1 ) a= ( int ) ( a+1 );
		cv::adaptiveThreshold ( imgToBeThresHolded,thres,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY,47,2 );
		

		///detect rectangles
		std::vector<std::vector<cv::Point2f>> MarkerCanditates;

		int minSize=_minSize*std::max(thres.cols,thres.rows)*4;
		int maxSize=_maxSize*std::max(thres.cols,thres.rows)*4;
		std::vector<std::vector<cv::Point> > contours2;
		std::vector<cv::Vec4i> hierarchy2;
		thres.copyTo ( thres2 );
		cv::findContours ( thres2 , contours2, hierarchy2,CV_RETR_TREE, CV_CHAIN_APPROX_NONE );
		cv::vector<cv::Point>  approxCurve;
		///for each contour, analyze if it is a paralelepiped likely to be the marker
	    for ( unsigned int i=0;i<contours2.size();i++ )
		{
			//check it is a possible element by first checking is has enough points
			if ( minSize< contours2[i].size() &&contours2[i].size()<maxSize  )
			{
				approxPolyDP (  contours2[i]  ,approxCurve , double ( contours2[i].size() ) *0.05 , true );
				//check that the poligon has 4 points
				if ( approxCurve.size() ==4 )
				{
					if ( cv::isContourConvex ( cv::Mat ( approxCurve ) ) )
					{
						//ensure that the   distace between consecutive points is large enough
						float minDist=1e10;
						for ( int j=0;j<4;j++ )
						{
							float d= std::sqrt ( ( float ) ( approxCurve[j].x-approxCurve[ ( j+1 ) %4].x ) * ( approxCurve[j].x-approxCurve[ ( j+1 ) %4].x ) +
												 ( approxCurve[j].y-approxCurve[ ( j+1 ) %4].y ) * ( approxCurve[j].y-approxCurve[ ( j+1 ) %4].y ) );
							// 		norm(Mat(approxCurve[i]),Mat(approxCurve[(i+1)%4]));
							if ( d<minDist ) minDist=d;
						}
						//check that distance is not very small
						if ( minDist>10 )
						{
							//add the points
							// 	      cout<<"ADDED"<<endl;
							MarkerCanditates.push_back ( std::vector<cv::Point2f>() );
							//MarkerCanditates.back().idx=i;
							for ( int j=0;j<4;j++ )
							{
								MarkerCanditates.back().push_back ( cv::Point2f ( approxCurve[j].x,approxCurve[j].y ) );
							}
						}
					}
				}
			}
		}
		///sort the points in anti-clockwise order
		std::valarray<bool> swapped(false,MarkerCanditates.size());//used later
		for ( unsigned int i=0;i<MarkerCanditates.size();i++ )
		{

			//trace a line between the first and second point.
			//if the thrid point is at the right side, then the points are anti-clockwise
			double dx1 = MarkerCanditates[i][1].x - MarkerCanditates[i][0].x;
			double dy1 =  MarkerCanditates[i][1].y - MarkerCanditates[i][0].y;
			double dx2 = MarkerCanditates[i][2].x - MarkerCanditates[i][0].x;
			double dy2 = MarkerCanditates[i][2].y - MarkerCanditates[i][0].y;
			double o = ( dx1*dy2 )- ( dy1*dx2 );

			if ( o  < 0.0 )		 //if the third point is in the left side, then sort in anti-clockwise order
			{
				std::swap ( MarkerCanditates[i][1],MarkerCanditates[i][3] );
				swapped[i]=true;
				//sort the contour points
	//  	    reverse(MarkerCanditates[i].contour.begin(),MarkerCanditates[i].contour.end());//????

			}
		}
		/// remove these elements whise corners are too close to each other
		//first detect candidates
		std::vector<std::pair<int,int>  > TooNearCandidates;
		for ( unsigned int i=0;i<MarkerCanditates.size();i++ )
		{
			// 	cout<<"Marker i="<<i<<MarkerCanditates[i]<<endl;
			//calculate the average distance of each corner to the nearest corner of the other marker candidate
			for ( unsigned int j=i+1;j<MarkerCanditates.size();j++ )
			{
				float dist=0;
				for ( int c=0;c<4;c++ )
					dist+= sqrt ( ( MarkerCanditates[i][c].x-MarkerCanditates[j][c].x ) * ( MarkerCanditates[i][c].x-MarkerCanditates[j][c].x ) + ( MarkerCanditates[i][c].y-MarkerCanditates[j][c].y ) * ( MarkerCanditates[i][c].y-MarkerCanditates[j][c].y ) );
				dist/=4;
				//if distance is too small
				if ( dist< 10 )
				{
					TooNearCandidates.push_back ( std::pair<int,int> ( i,j ) );
				}
			}
		}
		//mark for removal the element of  the pair with smaller perimeter
		std::valarray<bool> toRemove ( false,MarkerCanditates.size() );
		for ( unsigned int i=0;i<TooNearCandidates.size();i++ )
		{
			if ( perimeter ( MarkerCanditates[TooNearCandidates[i].first ] ) >perimeter ( MarkerCanditates[ TooNearCandidates[i].second] ) )
				toRemove[TooNearCandidates[i].second]=true;
			else toRemove[TooNearCandidates[i].first]=true;
		}

		for (size_t i=0;i<MarkerCanditates.size();i++) 
		{
			if (!toRemove[i]) 
			{
				/*for(int k = 0; k < 4; k++)
				{
					if (k!= 3)
					cv::line(TheInputImage,MarkerCanditates[i][k],MarkerCanditates[i][k+1],cv::Scalar(0,0,255,255),1,CV_AA);
					else
						cv::line(TheInputImage,MarkerCanditates[i][k],MarkerCanditates[i][0],cv::Scalar(0,0,255,255),1,CV_AA);
				}*/
			}
		}
		//finally, assign to the remaining candidates the contour
		//OutMarkerCanditates.reserve(MarkerCanditates.size());
		/*for (size_t i=0;i<MarkerCanditates.size();i++) {
        if (!toRemove[i]) {
			for(int k = 0; k < 4; i++)
			{
				if (k!= 3)
				cv::line(TheInputImage,MarkerCanditates[i][k],MarkerCanditates[i][k+1],cv::Scalar(0,0,255,255),1,CV_AA);
				else
					cv::line(TheInputImage,MarkerCanditates[i][k],MarkerCanditates[i][0],cv::Scalar(0,0,255,255),1,CV_AA);
			}
           // OutMarkerCanditates.push_back(MarkerCanditates[i]);
           // OutMarkerCanditates.back().contour=contours2[ MarkerCanditates[i].idx];
           // if (swapped[i] && _enableCylinderWarp )//if the corners where swapped, it is required to reverse here the points so that they are in the same order
           //     reverse(OutMarkerCanditates.back().contour.begin(),OutMarkerCanditates.back().contour.end());//????
        }
    }*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		AvrgTime.first+=((double)cv::getTickCount()-tick)/cv::getTickFrequency();
        AvrgTime.second++;
		std::stringstream s;
		//s << "(" << 1000*AvrgTime.first/AvrgTime.second << ") fps";
		//cv::putText(TheInputImage,s.str(), cv::Point(10,50),cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,255,255),2);
        std::cout<<"Time detection="<<1000*AvrgTime.first/AvrgTime.second<<" milliseconds"<<std::endl;

		cv::imshow("in",TheInputImage);
		cv::imshow("thres",thres);





		key=cv::waitKey(waitTime);//wait for key to be pressed
	}
#else
	
	//Print keymapping:
	std::cout	<< "KeyMapping:" << "\n"
				<< "Esc" << ":\t " << "Exit SLVision." << "\n"
				<< KEY_CHANGE_VIEW << ":\t " << "Change view (when anything is displayed, performance is enhanced)." << "\n"
				<< KEY_CALIBRATION << ":\t " << "Opens Calibration window." << "\n"
				<< "o" << ":\t " << "Shows option dialog:" << "\n"
				<< "\t   " << "4 , 6" << ":\t " << "Changes processor." << "\n"
				<< "\t   " << "8 , 2" << ":\t " << "Switchs processor options." << "\n"
				<< "\t   " << "+ , -" << ":\t " << "Changes options values." << "\n" ;
	//
	IplImage*		bgs_image;
	IplImage*		six_dof_output;
	IplImage*		touch_finder_output;
	IplImage*		hand_finder_output;
	CvCapture*		cv_camera_capture;
	IplImage*		captured_image;
	IplImage*		gray_image;
	char			presskey;

	claibrateMode			= false;
	is_running				= true;
	bg_substraction			= false;
	process_bg				= false;

	Globals::LoadDefaultDistortionMatrix();

	//Camera initialization and initialize frame
	cv_camera_capture = cvCaptureFromCAM(CAMERA_ID);		//allocates and initialized the CvCapture structure for reading a video stream from the camera
	if(cv_camera_capture == NULL) 
		return -1;
	captured_image = cvQueryFrame(cv_camera_capture);		//grabs a frame from camera
	Globals::screen = captured_image;
	gray_image = cvCreateImage(cvGetSize(captured_image),IPL_DEPTH_8U,1);
	bgs_image = cvCreateImage(cvGetSize(captured_image),IPL_DEPTH_8U,1);

	// retreive width and height
	Globals::width = cvGetSize(captured_image).width;
	Globals::height = cvGetSize(captured_image).height;
	sprintf(Globals::dim,"%ix%i",Globals::width,Globals::height);

	///init objects
	calibrator = new Calibrator();
	
	markerfinder = new MarkerFinder();
	processors.push_back(markerfinder);
	TuioServer::Instance().RegisterProcessor(markerfinder);
	
	touchfinder = new TouchFinder();
	processors.push_back(touchfinder);
	TuioServer::Instance().RegisterProcessor(touchfinder);

	handfinder = new HandFinder();
	HandFinder::instance = handfinder;
	//Globals::hand_finder = handfinder;
	processors.push_back(handfinder);
	TuioServer::Instance().RegisterProcessor(handfinder);

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
			touch_finder_output = touchfinder->ProcessFrame(gray_image);
			hand_finder_output = handfinder->ProcessFrame(gray_image);
			//***********
			//******OSC SEND DATA****
			markerfinder->SendOSCData();
			touchfinder->SendOSCData();
			handfinder->SendOSCData();
			
		}
		else
		{
			calibrator->ProcessFrame(gray_image);
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
				if(touchfinder->IsEnabled()) cvShowImage(TOUCH_THRESHOLD,touch_finder_output);
				if(handfinder->IsEnabled()) cvShowImage(HAND_THRESHOLD,hand_finder_output);
			}
		}

		//sends OSC bundle or update it
		TuioServer::Instance().SendBundle();
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
//			case KEY_SHOW_FID_PROCESSOR:
//				ToggleDisplayFIDProcessor();
//				break;
			case KEY_CALIBRATION:
				ToggleCalibrationMode();
				break;
			case KEY_CALIBRATION_GRID:
			case KEY_RESET:
//			case KEY_RESET_Z:
//				if(claibrateMode) calibrator->ProcessKey(presskey);
//				break;
			case KEY_PREVIOUS_OPTION_1:
			case KEY_PREVIOUS_OPTION_2:
			case KEY_PREVIOUS_OPTION_3:
				
				if(claibrateMode)
				{
					calibrator->ProcessKey(presskey);
				}
				else if(show_options)
					Switchleft();
				break;
			case KEY_NEXT_OPTION_1:
			case KEY_NEXT_OPTION_2:
			case KEY_NEXT_OPTION_3:
				if(claibrateMode)
				{
					calibrator->ProcessKey(presskey);
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
					calibrator->ProcessKey(presskey);
				}
				break;
				//test
		}
	} ///end while bucle

	//finishing and erasing data:
	TuioServer::Instance().SendEmptyBundle(); //sends clear empty tuioMessage
	cvReleaseCapture(&cv_camera_capture);
	cvReleaseImage(&gray_image);
	delete (markerfinder);
	delete (&TuioServer::Instance());
	delete (calibrator);
	delete (touchfinder);
	delete (handfinder);
	//destroy windows
	cvDestroyWindow(MAIN_TITTLE);
	cvDestroyWindow(SIX_DOF_THRESHOLD);
#endif
    return 0;
}

#ifndef NEWMETHOD
void SwitchScreen()
{
	screen_to_show ++;
	if(screen_to_show > VIEW_NONE) screen_to_show = 0;
		SetView(screen_to_show);
}

void SetView(int view)
{
	int &glob_view = datasaver::GlobalConfig::getRef("MAIN:VIEW",0);
	if(view < 0 || view >= VIEW_NONE)
	{
		screen_to_show = VIEW_NONE;
		Globals::is_view_enabled = false;
		cvDestroyWindow(SIX_DOF_THRESHOLD);
		cvDestroyWindow(TOUCH_THRESHOLD);
		cvDestroyWindow(MAIN_TITTLE);
		cvDestroyWindow(HAND_THRESHOLD);
		cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
	}
	else
	{
		screen_to_show = view;
		Globals::is_view_enabled = true;
		if(screen_to_show == VIEW_RAW)
		{
			cvDestroyWindow(HAND_THRESHOLD);
			cvDestroyWindow(TOUCH_THRESHOLD);
			cvDestroyWindow(SIX_DOF_THRESHOLD);
			cvDestroyWindow(MAIN_TITTLE);
			cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
		}
		else if(screen_to_show == VIEW_THRESHOLD)
		{
			cvDestroyWindow(HAND_THRESHOLD);
			cvDestroyWindow(TOUCH_THRESHOLD);
			cvDestroyWindow(SIX_DOF_THRESHOLD);
			cvDestroyWindow(MAIN_TITTLE);
			//frame processors windows
			if(markerfinder->IsEnabled()) cvNamedWindow (SIX_DOF_THRESHOLD, CV_WINDOW_AUTOSIZE);
			if(touchfinder->IsEnabled()) cvNamedWindow (TOUCH_THRESHOLD, CV_WINDOW_AUTOSIZE);
			if(handfinder->IsEnabled()) cvNamedWindow (HAND_THRESHOLD, CV_WINDOW_AUTOSIZE);
			cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
		}
	}
	glob_view = screen_to_show;
}


void ToggleCalibrationMode()
{
	if(screen_to_show != VIEW_RAW) return;
	claibrateMode = !claibrateMode;
	if(claibrateMode)
	{
		calibrator->StartCalibration();
		if(processors.size() != 0)
		{
			show_options = false;
			processors[selected_processor]->EnableKeyProcessor(show_options);
		}

	}
	else
	{
		calibrator->EndCalibration();
	}
}

void Switchleft()
{
	if(processors.size() > 1)
	{
		processors[selected_processor]->EnableKeyProcessor(false);
		selected_processor --;
		if(selected_processor < 0) selected_processor = processors.size() - 1;
		processors[selected_processor]->EnableKeyProcessor();
	}
}

void Switchright()
{
	if(processors.size() > 1)
	{
		processors[selected_processor]->EnableKeyProcessor(false);
		selected_processor ++;
		if(selected_processor >= (int)processors.size()) 
			selected_processor = 0;
		processors[selected_processor]->EnableKeyProcessor();
	}
}
#else
int perimeter ( std::vector<cv::Point2f> &a )
{
    int sum=0;
    for ( unsigned int i=0;i<a.size();i++ )
    {
        int i2= ( i+1 ) %a.size();
        sum+= sqrt ( ( a[i].x-a[i2].x ) * ( a[i].x-a[i2].x ) + ( a[i].y-a[i2].y ) * ( a[i].y-a[i2].y ) ) ;
    }
    return sum;
}

void cvTackBarEvents(int pos,void*)
{
	std::cout << a << " " << b << std::endl;
   /* if (a<3) a=3;
    if (a%2!=1) a++;
    if (ThresParam2<1) ThresParam2=1;
    ThresParam1=iThresParam1;
    ThresParam2=iThresParam2;*/
 /*   MDetector.setThresholdParams(ThresParam1,ThresParam2);
//recompute
    MDetector.detect(TheInputImage,TheMarkers,TheCameraParameters);
    TheInputImage.copyTo(TheInputImageCopy);
    for (unsigned int i=0;i<TheMarkers.size();i++)	TheMarkers[i].draw(TheInputImageCopy,Scalar(0,0,255),1);
    //print other rectangles that contains no valid markers


//draw a 3d cube in each marker if there is 3d info
    if (TheCameraParameters.isValid())
        for (unsigned int i=0;i<TheMarkers.size();i++)
            CvDrawingUtils::draw3dCube(TheInputImageCopy,TheMarkers[i],TheCameraParameters);

    cv::imshow("in",TheInputImageCopy);
    cv::imshow("thres",MDetector.getThresholdedImage());
	*/
}
#endif