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

typedef std::vector<FrameProcessor*> Vector_processors;

//Helping Functions
void ToggleDisplayFIDProcessor();
void ToggleCalibrationMode();
void SwitchScreen();
void SetView(int view);
void Switchleft();
void Switchright();

Calibrator*		calibrator;

// main window properties
bool	show_fid_processor;
double	process_time;
bool	claibrateMode;
bool	is_running;
bool	bg_substraction;
bool	process_bg;
int		screen_to_show;
bool	is_view_enabled;


IplImage* bgs_image;
IplImage* threshold_beyond_image;
IplImage* threshold_surface_image;
IplImage* threshold_adaptive_image;

int threshold_beyond_value;
int threshold_surface_value;

int view_mode; //0 source, 1 thresholds, 2

bool show_options;
int selected_processor;
Vector_processors processors;
MarkerFinder*   markerfinder;

int main(int argc, char* argv[])
{	
	claibrateMode = false;
	show_fid_processor = false;
	is_running = true;
	bg_substraction = false;
	process_bg = false;
	screen_to_show = 0;
	is_view_enabled = true;

	threshold_surface_value = 45;
	threshold_beyond_value = 179;

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
	bgs_image = cvCreateImage(cvGetSize(Globals::main_image),IPL_DEPTH_8U,1);
	threshold_beyond_image = cvCreateImage(cvGetSize(Globals::main_image),IPL_DEPTH_8U,1);
	threshold_surface_image = cvCreateImage(cvGetSize(Globals::main_image),IPL_DEPTH_8U,1);
	threshold_adaptive_image = cvCreateImage(cvGetSize(Globals::main_image),IPL_DEPTH_8U,1);

	Globals::intrinsic = (CvMat*)cvLoad(M_PATH_INTRINSIC);
	Globals::distortion = (CvMat*)cvLoad(M_PATH_DISTORTION);
	if(Globals::intrinsic == NULL || Globals::distortion == NULL) Globals::LoadDefaultDistortionMatrix();

	// retreive width and height
	Globals::width = cvGetSize(Globals::captured_image).width;
	Globals::height = cvGetSize(Globals::captured_image).height;
	sprintf(Globals::dim,"%ix%i",Globals::width,Globals::height);


	///init objects
	calibrator = new Calibrator();

	show_options = false;
	selected_processor = 0;
	markerfinder = new MarkerFinder();
	processors.push_back(markerfinder);

	TuioServer::Instance().RegisterProcessor(markerfinder);

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
			//bg substraction:
			if(process_bg)
			{
				cvCopy(gray_image,bgs_image);	
				process_bg = false;
			}
			if(bg_substraction && bgs_image != NULL)
			{
				cvAbsDiff(gray_image,bgs_image,gray_image);
			}

			cvSmooth(gray_image,gray_image,CV_GAUSSIAN,3);

			//cvThreshold(gray_image,threshold_surface_image,threshold_beyond_value,255, CV_THRESH_BINARY);
			//cvThreshold(gray_image,threshold_beyond_image,threshold_surface_value,255, CV_THRESH_BINARY);
			//cvAdaptiveThreshold(gray_image,threshold_adaptive_image,255,/*CV_ADAPTIVE_THRESH_MEAN_C*/CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,55,2);
			cvThreshold(gray_image,threshold_adaptive_image,100,255, CV_THRESH_BINARY);

			//find thresholders (surface and beyond)
			//cvThreshold (main_processed_image, main_processed_image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

			/*******Temporally solution****/
			markerfinder->ProcessFrame(threshold_adaptive_image);
			/***********/
		}
		else
		{
			calibrator->ProcessFrame(gray_image);
		}
		
		//Key Check
		presskey=cvWaitKey (100);
		switch(presskey)
		{
			case KEY_EXIT:
				is_running = false;
				break;
			case KEY_CHANGE_VIEW:
				SwitchScreen();
				break;
			case KEY_SHOW_FID_PROCESSOR:
				ToggleDisplayFIDProcessor();
				break;
			case KEY_CALIBRATION:
				ToggleCalibrationMode();
				break;
			case KEY_CALIBRATION_GRID:
			case KEY_RESET:
				if(claibrateMode) calibrator->ProcessKey(presskey);
				break;
			case KEY_ENABLE_BGS:
				bg_substraction = true;
				process_bg = true;
				break;
			case KEY_DISABLE_BGS:
				bg_substraction = false;
				break;
			case 4:
				Switchleft();
				break;
			case 6:
				Switchright();
				break;
			case 'o':
				if(processors.size() != 0)
				{
					show_options = !show_options;
					processors[selected_processor]->EnableKeyProcessor(show_options);
				}
				break;
			case '8':
			case '2':
			case '+':
			case '-':
				if(show_options)
				{
					for(Vector_processors::iterator it = processors.begin(); it != processors.end(); it++)
						(*it)->ProcessKey(presskey);
				}
				break;

				//test
		}

		//calculates the processtime
		process_time = (double)cvGetTickCount()-process_time;
		if(Globals::is_view_enabled)
		{
			if(show_options)
			{
				for(Vector_processors::iterator it = processors.begin(); it != processors.end(); it++)
					(*it)->DrawMenu(Globals::screen);
			}

			sprintf_s(text,"process_time %gms", process_time/(cvGetTickFrequency()*1000.)); 
			Globals::Font::Write(Globals::screen,text,cvPoint(10, 40),FONT_HELP,0,255,0);
			if(screen_to_show == VIEW_RAW)
				cvShowImage(MAIN_TITTLE,Globals::screen);
			else if(screen_to_show == VIEW_THRESHOLD)
			{
				cvShowImage(THRESHOLD_BEYOND_TITTLE,threshold_adaptive_image);
				//cvShowImage(THRESHOLD_SURFACE_TITTLE,threshold_surface_image);
				cvShowImage(THRESHOLD_SURFACE_TITTLE,Globals::screen);
			}
			if(show_fid_processor)cvShowImage(FIDUCIAL_TITTLE,Globals::fiducial_image_marker);
		}

		//update bundle
		TuioServer::Instance().SendBundle();
	}

	//send clear empty tuioMessage
	TuioServer::Instance().SendEmptyBundle();

	cvReleaseCapture(&Globals::cv_camera_capture);
	cvReleaseImage(&Globals::main_image);
	cvReleaseImage(&gray_image);
	cvReleaseImage(&threshold_beyond_image);
	cvReleaseImage(&threshold_surface_image);
	cvReleaseImage(&threshold_adaptive_image);
	delete (markerfinder);
	delete (&TuioServer::Instance());

	//Destroy windows
	if(show_fid_processor)cvDestroyWindow(FIDUCIAL_TITTLE);
	if(screen_to_show == VIEW_THRESHOLD)
	{
		cvNamedWindow (THRESHOLD_BEYOND_TITTLE, CV_WINDOW_AUTOSIZE);
		cvNamedWindow (THRESHOLD_SURFACE_TITTLE, CV_WINDOW_AUTOSIZE);
	}
	cvDestroyWindow(MAIN_TITTLE);
    return 0;

}

