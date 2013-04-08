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

#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//FONT defines
#define FONT_AXIS					0
#define FONT_HELP					1

//CAMERA constants
#define CAMERA_ID					0

#define APP_NAME					"SLVision"
#define M_PATH_DISTORTION			"distortion.xml"
#define M_PATH_INTRINSIC			"instrinsic.xml"

//#define KEY_SHOW_FID_PROCESSOR		'V'
#define KEY_CHANGE_VIEW				'v'
#define KEY_EXIT					27 //esc
#define KEY_CALIBRATION				'c'
#define KEY_CALIBRATION_GRID		'g'
#define KEY_RESET					'r'
//#define KEY_ENABLE_BGS				'b'
//#define KEY_DISABLE_BGS				'B'
//#define KEY_RESET_Z					'z'
#define KEY_NEXT_OPTION_1			'6'
#define KEY_NEXT_OPTION_2			'd'
#define KEY_NEXT_OPTION_3			'D'

#define KEY_PREVIOUS_OPTION_1		'4'
#define KEY_PREVIOUS_OPTION_2		'a'
#define KEY_PREVIOUS_OPTION_3		'A'

#define KEY_SHOW_OPTIONS_1			'o'
#define KEY_SHOW_OPTIONS_2			'O'

#define KEY_MENU_UP_1				'8'
#define KEY_MENU_UP_2				'w'
#define KEY_MENU_UP_3				'W'

#define KEY_MENU_DOWN_1				'2'
#define KEY_MENU_DOWN_2				's'
#define KEY_MENU_DOWN_3				'S'

#define KEY_MENU_INCR_1				'+'
#define KEY_MENU_INCR_2				'e'
#define KEY_MENU_INCR_3				'E'

#define KEY_MENU_DECR_1				'-'
#define KEY_MENU_DECR_2				'q'
#define KEY_MENU_DECR_3				'Q'

//#define MARKER_SIZE (54) //marker size in mm

class Globals
{
protected:
//	static float			z_min, z_max;
public:
	/************************
	* Camera parameters
	*************************/
	static cv::Mat CameraMatrix; // 3x3 matrix (fx 0 cx, 0 fy cy, 0 0 1)
	static cv::Mat Distortion; //4x1 matrix (k1,k2,p1,p2)
    static cv::Size CamSize; //size of the image

	/************************
	* Screen Globals
	*************************/
	static IplImage*		screen;				//image to show on the screen
	static bool				is_view_enabled;	//flag window image enabled or disabled
	static int				width;
	static int				height;
	static char				dim[100];
	//static CvMat*			intrinsic;
	//static CvMat*			distortion;
	static unsigned int		ssidGenerator;

	static void LoadDefaultDistortionMatrix();
	static void UpdateZValues(float z);

	static int				&calib_x_max;
	static int				&calib_x_min;
	static int				&calib_y_max;
	static int				&calib_y_min;


	static float GetX(int coord);
	static float GetY(int coord);

	class Font
	{
		static CvFont font_info;
		static CvFont font_axis;
		static float hscale,vscale,italicscale;
		static int thickness;
		static float axis_hscale, axis_vscale;
	public:
		static void InitFont();
		static void Write(IplImage* dest,const char* text, const CvPoint &position, int font_type, short r, short g, short b);
	};
};

