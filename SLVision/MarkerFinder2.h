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
#include "FiducialFinder2.h"

#define FIDUCIAL_IMAGE_SIZE 70

typedef std::map<unsigned int, Fiducial*> FiducialMap;

class MarkerFinder2 :
	public FrameProcessor
{
	//FIDUCIAL IMAGE Data
	IplImage*		fiducial_image;
	IplImage*		fiducial_image_zoomed;
	//MAIN IMAGE Data
	IplImage*		main_processed_image;
	IplImage*		main_processed_contour;
	CvMemStorage*	main_storage;
	CvMemStorage*	main_storage_poligon;
	CvMoments*		blob_moments;

	CvSeq*			firstcontour;
	CvSeq*			polycontour;
	//
	FiducialFinder2* fiducial_finder;

	//unsigned int ssidGenerator;
	FiducialMap		fiducial_map;
	
	unsigned int	tmp_ssid;
	bool			to_process;
	float			minimum_distance;
	std::vector<unsigned int> to_remove;
	Fiducial		temporal;

	//
	CvMat*			map_matrix;
	CvPoint2D32f src_pnt[4], dst_pnt[4], tmp_pnt[4];

	int i, j, k;
	CvMat object_points;
	CvMat image_points;
	CvMat point_counts;
	CvMat *rotation;
	CvMat *translation;
	CvMat *srcPoints3D;
	CvMat *dstPoints2D;
	CvPoint3D32f baseMarkerPoints[4];

	char text[100];

	CvMat *rotationMatrix;

	int threshold_value;
	bool use_adaptive_threshold;
	int adaptive_block_size;
	bool invert_rotation_matrix;

public:
	MarkerFinder2(void);
	~MarkerFinder2(void);
	AliveList GetAlive();
	void KeyInput(char key);
protected:
	void InitGeometry();
	void InitFrames(IplImage*	main_image);
	void UpdatedValuesFromGui();
	IplImage* Process(IplImage*	main_image);
	void RepportOSC();
};


