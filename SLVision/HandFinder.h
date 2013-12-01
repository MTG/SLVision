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
#include "Hand.h"
#include <map>

class HandFinder :
	public FrameProcessor
{
	cv::Mat									grey, thres, thres_contours;
	int &									Threshold_value;
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
	int										& min_area;
//	IplImage*		main_processed_image;
//	IplImage*		main_processed_contour;
//	CvMemStorage*	main_storage;
//	CvMemStorage*	main_storage_poligon;
//	CvMoments*		blob_moments;
//	CvSeq*			firstcontour;
//	CvSeq*			polycontour;
//	CvPoint			hand_centroid;
	std::map<unsigned long, Hand> hands;
	std::vector<unsigned long> to_be_removed;
//	std::vector<unsigned long> to_remove;
//
//	int				threshold_value;
//	int				min_pinch_blob_size;
//	float area;
//	float length;
//	int max_area;
//	int min_area;
//
public:
//	static HandFinder* instance;
	HandFinder(void);
	~HandFinder(void);
//
	AliveList GetAlive();
//	bool TouchInHand(float x, float y);
//	std::map<unsigned long, Hand*>* GetHands();
protected:
//	void KeyInput(char key);
//	void UpdatedValuesFromGui(); 
	void Process(cv::Mat&	main_image);
	void BuildGui(bool force = false);
	void RepportOSC();
};

