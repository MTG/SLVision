/*
 * Copyright (C) 2011-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of SLVision
 *
 * SLVision is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/*
 *	Daniel Gallardo Grassot
 *	daniel.gallardo@upf.edu
 *	Barcelona 2011
 */

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

//CAMERA constants
#define CAMERA_ID					0

#define APP_NAME					"SLVision"

#define KEY_EXIT					27 //esc

class Globals
{
public:
	/************************
	* Camera parameters
	*************************/
	static cv::Mat CameraMatrix; // 3x3 matrix (fx 0 cx, 0 fy cy, 0 0 1)
	static cv::Mat Distortion; //4x1 matrix (k1,k2,p1,p2)
    static cv::Size CamSize; //size of the image
	static cv::Mat CameraFrame;

	/************************
	* Screen Globals
	*************************/
	static bool				is_view_enabled;	//flag window image enabled or disabled
	static int				width;
	static int				height;
	static unsigned int		ssidGenerator;
	static std::string		dim;

	static void LoadDefaultDistortionMatrix();

};

