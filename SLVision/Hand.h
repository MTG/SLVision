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
#include <vector>

#define HAND_CENTROID_SIMILARITY 100
class Hand
{
protected:
	unsigned long sessionID;
	cv::Point startarm;
	cv::Point center_hand;
	cv::Point centroid;
	cv::Point fingers[5];
	
	
	bool is_open;
	bool is_confirmed;
	bool is_on_the_surface;

	cv::vector<cv::Point> blobPath;
	cv::vector<int> hull;
	std::vector<cv::Vec4i> defects;
public:

	//Functions
	Hand(void);
	Hand(unsigned long _sessionID, const cv::Point & _centroid);
	bool IsItTheSame( cv::Point &point );
	void UpdateData( cv::Point &point, cv::vector<cv::Point> &path );
	unsigned long GetSID();
	bool IsValid();

	void Draw();
	bool is_updated;
private:
	void Reset();
	float IsNearEdge( cv::Point & p );
//	~Hand(void);
//    float Distance(const CvPoint & _centroid);
//    unsigned long GetSessionID();
//    void Update(const CvPoint & _centroid);
//
//    void Clear();
//    void AddVertex(int x, int y);
//    void AddVertexConvex(int x, int y);
//    void ComputeHand(float area, float length);
//    void draw(float x = 0, float y = 0);
//
//	bool IsUpdated();
//	int IsConfirmedAsHand();
//	int IsOpened();
//	CvPoint GetCentroid();
//	float GetArea();
//	float TCentroidX();
//	float TCentroidY();
//	float GetLwPCentroidX();
//	float GetLwPCentroidY();
//
//	bool IsPinching();
//	bool IsPinchingEnd();
//	void SetPinch(CvSeq* seq);
//
//	bool IsFinger(float x, float y);
};