void SwitchScreen()
{
	screen_to_show ++;
	if(screen_to_show > VIEW_NONE) screen_to_show = 0;
		SetView(screen_to_show);
}

void SetView(int view)
{
	if(view < 0 || view >= VIEW_NONE)
	{
		screen_to_show = VIEW_NONE;
		is_view_enabled = false;
		cvDestroyWindow(THRESHOLD_BEYOND_TITTLE);
		cvDestroyWindow(THRESHOLD_SURFACE_TITTLE);
		cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
	}
	else
	{
		screen_to_show = view;
		is_view_enabled = true;
		if(screen_to_show == VIEW_RAW)
		{
			cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
			cvDestroyWindow(THRESHOLD_BEYOND_TITTLE);
			cvDestroyWindow(THRESHOLD_SURFACE_TITTLE);
		}
		else if(screen_to_show == VIEW_THRESHOLD)
		{
			//Globals::screen = thresholded_image_marker;
			cvDestroyWindow(MAIN_TITTLE);
			cvNamedWindow (THRESHOLD_BEYOND_TITTLE, CV_WINDOW_AUTOSIZE);
			cvNamedWindow (THRESHOLD_SURFACE_TITTLE, CV_WINDOW_AUTOSIZE);
		}
	}
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

void Switchleft()
{
	if(processors.size() > 1)
	{
		processors[selected_processor]->EnableKeyProcessor(false);
		selected_processor --;
		if(selected_processor < 0) selected_processor = 0;
		processors[selected_processor]->EnableKeyProcessor();
	}
}

void Switchright()
{
	if(processors.size() > 1)
	{
		processors[selected_processor]->EnableKeyProcessor(false);
		selected_processor ++;
		if(selected_processor >= 0) processors.size();
		processors[selected_processor]->EnableKeyProcessor();
	}
}