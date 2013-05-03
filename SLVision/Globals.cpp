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
#include <string>


//IplImage*		Globals::screen						= NULL;
bool			Globals::is_view_enabled			= false;

int				Globals::width						= 0;
int				Globals::height						= 0;
//char			Globals::dim[100];

unsigned int	Globals::ssidGenerator				= 1;

//HandFinder*		Globals::hand_finder				= NULL;


cv::Mat Globals::CameraMatrix; // 3x3 matrix (fx 0 cx, 0 fy cy, 0 0 1)
cv::Mat Globals::Distortion; //4x1 matrix (k1,k2,p1,p2)
cv::Size Globals::CamSize; //size of the image

void Globals::LoadDefaultDistortionMatrix()
{
	std::string filePath("camera_params.yml");
	cv::FileStorage fs(filePath, cv::FileStorage::READ);
	std::cout << "Reading " << filePath << std::endl;
	bool success = false;
	int w=-1,h=-1;
    cv::Mat MCamera,MDist;
	if(fs.isOpened())
	{
		fs["image_width"] >> w;
		fs["image_height"] >> h;
		fs["distortion_coefficients"] >> MDist;
		fs["camera_matrix"] >> MCamera;
		
		if (MCamera.cols==0 || MCamera.rows==0)
		{
			std::cout << "File :"+filePath+" does not contains valid camera matrix  ----  CameraParameters::readFromXML" << std::endl;
		}
		if (w==-1 || h==0)
		{
			std::cout << "File :"+filePath+" does not contains valid camera dimensions ---- CameraParameters::readFromXML"<< std::endl;
		}

		if (MCamera.type()!=CV_32FC1) MCamera.convertTo(CameraMatrix,CV_32FC1);
		else CameraMatrix=MCamera;

		if (MDist.total()<4)
		{
			std::cout << "File :"+filePath+" does not contains valid distortion_coefficients ---- CameraParameters::readFromXML" << std::endl;
		}
		//convert to 32 and get the 4 first elements only
		cv::Mat mdist32;
		MDist.convertTo(mdist32,CV_32FC1);
		Distortion.create(1,4,CV_32FC1);
		for (int i=0;i<4;i++)
			Distortion.ptr<float>(0)[i]=mdist32.ptr<float>(0)[i];

		CamSize.width=w;
		CamSize.height=h;
		success = true;
	}
	if( ! success )
	{
		std::cout << "Error!!!\nLoading default values, the 3d data could be distortionated and unprecise" << std::endl;
		CameraMatrix.create(3,3,CV_32FC1);
		Distortion.create(1,4,CV_32FC1);
		CameraMatrix.ptr<float>(0)[0] = 6.57591187e+002f;		CameraMatrix.ptr<float>(0)[1] = 0.f;					CameraMatrix.ptr<float>(0)[2] = 3.16504272e+002f;
		CameraMatrix.ptr<float>(1)[0] = 0.f;					CameraMatrix.ptr<float>(1)[1] = 6.60952637e+002f;		CameraMatrix.ptr<float>(1)[2] = 2.27605789e+002f;
		CameraMatrix.ptr<float>(2)[0] = 0.f;					CameraMatrix.ptr<float>(2)[1] = 0.f;					CameraMatrix.ptr<float>(2)[2] = 1.f;

		Distortion.ptr<float>(0)[0]= -1.49060376e-002f;
		Distortion.ptr<float>(0)[1]= 2.05916256e-001f; 
		Distortion.ptr<float>(0)[2]= -5.76808210e-003f;
		Distortion.ptr<float>(0)[3]= -8.43471102e-003f;
		
		CamSize.width = 640;
		CamSize.height = 480;
	}
}

/*float Globals::GetX(int coord)
{
	//return 1.0f-(float)(coord - Globals::calib_x_min) / (float)(Globals::calib_x_max - Globals::calib_x_min);
	return coord;
}

float Globals::GetY(int coord)
{
	//return (float)(coord - Globals::calib_y_min) / (float)(Globals::calib_y_max - Globals::calib_y_min);
	return coord;
}

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
}*/