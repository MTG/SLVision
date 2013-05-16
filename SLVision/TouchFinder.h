/*
	Daniel Gallardo Grassot
	daniel.gallardo@upf.edu
	Barcelona 2013

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

