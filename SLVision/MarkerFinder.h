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
#include "FiducialFinder.h"


class candidate
{
public:
	std::vector<cv::Point2f> points;
	double area, x, y;
	candidate( double _area, double _x, double _y, std::vector<cv::Point2f>& _points):
		points(std::vector<cv::Point2f>(_points)),
		area(_area),
		x(_x),
		y(_y)
	{
	}
};

class MarkerFinder: public FrameProcessor
{
public:
	MarkerFinder(void);
	~MarkerFinder(void);
	AliveList GetAlive();
protected:
	cv::Mat grey, thres, thres2;

	int & use_adaptive_bar_value;
	bool use_adaptive_threshold;

	int & block_size;
	int & threshold_C;
	int & Threshold_value;


	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::Mat fiducial_image,fiducial_image_zoomed;
	cv::Point2f dst_pnt[4], tmp_pnt[4];
	cv::Mat mapmatrix;
	//void InitGeometry();
	//void InitFrames(IplImage*	main_image);
	//void UpdatedValuesFromGui();
	void Process(cv::Mat&	main_image);
	void BuildGui(bool force = false);
	//void RepportOSC();

	FiducialFinder finder;

	void InitGeometry();
	int perimeter ( std::vector<cv::Point2f> &a );
	void SquareDetector(std::vector<candidate>& MarkerCanditates, std::vector<candidate>& dest);
};


//#include "frameprocessor.h"
//#include "FiducialFinder.h"
//
//#define FIDUCIAL_IMAGE_SIZE 70
//#ifndef M_PI
//#define M_PI 3.141592f
//#endif
//typedef std::map<unsigned int, Fiducial*> FiducialMap;
//
//#define ENABLEPOSE 1
//#define DRAWPOSE 1
//#define USEEIGHTPOINTS 1
////#define SHOWDEBUG 1
//
//class MarkerFinder :
//	public FrameProcessor
//{
//	cv::Mat Rvec,Tvec;
//
//	//FIDUCIAL IMAGE Data
//	IplImage*		fiducial_image;
//	IplImage*		fiducial_image_zoomed;
//	//MAIN IMAGE Data
//	IplImage*		main_processed_image;
//	IplImage*		main_processed_contour;
//	CvMemStorage*	main_storage;
//	CvMemStorage*	main_storage_poligon;
//	CvMoments*		blob_moments;
//
//	CvSeq*			firstcontour;
//	CvSeq*			polycontour;
//	//
//	FiducialFinder* fiducial_finder;
//
//	//unsigned int ssidGenerator;
//	FiducialMap		fiducial_map;
//	
//	unsigned int	tmp_ssid;
//	bool			to_process;
//	float			minimum_distance;
//	std::vector<unsigned int> to_remove;
//	Fiducial		temporal;
//
//	//
//	CvMat*			map_matrix;
//	CvPoint2D32f dst_pnt[4], tmp_pnt[4];
//#ifdef USEEIGHTPOINTS
//	CvPoint2D32f src_pnt[8];
//#else
//	CvPoint2D32f src_pnt[4];
//#endif
//
//	int i, j, k;
//	CvMat object_points;
//	CvMat image_points;
//	CvMat point_counts;
//	CvMat *rotation;
//	CvMat *translation;
//	CvMat *srcPoints3D;
//	CvMat *dstPoints2D;
//	CvPoint3D32f baseMarkerPoints[4];
//
//	char text[100];
//
//	CvMat *rotationMatrix;
//
//	int threshold_value;
//	bool use_adaptive_threshold;
//	int adaptive_block_size;
//	bool invert_rotation_matrix;
//
//public:
//	MarkerFinder(void);
//	~MarkerFinder(void);
//	AliveList GetAlive();
//	void KeyInput(char key);
//protected:
//	void InitGeometry();
//	void InitFrames(IplImage*	main_image);
//	void UpdatedValuesFromGui();
//	IplImage* Process(IplImage*	main_image);
//	void RepportOSC();
//};
//
