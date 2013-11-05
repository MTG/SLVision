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

//#pragma once
//#include <cv.h>
//#include <cxcore.h>
//#include <highgui.h>
//
//#define N_BOARDS 3
//#define FRAME_STEP 5
//#define BOARD_W 9
//#define BOARD_H 6
//
//class Calibrator
//{
//	/*int n_boards;  //Number of snapshots of the chessboard
//	int frame_step;   //Frames to be skipped
//	int board_w;   //Enclosed corners horizontally on the chessboard
//	int board_h;   //Enclosed corners vertically on the chessboard */
//	int		board_total;
//	CvSize	board_sz;
//	CvMat*	image_points;
//	CvMat*	object_points;
//	CvMat*	point_counts;
////	CvMat*	intrinsic_matrix;
////	CvMat*	distortion_coeffs;
//	CvPoint2D32f* corners;
//	int corner_count;
//	int successes;
//	int step, frame;
//	bool chessboard_finder;
//	CvMat* object_points2;
//	CvMat* image_points2;
//	CvMat* point_counts2;
//	IplImage* mapx;
//	IplImage* mapy;
//	//int x_box, y_box, w_box, h_box;
//	int selected_side;
//public:
//	Calibrator(void);
//	~Calibrator(void);
//	void ProcessFrame(IplImage*	main_image);
//	void StartCalibration();
//	void EndCalibration();
//	void StartChessBoardFinder();
//	void ProcessKey(char key);
//};

