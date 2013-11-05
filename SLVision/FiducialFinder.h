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
typedef std::map<unsigned long, std::pair<int,int>> IDMap;
typedef std::pair<int,int> node;
typedef std::vector<std::pair<int,int>> vector_nodes;


class FiducialFinder
{
	int				fiducial_window_size;
	intList			fiducial_nodes;
	int				nodecount;
	IDMap			idmap;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	vector_nodes GetLevel (int level, int id, std::vector<cv::Vec4i>& hierarchy);

public:
	FiducialFinder(int _fiducial_window_size);
	~FiducialFinder();
	int DecodeFiducial(cv::Mat& src, Fiducial & candidate);
protected:
	unsigned int BinaryListToInt(const intList &data);
	unsigned int StringBinaryListToInt(const char* data, int& size);
	float GetMinDistToFiducialAxis(int &axis, float x, float y);
	int GetId(unsigned int candidate);
	int GetSize(unsigned int candidate);
	void InitFID();
	void LoadFiducialList ();
	void RepportOSC();
};
