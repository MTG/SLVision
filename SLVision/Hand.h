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
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>

#define HAND_CENTROID_SIMILARITY 100
class Hand
{
protected:
	unsigned long sessionID;
	
	
	cv::Point centroid;
	cv::Point fingers[5];


	cv::Point center_hand;  //center of the hand (excluding the arm)
	float influence_radius; //hand radius (excluding the arm)

	cv::Point startarm;  // point where the arm starts (use to be a point on a side of the table)
	cv::Point endarm; //point where the hand ends
	
	
	bool is_open;
	bool is_confirmed;
	bool is_hand;
	bool is_on_the_surface;

	cv::vector<cv::Point> blobPath;
	cv::vector<int> hull;
	std::vector<cv::Vec4i> defects;

	//auxiliar data
	int edge_index;
	int further_index;
	float auxiliar_distance;
	float temp_dist;
	std::vector<int> finger_candidates;
public:

	//Functions
	Hand(void);
	Hand(unsigned long _sessionID, const cv::Point & _centroid);
	bool IsItTheSame( cv::Point &point );
	void UpdateData( cv::Point &point, cv::vector<cv::Point> &path );
	unsigned long GetSID();
	bool IsValid();

	void Draw(bool force = false);
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

