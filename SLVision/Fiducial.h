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

float vect_point_dist(float ax, float ay, float bx, float by, float cx, float cy);
float ivect_point_dist(int ax, int ay, int bx, int by, int cx, int cy);
float fnsqdist(float x, float y, float a, float b);
float insqdist(int x, int y, int a, int b);
float nsqdist(const CvPoint &a, const CvPoint &b);
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
	void SetSize(int size);
	void CalculateIntrinsics();
	void Update(float x, float y,cv::Point2f a,cv::Point2f b,cv::Point2f c,cv::Point2f d, float area, int orientation);
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

