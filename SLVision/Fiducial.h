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
//#define USE_EIGHT_POINTS
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

float vect_point_dist(float ax, float ay, float bx, float by, float cx, float cy);
float ivect_point_dist(float ax, float ay, float bx, float by, float cx, float cy);
float fnsqdist(float x, float y, float a, float b);
float insqdist(float x, float y, float a, float b);
float nsqdist(const CvPoint &a, const CvPoint &b);
float nsqdist2(const cv::Point &a, const cv::Point &b);
double IsLeft(double ax, double ay, double bx, double by, float px, float py);

class Fiducial
{
	
	float x, y;
	float area;
	float size;
	unsigned int fiducial_id;
	int orientation;
//	static unsigned int id_generator;
	bool is_updated;
	double removed_time;
	cv::Point2f a,b,c,d;
	cv::Mat rotation_vector,translation_vector;
#ifdef USE_EIGHT_POINTS
	cv::Point2f e,f,g,h;
	bool has_inner_corenr;
#endif

public:
	
	//CvPoint ea,eb,ec,ed;
	float yaw,pitch,roll;
	float xpos, ypos, zpos;

	float r11, r12, r13, r21, r22, r23, r31, r32, r33;

	Fiducial(void);
	Fiducial(const Fiducial &copy);
	~Fiducial(void);
	void clear();
	void SetId(unsigned int id);
	void SetSize(float size);
	void CalculateIntrinsics();
	void Update(float x, float y,cv::Point2f a,cv::Point2f b,cv::Point2f c,cv::Point2f d, float area, int orientation);
#ifdef USE_EIGHT_POINTS
	void Update(float x, float y,cv::Point2f a,cv::Point2f b,cv::Point2f c,cv::Point2f d,cv::Point2f e,cv::Point2f f,cv::Point2f g,cv::Point2f h, float area, int orientation);
	bool HasInnerCorners();
#endif
	void Update(const Fiducial &copy);
	bool Is_inside(const Fiducial &f);
	bool CanUpdate(const Fiducial &f, float & minimum_distance);
	int GetOrientation();
	unsigned int GetFiducialID();
	float GetX();
	float GetY();
	float GetSize();
	cv::Mat GetRotationVector();
	cv::Mat GetTranslationVector();
	bool IsUpdated();
	bool CanBeRemoved(double actual_time);
	void RemoveStart(double actual_time);
	void SetOrientation(int o);
	cv::Point2f GetCorner(int corner);
	void OritentateCorners();
	//fiducial id generator
	//static unsigned int GetNewId();
};

