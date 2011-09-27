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

//Helping Functions
void ToggleDisplayFIDProcessor();

MarkerFinder*   markerfinder;

bool show_fid_processor;
double	process_time;


int main(int argc, char* argv[])
{	
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
	markerfinder = new MarkerFinder();
	TuioServer::Instance().RegisterProcessor(markerfinder);

	Globals::width = cvGetSize(Globals::captured_image).width;
	Globals::height = cvGetSize(Globals::captured_image).height;
	sprintf(Globals::dim,"%ix%i",Globals::width,Globals::height);

	Globals::Font::InitFont();
	char text[255] = "";

	//Window Setup
	cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
	Globals::SetView(VIEW_RAW);
	show_fid_processor = true;
	ToggleDisplayFIDProcessor();

	while(1)
	{
		//Frame update
		process_time = (double)cvGetTickCount();
		Globals::captured_image=cvQueryFrame(Globals::cv_camera_capture);
		cvCopy(Globals::captured_image,Globals::main_image);
		/***********/
		markerfinder->ProcessFrame(Globals::main_image);
		/***********/
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
		if(presskey==27) break;
		else if(presskey=='v')
		{
			Globals::SwitchScreen();
		}
		else if(presskey == 'V')
			ToggleDisplayFIDProcessor();

		TuioServer::Instance().SendBundle();
	}

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
