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
#include "frameprocessor.h"
#include "FiducialFinder.h"
#define DRAW_POSE

typedef std::map<unsigned int, Fiducial*> FiducialMap;

class candidate
{
public:
	std::vector<cv::Point2f> points;
	float area, x, y;
#ifdef USE_EIGHT_POINTS
	std::vector<cv::Point2f> interior_points;

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
	void RepportOSC();
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
	/******************************************************
	* Hierarchy structure
	* -------------------
	*
	*  hierarchy[id][0]=next; 
	*  hierarchy[id][1]=previous; 
	*  hierarchy[id][2]=first_child; 
	*  hierarchy[id][3]=parent;
	*
	*  if hierarchy value == -1 ---> not found  else gets  the contour index
	*
	*******************************************************/
	std::vector<cv::Vec4i>					hierarchy;
	cv::vector<cv::Point>					approxCurve;
	cv::Mat									fiducial_image,fiducial_image_zoomed;
	cv::Point2f								dst_pnt[4], tmp_pnt[4];
	cv::Mat									mapmatrix;
	std::vector<unsigned int>				to_remove;
	bool									is_fiducial_display_shown;

	void Process(cv::Mat&	main_image);
	void BuildGui(bool force = false);
	
	void InitGeometry();
	int perimeter ( std::vector<cv::Point2f> &a );
	void SquareDetector(std::vector<candidate>& MarkerCanditates, std::vector<candidate>& dest);
	bool IsSquare(int index);
};
