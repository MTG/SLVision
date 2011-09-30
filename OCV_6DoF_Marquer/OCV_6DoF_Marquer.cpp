// OCV_6DoF_Marquer.cpp : Defines the entry point for the console application.
//

////#include "StdAfx.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>

#include "Globals.h"
#include "MarkerFinder.h"
#include "TuioServer.h"
#include "Calibrator.h"

#define KEY_SHOW_FID_PROCESSOR	'V'
#define KEY_CHANGE_VIEW			'v'
#define KEY_EXIT				27 //esc
#define KEY_CALIBRATION			'c'
#define KEY_CALIBRATION_GRID	'g'

//Helping Functions
void ToggleDisplayFIDProcessor();
void ToggleCalibrationMode();

// markerfinder defined
MarkerFinder*   markerfinder;
Calibrator*		calibrator;

// main window properties
bool	show_fid_processor;
double	process_time;
bool	claibrateMode;
bool	is_running;


int main(int argc, char* argv[])
{	
	claibrateMode = false;
	show_fid_processor = false;
	is_running = true;

	Globals::cv_camera_capture = NULL;
	char presskey;
	
	//Camera initialization
	Globals::cv_camera_capture = cvCaptureFromCAM(CAMERA_ID);								//allocates and initialized the CvCapture structure for reading a video stream from the camera
	if(Globals::cv_camera_capture == NULL) return -1;
	Globals::captured_image=cvQueryFrame(Globals::cv_camera_capture);						//grabs a frame from camera
	
	//main frame allocation
	Globals::main_image = cvCreateImage(cvGetSize(Globals::captured_image),IPL_DEPTH_8U,3);			//allocates 3-channel memory for the image
	cvCopy(Globals::captured_image,Globals::main_image);												//copy data into captured image
	Globals::screen = Globals::main_image;

	IplImage* gray_image;
	gray_image = cvCreateImage(cvGetSize(Globals::main_image),IPL_DEPTH_8U,1);


	Globals::intrinsic = (CvMat*)cvLoad(M_PATH_INTRINSIC);
	Globals::distortion = (CvMat*)cvLoad(M_PATH_DISTORTION);
	if(Globals::intrinsic == NULL) //if not loaded load default values
	{
		Globals::intrinsic  = cvCreateMat(3,3,CV_32FC1);
	
		CV_MAT_ELEM( *Globals::intrinsic, float, 0, 0) = 6.57591187e+002;	CV_MAT_ELEM( *Globals::intrinsic, float, 0, 1) = 0. ;				CV_MAT_ELEM( *Globals::intrinsic, float, 0, 2) = 3.16504272e+002;
		CV_MAT_ELEM( *Globals::intrinsic, float, 1, 0) = 0.;				CV_MAT_ELEM( *Globals::intrinsic, float, 1, 1) = 6.60952637e+002;	CV_MAT_ELEM( *Globals::intrinsic, float, 1, 2) = 2.27605789e+002;
		CV_MAT_ELEM( *Globals::intrinsic, float, 2, 0) = 0.;				CV_MAT_ELEM( *Globals::intrinsic, float, 2, 1) = 0.;				CV_MAT_ELEM( *Globals::intrinsic, float, 2, 2) = 1.;
	}
	if(Globals::distortion == NULL) //if not loaded load default values
	{
		Globals::distortion = cvCreateMat(4,1,CV_32FC1);
		CV_MAT_ELEM( *Globals::distortion, float, 0, 0) = -1.49060376e-002; 
		CV_MAT_ELEM( *Globals::distortion, float, 1, 0) = 2.05916256e-001; 
		CV_MAT_ELEM( *Globals::distortion, float, 2, 0) = -5.76808210e-003;
		CV_MAT_ELEM( *Globals::distortion, float, 3, 0) = -8.43471102e-003; 
	}

	calibrator = new Calibrator();
	markerfinder = new MarkerFinder();
	TuioServer::Instance().RegisterProcessor(markerfinder);

	// retreive width and height
	Globals::width = cvGetSize(Globals::captured_image).width;
	Globals::height = cvGetSize(Globals::captured_image).height;
	sprintf(Globals::dim,"%ix%i",Globals::width,Globals::height);

	// initializes font
	Globals::Font::InitFont();
	char text[255] = "";

	//Window Setup ( creates main window) )
	cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
	Globals::SetView(VIEW_RAW);
	
	//ToggleDisplayFIDProcessor();

	while(is_running)
	{
		//Frame update
		process_time = (double)cvGetTickCount();
		Globals::captured_image=cvQueryFrame(Globals::cv_camera_capture);
		cvCopy(Globals::captured_image,Globals::main_image);

		cvCvtColor(Globals::main_image, gray_image, CV_BGR2GRAY); 

		if(!claibrateMode)
		{
			/********TODO*****
			* - Calculate the double thresholder (avobe(di) and on (ftir)
			* - markerfinder->ProcessFrame(thresholder_above);
			* - handfinder->ProcessFrame(thresholder_above);
			* - fingerfeedback->Processframe(thresholdet_on);
			******************/

			/*******Temporally solution****/
			markerfinder->ProcessFrame(Globals::main_image);
			/***********/
		}
		else
		{
			calibrator->ProcessFrame(gray_image);
		}

		//calculates the processtime
		process_time = (double)cvGetTickCount()-process_time;
		if(Globals::is_view_enabled)
		{
			sprintf_s(text,"process_time %gms", process_time/(cvGetTickFrequency()*1000.)); 
			Globals::Font::Write(Globals::screen,text,cvPoint(10, 40),FONT_HELP,0,255,0);
			cvShowImage(MAIN_TITTLE,Globals::screen);
			if(show_fid_processor)cvShowImage(FIDUCIAL_TITTLE,Globals::fiducial_image_marker);
		}
		
		//Key Check
		presskey=cvWaitKey (100);
		switch(presskey)
		{
			case KEY_EXIT:
				is_running = false;
				break;
			case KEY_CHANGE_VIEW:
				Globals::SwitchScreen();
				break;
			case KEY_SHOW_FID_PROCESSOR:
				ToggleDisplayFIDProcessor();
				break;
			case KEY_CALIBRATION:
				ToggleCalibrationMode();
				break;
			case KEY_CALIBRATION_GRID:
				if(claibrateMode) calibrator->ProcessKey(presskey);
				break;
		}

		//update bundle
		TuioServer::Instance().SendBundle();
	}

	//send clear empty tuioMessage
	TuioServer::Instance().SendEmptyBundle();

	cvReleaseCapture(&Globals::cv_camera_capture);
	cvReleaseImage(&Globals::main_image);
	delete (markerfinder);
	delete (&TuioServer::Instance());

	//Destroy windows
	if(show_fid_processor)cvDestroyWindow(FIDUCIAL_TITTLE);
	cvDestroyWindow(MAIN_TITTLE);
    return 0;

}

void ToggleDisplayFIDProcessor()
{
	if(claibrateMode) return;
	show_fid_processor = ! show_fid_processor;
	if(show_fid_processor)
	{
		cvNamedWindow (FIDUCIAL_TITTLE, CV_WINDOW_AUTOSIZE);
	}
	else
	{
		cvDestroyWindow(FIDUCIAL_TITTLE);
	}
}

void ToggleCalibrationMode()
{
	if(show_fid_processor) return;
	claibrateMode = !claibrateMode;
	if(claibrateMode)
	{
		calibrator->StartCalibration();
	}
	else
	{
		calibrator->EndCalibration();
	}
}