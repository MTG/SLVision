#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include "Globals.h"
#include "MarkerFinder.h"
#include "TuioServer.h"
#include "Calibrator.h"
#include "GlobalConfig.h"
#include "TouchFinder.h"

//VIEW constants
#define VIEW_RAW					0
#define VIEW_THRESHOLD				1
#define VIEW_NONE					2

//Window tittles
#define MAIN_TITTLE					"6dof reacTIVision"
#define FIDUCIAL_TITTLE				"Fiducial view"
#define SIX_DOF_THRESHOLD			"Threshold 6 DoF"
#define TOUCH_THRESHOLD				"Threshold Touch"
//#define THRESHOLD_SURFACE_TITTLE	"Threshold surface"

typedef std::vector<FrameProcessor*> Vector_processors;

//Helping Functions
//void ToggleDisplayFIDProcessor();
void ToggleCalibrationMode();
void SwitchScreen();
void SetView(int view);
void Switchleft();
void Switchright();

Vector_processors	processors;
Calibrator*			calibrator;
MarkerFinder*		markerfinder;
TouchFinder*		touchfinder;

// main window properties
int64	process_time;
bool	claibrateMode;
bool	is_running;
bool	bg_substraction;
bool	process_bg;

int screen_to_show; //0 source, 1 thresholds, 2 Nothing

bool show_options;
int selected_processor;


int main(int argc, char* argv[])
{	
	IplImage*		bgs_image;
	IplImage*		six_dof_output;
	IplImage*		touch_finder_output;
	CvCapture*		cv_camera_capture;
	IplImage*		captured_image;
	IplImage*		gray_image;
	char			presskey;

	claibrateMode			= false;
	is_running				= true;
	bg_substraction			= false;
	process_bg				= false;

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
			//cvAdaptiveThreshold(gray_image,threshold_adaptive_image,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,55,2);//CV_ADAPTIVE_THRESH_MEAN_C
			//cvThreshold(gray_image,threshold_adaptive_image,100,255, CV_THRESH_BINARY);

			//find thresholders (surface and beyond)
			//cvThreshold (main_processed_image, main_processed_image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

			//******Temporally solution****
			six_dof_output = markerfinder->ProcessFrame(gray_image);
			touch_finder_output = touchfinder->ProcessFrame(gray_image);
			//***********
			
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
				cvShowImage(SIX_DOF_THRESHOLD,six_dof_output);
				cvShowImage(TOUCH_THRESHOLD,touch_finder_output);
			}
		}

		//sends OSC bundle or update it
		TuioServer::Instance().SendBundle();
		//Key Check
		presskey=cvWaitKey (70);
		switch(presskey)
		{
			case KEY_EXIT:
				is_running = false;
				break;
			case KEY_CHANGE_VIEW:
				SwitchScreen();
				break;
			case KEY_SHOW_FID_PROCESSOR:
//				ToggleDisplayFIDProcessor();
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
			case '4':
				Switchleft();
				break;
			case '6':
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
	} ///end while bucle

	
	
	//finishing and erasing data:
	TuioServer::Instance().SendEmptyBundle(); //sends clear empty tuioMessage
	cvReleaseCapture(&cv_camera_capture);
	cvReleaseImage(&gray_image);
	delete (markerfinder);
	delete (&TuioServer::Instance());
	//destroy windows
	cvDestroyWindow(MAIN_TITTLE);
	cvDestroyWindow(SIX_DOF_THRESHOLD);
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
	int &glob_view = datasaver::GlobalConfig::getRef("MAIN:VIEW",0);
	if(view < 0 || view >= VIEW_NONE)
	{
		screen_to_show = VIEW_NONE;
		Globals::is_view_enabled = false;
		cvDestroyWindow(SIX_DOF_THRESHOLD);
		cvDestroyWindow(TOUCH_THRESHOLD);
		cvDestroyWindow(MAIN_TITTLE);
		cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
	}
	else
	{
		screen_to_show = view;
		Globals::is_view_enabled = true;
		if(screen_to_show == VIEW_RAW)
		{
			cvDestroyWindow(TOUCH_THRESHOLD);
			cvDestroyWindow(SIX_DOF_THRESHOLD);
			cvDestroyWindow(MAIN_TITTLE);
			cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
		}
		else if(screen_to_show == VIEW_THRESHOLD)
		{
			cvDestroyWindow(TOUCH_THRESHOLD);
			cvDestroyWindow(SIX_DOF_THRESHOLD);
			cvDestroyWindow(MAIN_TITTLE);
			//frame processors windows
			cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
			cvNamedWindow (SIX_DOF_THRESHOLD, CV_WINDOW_AUTOSIZE);
			cvNamedWindow (TOUCH_THRESHOLD, CV_WINDOW_AUTOSIZE);
		}
	}
	glob_view = screen_to_show;
}

/*void ToggleDisplayFIDProcessor()
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
}*/

void ToggleCalibrationMode()
{
	if(screen_to_show != VIEW_RAW) return;
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