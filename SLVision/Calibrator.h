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
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define N_BOARDS 3
#define FRAME_STEP 5
#define BOARD_W 9
#define BOARD_H 6

class Calibrator
{
	/*int n_boards;  //Number of snapshots of the chessboard
	int frame_step;   //Frames to be skipped
	int board_w;   //Enclosed corners horizontally on the chessboard
	int board_h;   //Enclosed corners vertically on the chessboard */
	int		board_total;
	CvSize	board_sz;
	CvMat*	image_points;
	CvMat*	object_points;
	CvMat*	point_counts;
//	CvMat*	intrinsic_matrix;
//	CvMat*	distortion_coeffs;
	CvPoint2D32f* corners;
	int corner_count;
	int successes;
	int step, frame;
	bool chessboard_finder;
	CvMat* object_points2;
	CvMat* image_points2;
	CvMat* point_counts2;
	IplImage* mapx;
	IplImage* mapy;
	//int x_box, y_box, w_box, h_box;
	int x_min, x_max, y_min, y_max;
	int selected_side;
public:
	Calibrator(void);
	~Calibrator(void);
	void ProcessFrame(IplImage*	main_image);
	void StartCalibration();
	void EndCalibration();
	void StartChessBoardFinder();
	void ProcessKey(char key);
};

