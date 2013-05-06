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

