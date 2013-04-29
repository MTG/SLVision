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
#include <list>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "Fiducial.h"

#define MIN_DIST_TO_SIDE	10

#define AXIS_UP				0
#define AXIS_LEFT			1
#define AXIS_RIGHT			2
#define AXIS_DOWN			3
#define FIDUCIAL_WIN_SIZE	70
#define MIN_DIST_TO_SIDE	10
//datatypes
typedef std::list<int>		intList;
typedef std::map<unsigned long, int> IDMap;

class FiducialFinder
{
	int				fiducial_window_size;
	//CvMoments*      fiducial_blob_moments;
	//IplImage*		fiducial_processed_image;
	//CvMemStorage*	fiducial_storage;
	intList			fiducial_nodes;
	int				nodecount;
	IDMap			idmap;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

public:
	FiducialFinder(int _fiducial_window_size);
	~FiducialFinder();
	int DecodeFiducial(cv::Mat& src, Fiducial & candidate);
protected:
	unsigned int BinaryListToInt(const intList &data);
	unsigned int StringBinaryListToInt(const char* data);
	float GetMinDistToFiducialAxis(int &axis, float x, float y);
	int GetId(unsigned int candidate);
	void InitFID();
	void LoadFiducialList ();
	void RepportOSC();
};