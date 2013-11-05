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
#include "Touch.h"

typedef std::map<unsigned int, Touch*> Pointmap;

class TouchFinder :
	public FrameProcessor
{
private:
	Touch temp_touch;
	float temp_minimum_distance, test_distance;
	unsigned int candidate_id;
	std::vector<unsigned int> to_be_removed;
//	char text[100];
protected:
	int										& Threshold_value;
	int										& max_area;
	int										& min_area;
	cv::Mat									grey, thres, thres_contours;
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

//	IplImage*		main_processed_image;
//	IplImage*		main_processed_contour;
//	int				threshold_value;
//	CvMemStorage*	main_storage;
//	CvSeq*			firstcontour;
//	int				max_blob_size;
//	int				min_blob_size;
//	CvMoments*		blob_moments;
	Pointmap		pointmap;
//
//	void KeyInput(char key);
//	void UpdatedValuesFromGui(); 
//	IplImage* Process(IplImage*	main_image);
//	void RepportOSC();
	void Process(cv::Mat&	main_image);
	void BuildGui(bool force = false);
	void RepportOSC();
public:
	TouchFinder(void);
	~TouchFinder(void);
	AliveList GetAlive();
};

