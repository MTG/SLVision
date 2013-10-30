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

