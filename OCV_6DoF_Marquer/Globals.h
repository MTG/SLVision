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

#define KEY_SHOW_FID_PROCESSOR		'V'
#define KEY_CHANGE_VIEW				'v'
#define KEY_EXIT					27 //esc
#define KEY_CALIBRATION				'c'
#define KEY_CALIBRATION_GRID		'g'
#define KEY_RESET					'r'
#define KEY_ENABLE_BGS				'b'
#define KEY_DISABLE_BGS				'B'
#define KEY_RESET_Z					'z'

//#define MARKER_SIZE (54) //marker size in mm

class Globals
{
protected:
	static float			z_min, z_max;
public:
	/************************
	* Screen Globals
	*************************/
	static IplImage*		screen;				//image to show on the screen
	static bool				is_view_enabled;	//flag window image enabled or disabled
	static int				width;
	static int				height;
	static char				dim[100];
	static CvMat*			intrinsic;
	static CvMat*			distortion;
	static unsigned int		ssidGenerator;

	static void LoadDefaultDistortionMatrix();
	static void UpdateZValues(float z);
	static float GetZValue(float z);
	static void ResetZValues();

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
		//cvPutText(screen, "Y", endpoint, &axisfont,CV_RGB(0,255,0));
	};
};

