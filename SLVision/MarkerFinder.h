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
#include "frameprocessor.h"
#include "FiducialFinder.h"
#define DRAW_POSE

typedef std::map<unsigned int, Fiducial*> FiducialMap;

class candidate
{
public:
#ifdef USE_EIGHT_POINTS
	std::vector<cv::Point2f> interior_points;
#endif
	std::vector<cv::Point2f> points;

	float area, x, y;
#ifdef USE_EIGHT_POINTS
	candidate( float _area, float _x, float _y, std::vector<cv::Point2f>& _points, std::vector<cv::Point2f>& _interior_points):
		interior_points(std::vector<cv::Point2f>(_interior_points)),
		points(std::vector<cv::Point2f>(_points)),
		area(_area),
		x(_x),
		y(_y)
	{
	}
#else
	candidate( float _area, float _x, float _y, std::vector<cv::Point2f>& _points):
		points(std::vector<cv::Point2f>(_points)),
		area(_area),
		x(_x),
		y(_y)
	{
	}
#endif
};

class MarkerFinder: public FrameProcessor
{
public:
	MarkerFinder(void);
	~MarkerFinder(void);
	AliveList GetAlive();
protected:
	cv::Mat									grey, thres, thres2;
	int										&use_adaptive_bar_value;
	bool									use_adaptive_threshold;
	int										& block_size;
	int										& threshold_C;
	int										& Threshold_value;
	FiducialMap								fiducial_map;
	FiducialFinder							finder;
	std::vector<std::vector<cv::Point> >	contours;
	//hierarchy[id][0]=next; hierarchy[id][0]=previous; hierarchy[id][0]=first_child; hierarchy[id][0]=parent;  
	//if hierarchy == -1 ---> not found  else gets  the contour index
	std::vector<cv::Vec4i>					hierarchy;
	cv::vector<cv::Point>					approxCurve;
	cv::Mat									fiducial_image,fiducial_image_zoomed;
	cv::Point2f								dst_pnt[4], tmp_pnt[4];
	cv::Mat									mapmatrix;
	std::vector<unsigned int>				to_remove;
	bool									is_fiducial_display_shown;

	void Process(cv::Mat&	main_image);
	void BuildGui(bool force = false);
	void RepportOSC();
	void InitGeometry();
	int perimeter ( std::vector<cv::Point2f> &a );
	void SquareDetector(std::vector<candidate>& MarkerCanditates, std::vector<candidate>& dest);
	bool IsSquare(int index);
};