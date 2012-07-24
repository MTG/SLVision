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
#include "GlobalConfig.h"
#include <iostream>

IplImage*		Globals::screen						= NULL;
bool			Globals::is_view_enabled			= false;

float			Globals::Font::hscale				= 0.5f;
float			Globals::Font::vscale				= 0.5f;
float			Globals::Font::italicscale			= 0.0f;
int 			Globals::Font::thickness			= 1;
float			Globals::Font::axis_hscale			= 0.8f;
float			Globals::Font::axis_vscale			= 0.8f;
CvFont			Globals::Font::font_info;
CvFont			Globals::Font::font_axis;

int				Globals::width						= 0;
int				Globals::height						= 0;
char			Globals::dim[100];

CvMat* 			Globals::intrinsic					= NULL;
CvMat*			Globals::distortion					= NULL;

unsigned int	Globals::ssidGenerator				= 1;

//float			Globals::z_min						= -1.0f;
//float			Globals::z_max						= -1.0f;
int				&Globals::calib_x_max = datasaver::GlobalConfig::getRef("MAIN:CALIBRATOR:TABLE:XMAX",Globals::width);
int				&Globals::calib_x_min = datasaver::GlobalConfig::getRef("MAIN:CALIBRATOR:TABLE:XMIN",0);
int				&Globals::calib_y_max = datasaver::GlobalConfig::getRef("MAIN:CALIBRATOR:TABLE:YMAX",(int)(Globals::height));
int				&Globals::calib_y_min = datasaver::GlobalConfig::getRef("MAIN:CALIBRATOR:TABLE:YMIN",0);

void Globals::LoadDefaultDistortionMatrix()
{
	if(intrinsic != NULL)
	{
		cvReleaseMat(&intrinsic);
	}
	intrinsic  = cvCreateMat(3,3,CV_32FC1);
	CV_MAT_ELEM( *intrinsic, float, 0, 0) = 6.57591187e+002f;	CV_MAT_ELEM( *intrinsic, float, 0, 1) = 0.f;				CV_MAT_ELEM( *intrinsic, float, 0, 2) = 3.16504272e+002f;
	CV_MAT_ELEM( *intrinsic, float, 1, 0) = 0.f;				CV_MAT_ELEM( *intrinsic, float, 1, 1) = 6.60952637e+002f;	CV_MAT_ELEM( *intrinsic, float, 1, 2) = 2.27605789e+002f;
	CV_MAT_ELEM( *intrinsic, float, 2, 0) = 0.f;				CV_MAT_ELEM( *intrinsic, float, 2, 1) = 0.f;				CV_MAT_ELEM( *intrinsic, float, 2, 2) = 1.f;

	if(distortion != NULL)
	{
		cvReleaseMat(&distortion);
	}
	distortion = cvCreateMat(4,1,CV_32FC1);
	CV_MAT_ELEM( *distortion, float, 0, 0) = -1.49060376e-002f; 
	CV_MAT_ELEM( *distortion, float, 1, 0) = 2.05916256e-001f; 
	CV_MAT_ELEM( *distortion, float, 2, 0) = -5.76808210e-003f;
	CV_MAT_ELEM( *distortion, float, 3, 0) = -8.43471102e-003f; 

	cvSave(M_PATH_INTRINSIC,intrinsic);
	cvSave(M_PATH_DISTORTION,distortion);
}

float Globals::GetX(int coord)
{
	return (float)(coord - Globals::calib_x_min) / (float)(Globals::calib_x_max - Globals::calib_x_min);
}

float Globals::GetY(int coord)
{
	return (float)(coord - Globals::calib_y_min) / (float)(Globals::calib_y_max - Globals::calib_y_min);
}
/*
void Globals::UpdateZValues(float z)
{
	if(z_min == -1 || z_max == -1)
	{
		z_min = z;
		z_max = z;
	}
	if(z_min > z) z_min = z;
	if(z_max < z) z_max = z;
	//guardar al xml
}

float Globals::GetZValue(float z)
{
	float newz= z/(float)(Globals::width*Globals::height);
//	std::cout << newz << std::endl;
	return newz;
}

void Globals::ResetZValues()
{
	z_min = -1;
	z_max = -1;
}*/

void Globals::Font::InitFont()
{
	cvInitFont (&font_info, CV_FONT_HERSHEY_SIMPLEX , hscale, vscale, italicscale, thickness, CV_AA);
	cvInitFont (&font_axis, CV_FONT_HERSHEY_SIMPLEX , axis_hscale, axis_vscale, italicscale, thickness, CV_AA);
}

void Globals::Font::Write(IplImage* dest,const char* text, const CvPoint &position, int font_type, short r, short g, short b)
{
	if(dest != NULL && text != NULL)
	{
		switch(font_type)
		{
			case FONT_AXIS:
				cvPutText(dest, text, position, &font_axis,CV_RGB(r,g,b));
			break;
			default:
			case FONT_HELP:
				cvPutText(dest, text, position, &font_info,CV_RGB(r,g,b));
			break;
		}
	}
}