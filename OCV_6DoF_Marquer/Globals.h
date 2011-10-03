///fer servir el xml configurator que es va fer per al new reactivision.

#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//FONT defines
#define FONT_AXIS					0
#define FONT_HELP					1

//CAMERA constants
#define CAMERA_ID					0

//VIEW constants
#define VIEW_RAW					0
#define VIEW_THRESHOLD				1
#define VIEW_NONE					2
#define MAIN_TITTLE					"6dof reacTIVision"
#define FIDUCIAL_TITTLE				"Fiducial view"
#define THRESHOLD_BEYOND_TITTLE		"Threshold beyond"
#define THRESHOLD_SURFACE_TITTLE	"Threshold surface"

#define APP_NAME					"SLVision"
#define M_PATH_DISTORTION			"distortion.xml"
#define M_PATH_INTRINSIC			"instrinsic.xml"

#define KEY_SHOW_FID_PROCESSOR		'V'
#define KEY_CHANGE_VIEW				'v'
#define KEY_EXIT					27 //esc
#define KEY_CALIBRATION				'c'
#define KEY_CALIBRATION_GRID		'g'
#define KEY_RESET					'r'
#define KEY_ENABLE_BGS				'b'
#define KEY_DISABLE_BGS				'B'

#define MARKER_SIZE (90) //marker size in mm

class Globals
{
public:
	/************************
	* Screen Globals
	*************************/
	static IplImage*		screen;				//image to show on the screen
	static IplImage*		captured_image;		//camera captured image
	static IplImage*		thresholded_image_marker;		//camera captured image
	static IplImage*		fiducial_image_marker;		//camera captured image
	static CvCapture*		cv_camera_capture;	//camera stream capture
	static IplImage*		main_image;			//main image
	static bool				is_view_enabled;	//flag window image enabled or disabled

	static int				screen_to_show; //0 normal, 1 threshold, 2 none

	static int				width;
	static int				height;
	static char				dim[100];
	/************************
	*Tuio Globals
	*************************/
	static char*			address;
	static int				port;
	/************************
	*Globals::distortion Globals
	*************************/
	static CvMat *Globals::intrinsic;
	static CvMat *Globals::distortion;

	static void SetView(int view);
	static void SwitchScreen();

	static void LoadDefaultDistortionMatrix();


	class Font
	{
		static CvFont font_info;
		static CvFont font_axis;
		static float hscale,vscale,italicscale;
		static int thickness;
		static float axis_hscale, axis_vscale;
	public:
		static void InitFont();
		static void Write(IplImage* dest, char* text, const CvPoint &position, int font_type, short r, short g, short b);
		//cvPutText(screen, "Y", endpoint, &axisfont,CV_RGB(0,255,0));
	};
};

