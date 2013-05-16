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

#include "MarkerFinder.h"
#include "GlobalConfig.h"
#include "Globals.h"
#include "TuioServer.h"

MarkerFinder::MarkerFinder():
	FrameProcessor("MarkerFinder"),
	use_adaptive_bar_value(datasaver::GlobalConfig::getRef("FrameProcessor:MarkerFinder:adaptive_threshold:enable",1)),
	block_size (datasaver::GlobalConfig::getRef("FrameProcessor:MarkerFinder:adaptive_threshold:blocksize",47)),
	threshold_C (datasaver::GlobalConfig::getRef("FrameProcessor:MarkerFinder:adaptive_threshold:threshold_C",2)),
	Threshold_value (datasaver::GlobalConfig::getRef("FrameProcessor:MarkerFinder:threshold:threshold_value",33)),
	finder (FiducialFinder(FIDUCIAL_WIN_SIZE))
{
	//init values
	if(use_adaptive_bar_value == 0) use_adaptive_threshold = false;
	else use_adaptive_threshold = true;

	if(enable_processor == 1) Enable(true);
	else Enable(false);

	is_fiducial_display_shown = false;

	//create gui
	BuildGui();

	InitGeometry();
}

void MarkerFinder::InitGeometry()
{
	fiducial_image = cv::Mat(FIDUCIAL_WIN_SIZE,FIDUCIAL_WIN_SIZE,CV_8UC1);
	dst_pnt[2] = cvPoint2D32f (0, FIDUCIAL_WIN_SIZE);
	dst_pnt[3] = cvPoint2D32f (0, 0);
	dst_pnt[0] = cvPoint2D32f (FIDUCIAL_WIN_SIZE, 0);
    dst_pnt[1] = cvPoint2D32f (FIDUCIAL_WIN_SIZE, FIDUCIAL_WIN_SIZE);
}

MarkerFinder::~MarkerFinder(void)
{

}

AliveList MarkerFinder::GetAlive()
{
	AliveList to_return;
	if(IsEnabled())
	{
		for(FiducialMap::iterator it = fiducial_map.begin(); it!= fiducial_map.end(); it++)
		{
			to_return.push_back(it->first);
			//std::cout << " " << it->first;
		}
		//std::cout << std::endl;
	}
	return to_return;
}

bool MarkerFinder::IsSquare(int index)
{
	approxPolyDP (  contours[index]  ,approxCurve , double ( contours[index].size() ) *0.05, true );

	if ( approxCurve.size() ==4  && cv::isContourConvex ( cv::Mat ( approxCurve ) ) )
	{
		//ensure that the   distace between consecutive points is large enough
		float minDist=1e10;
		for ( int j=0;j<4;j++ )
		{
			float d= std::sqrt ( ( float ) ( approxCurve[j].x-approxCurve[ ( j+1 ) %4].x ) * ( approxCurve[j].x-approxCurve[ ( j+1 ) %4].x ) +
					 ( approxCurve[j].y-approxCurve[ ( j+1 ) %4].y ) * ( approxCurve[j].y-approxCurve[ ( j+1 ) %4].y ) );
			if ( d<minDist ) 
				minDist=d;
		}
		//check that distance is not very small
		if ( minDist>10 )
		{
			return true;
		}
	}
	return false;
}


void MarkerFinder::Process(cv::Mat&	main_image)
{
	/******************************************************
	* Redraw GUI if needed
	*******************************************************/
	if(use_adaptive_bar_value == 0 && use_adaptive_threshold == true)
	{
		use_adaptive_threshold = false;
		BuildGui(true);
	}
	else if (use_adaptive_bar_value == 1 && use_adaptive_threshold == false)
	{
		use_adaptive_threshold = true;
		BuildGui(true);
	}
	/******************************************************
	* Convert image to graycsale
	*******************************************************/
	if ( main_image.type() ==CV_8UC3 )   cv::cvtColor ( main_image,grey,CV_BGR2GRAY );
	else     grey=main_image;
	/******************************************************
	* Apply threshold
	*******************************************************/
	if(use_adaptive_threshold)
	{
		if (block_size<3) block_size=3;
		if (block_size%2!=1) block_size++;
		cv::adaptiveThreshold ( grey,thres,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,block_size,threshold_C );
	}
	else 
		cv::threshold(grey,thres,Threshold_value,255,cv::THRESH_BINARY_INV);
	/******************************************************
	* Find contours
	*******************************************************/
	thres2 = thres.clone();
	float _minSize=0.04f;
	float _maxSize=0.5f;
	unsigned int minSize=(unsigned int)(_minSize*std::max(thres.cols,thres.rows)*4);
	unsigned int maxSize=(unsigned int)(_maxSize*std::max(thres.cols,thres.rows)*4);
	cv::findContours ( thres2 , contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE );
//	cv::vector<cv::Point>  approxCurve;
	/******************************************************
	* Find Squares
	*******************************************************/
	std::vector<candidate> SquareCanditates;
	int idx = 0;
    for( ; idx >= 0; idx = hierarchy[idx][0] )
    {
		//80's effect :)
		//if ( minSize< contours[idx].size() &&contours[idx].size()<maxSize  )
		//{
        //cv::Scalar color( rand()&255, rand()&255, rand()&255 );
        //drawContours( Globals::CameraFrame, contours, idx, color, CV_FILLED, 8, hierarchy );
		//}
		if ( minSize< contours[idx].size() && contours[idx].size()< maxSize  )
		{
			if( IsSquare(idx) )
			{
				//calculates the area
				float area = (float)cv::contourArea(contours[idx]);
				//calculates the centroid
				cv::Moments fiducial_blob_moments = cv::moments(contours[idx],true);
				float x = (float)(fiducial_blob_moments.m10 / fiducial_blob_moments.m00);
				float y = (float)(fiducial_blob_moments.m01 / fiducial_blob_moments.m00);
#ifndef USE_EIGHT_POINTS
				//add the points
				SquareCanditates.push_back ( candidate(area,x,y,std::vector<cv::Point2f>()) );
				//MarkerCanditates.back().idx=i;
				for ( unsigned int j=0;j<4;j++ )
				{
					SquareCanditates.back().points.push_back ( cv::Point2f ( (float)approxCurve[j].x,(float)approxCurve[j].y ) );
				}
#else
				std::vector<cv::Point2f> points;
				for ( int j=0;j<4;j++ )
					points.push_back ( cv::Point2f ( approxCurve[j].x,approxCurve[j].y ) );
				/******************************************************
				* Search for the internal square
				*******************************************************/
				bool found = false;
				if(hierarchy[idx][2] != -1)
				{
					int idx2 = hierarchy[idx][2];
					cv::Scalar color( 0,0,255 );
					if(IsSquare(idx2))
					{
						found = true;
						//drawContours( Globals::CameraFrame, contours, idx2, color, 1, 8, hierarchy );
					}
					while (hierarchy[idx2][0] != -1 && ! found)
					{
						idx2 = hierarchy[idx2][0];
						if(IsSquare(idx2))
						{
							//drawContours( Globals::CameraFrame, contours, idx2, color, 1, 8, hierarchy );
							found = true;
						}
					}
				}
				std::vector<cv::Point2f> interior_points;
				if(found)
				{
					for ( int j=0;j<4;j++ )
						interior_points.push_back ( cv::Point2f ( approxCurve[j].x,approxCurve[j].y ) );
				}
				/******************************************************
				* fill candidates list
				*******************************************************/
				SquareCanditates.push_back ( candidate(area,x,y,points,interior_points) );
#endif
			}
		}
	}
	/******************************************************
	* Find Markers
	*******************************************************/
	std::vector<candidate> Squares;
	SquareDetector(SquareCanditates, Squares);
	for (size_t i=0;i<Squares.size();i++) 
	{
		
		///Marker identification
		for(int k = 0; k < 4; k++)
			tmp_pnt[k] = cv::Point2f(Squares[i].points[k]);
		//Zoom the square to detect the fiducial
		mapmatrix = cv::getPerspectiveTransform(tmp_pnt,dst_pnt);
		cv::warpPerspective(thres,fiducial_image,mapmatrix,cv::Size(FIDUCIAL_WIN_SIZE,FIDUCIAL_WIN_SIZE));
		cv::resize(fiducial_image, fiducial_image_zoomed, cv::Size(FIDUCIAL_WIN_SIZE*2,FIDUCIAL_WIN_SIZE*2));
		/******************************************************
		* Decode Markers
		*******************************************************/
		Fiducial temp = Fiducial();
#ifdef USE_EIGHT_POINTS
		if(Squares[i].interior_points.size() == 4)
		{
			temp.Update(Squares[i].x,
						Squares[i].y,
						Squares[i].points[0],
						Squares[i].points[1],
						Squares[i].points[2],
						Squares[i].points[3],
						Squares[i].interior_points[0],
						Squares[i].interior_points[1],
						Squares[i].interior_points[2],
						Squares[i].interior_points[3],
						Squares[i].area,
						0);
		}
		else
#endif
		temp.Update(Squares[i].x,Squares[i].y,Squares[i].points[0],Squares[i].points[1],Squares[i].points[2],Squares[i].points[3],Squares[i].area,0);
		/*************************************
		* Update Fiducial List
		**************************************/
		float minimum_distance = 99999999.0f;
		int tmp_ssid = 0;
		for(FiducialMap::iterator it = fiducial_map.begin(); it!= fiducial_map.end(); it++)
		{
			if( it->second->CanUpdate(temp,minimum_distance) )
			{
				tmp_ssid = it->first;
			}
		}
		if(tmp_ssid != 0)
		{
			temp = Fiducial(*fiducial_map[tmp_ssid]);
			temp.Update(Squares[i].x,Squares[i].y,Squares[i].points[0],Squares[i].points[1],Squares[i].points[2],Squares[i].points[3],Squares[i].area,0);
		}
		/*************************************
		* Decode Fiducial and pose Estimation
		**************************************/
		int tmp_id = finder.DecodeFiducial(fiducial_image, temp);
		if(tmp_id >= 0)
		{
			/*************************************
			* Update Fiducial List
			**************************************/
			if(tmp_ssid == 0)
			{
				if(temp.GetFiducialID() == -1) break;
				//add fiducial
				fiducial_map[Globals::ssidGenerator++] = new Fiducial(temp);
				tmp_ssid = Globals::ssidGenerator-1;
			}
			else if( tmp_ssid > 0)
			{
				fiducial_map[tmp_ssid]->Update(temp);
			}

			if(Globals::is_view_enabled)
			{
				//temp = Fiducial(*fiducial_map[tmp_ssid]);
				/******************************************************
				* Draw FiducialID and SSID
				*******************************************************/
				std::stringstream s;
				s << "FID: " << tmp_id << " SID: " << tmp_ssid;
				cv::putText(Globals::CameraFrame, s.str(), cv::Point2f(temp.GetX()+20,temp.GetY()), cv::FONT_HERSHEY_DUPLEX, 0.5f, cv::Scalar(255,0,255));
				/******************************************************
				* Draw Pose
				*******************************************************/
#ifdef DRAW_POSE
				cv::Mat objectPoints (8,3,CV_32FC1);
				cv::vector<cv::Point2f> imagePoints;
				cv::Mat Tvec = temp.GetTranslationVector();
				cv::Mat Rvec = temp.GetRotationVector();
#ifdef DRAW_POSE_COMPLETE
				/******************************************************
				* Draw Square
				*******************************************************/
				cv::putText(Globals::CameraFrame, "a", temp.GetCorner(0), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,255,0));
				cv::putText(Globals::CameraFrame, "b", temp.GetCorner(1), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,255,0));
				cv::putText(Globals::CameraFrame, "c", temp.GetCorner(2), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,255,0));
				cv::putText(Globals::CameraFrame, "d", temp.GetCorner(3), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,255,0));
				///show perimeter
				for(int k = 0; k < 4; k++)
				{
					if (k!= 3)
						cv::line(Globals::CameraFrame,Squares[i].points[k],Squares[i].points[k+1],cv::Scalar(0,0,255,255),1,CV_AA);
					else
						cv::line(Globals::CameraFrame,Squares[i].points[k],Squares[i].points[0],cv::Scalar(0,0,255,255),1,CV_AA);
//					tmp_pnt[k] = cv::Point2f(Squares[i].points[k]);
				}
#ifdef USE_EIGHT_POINTS
				if(temp.HasInnerCorners())
				{
					cv::putText(Globals::CameraFrame, "e", temp.GetCorner(4), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,255,0));
					cv::putText(Globals::CameraFrame, "f", temp.GetCorner(5), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,255,0));
					cv::putText(Globals::CameraFrame, "g", temp.GetCorner(6), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,255,0));
					cv::putText(Globals::CameraFrame, "h", temp.GetCorner(7), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,255,0));
					///show perimeter
					for(int k = 0; k < 4; k++)
					{
						if (k!= 3)
							cv::line(Globals::CameraFrame,Squares[i].points[k],Squares[i].points[k+1],cv::Scalar(0,0,255,255),1,CV_AA);
						else
							cv::line(Globals::CameraFrame,Squares[i].points[k],Squares[i].points[0],cv::Scalar(0,0,255,255),1,CV_AA);
					}
				}
#endif
				/*************************************
				* Draw pose params  cube
				**************************************/
				objectPoints.at<float>(0,0)=0;
				objectPoints.at<float>(0,1)=temp.GetSize();
				objectPoints.at<float>(0,2)=0;

				objectPoints.at<float>(1,0)=temp.GetSize();
				objectPoints.at<float>(1,1)=temp.GetSize();
				objectPoints.at<float>(1,2)=0;

				objectPoints.at<float>(2,0)=temp.GetSize();
				objectPoints.at<float>(2,1)=0;
				objectPoints.at<float>(2,2)=0;

				objectPoints.at<float>(3,0)=0;
				objectPoints.at<float>(3,1)=0;
				objectPoints.at<float>(3,2)=0;

				objectPoints.at<float>(4,0)=0;
				objectPoints.at<float>(4,1)=temp.GetSize();
				objectPoints.at<float>(4,2)=temp.GetSize();

				objectPoints.at<float>(5,0)=temp.GetSize();
				objectPoints.at<float>(5,1)=temp.GetSize();
				objectPoints.at<float>(5,2)=temp.GetSize();

				objectPoints.at<float>(6,0)=temp.GetSize();
				objectPoints.at<float>(6,1)=0;
				objectPoints.at<float>(6,2)=temp.GetSize();

				objectPoints.at<float>(7,0)=0;
				objectPoints.at<float>(7,1)=0;
				objectPoints.at<float>(7,2)=temp.GetSize();

				if(!(Tvec.rows == 0 || Tvec.cols == 0 || Rvec.rows == 0 || Rvec.cols == 0))
				{
					cv::projectPoints(objectPoints,temp.GetRotationVector(), temp.GetTranslationVector(),Globals::CameraMatrix,Globals::Distortion,imagePoints);
			
					//draw lines of different colours
					for (int i=0;i<4;i++)
						cv::line(Globals::CameraFrame,imagePoints[i],imagePoints[(i+1)%4],cv::Scalar(0,255,255,255),1,CV_AA);

					for (int i=0;i<4;i++)
						cv::line(Globals::CameraFrame,imagePoints[i+4],imagePoints[4+(i+1)%4],cv::Scalar(255,0,255,255),1,CV_AA);

					for (int i=0;i<4;i++)
						cv::line(Globals::CameraFrame,imagePoints[i],imagePoints[i+4],cv::Scalar(255,255,0,255),1,CV_AA);
				}
#endif
				/*************************************
				* Draw pose params  axis
				**************************************/
				float size=temp.GetSize()*3;
				float halfSize=temp.GetSize()/2;
				objectPoints= cv::Mat(4,3,CV_32FC1);
				objectPoints.at<float>(0,0)=halfSize;
				objectPoints.at<float>(0,1)=halfSize;
				objectPoints.at<float>(0,2)=0;
				objectPoints.at<float>(1,0)=size;
				objectPoints.at<float>(1,1)=halfSize;
				objectPoints.at<float>(1,2)=0;
				objectPoints.at<float>(2,0)=halfSize;
				objectPoints.at<float>(2,1)=size;
				objectPoints.at<float>(2,2)=0;
				objectPoints.at<float>(3,0)=halfSize;
				objectPoints.at<float>(3,1)=halfSize;
				objectPoints.at<float>(3,2)=size;
				imagePoints.clear();
				cv::projectPoints( objectPoints, Rvec,Tvec, Globals::CameraMatrix,Globals::Distortion, imagePoints);
				//draw lines of different colours
				cv::line(Globals::CameraFrame,imagePoints[0],imagePoints[1],cv::Scalar(0,0,255,255),2,CV_AA);
				cv::putText(Globals::CameraFrame, "X", imagePoints[1], cv::FONT_HERSHEY_SIMPLEX, 1.0f, cv::Scalar(0,0,255));
				cv::line(Globals::CameraFrame,imagePoints[0],imagePoints[2],cv::Scalar(0,255,0,255),2,CV_AA);
				cv::putText(Globals::CameraFrame, "Y", imagePoints[2], cv::FONT_HERSHEY_SIMPLEX, 1.0f, cv::Scalar(0,255,0));
				cv::line(Globals::CameraFrame,imagePoints[0],imagePoints[3],cv::Scalar(255,0,0,255),2,CV_AA);
				cv::putText(Globals::CameraFrame, "Z", imagePoints[3], cv::FONT_HERSHEY_SIMPLEX, 1.0f, cv::Scalar(255,0,0));
#endif //DRAWPOSE
			}
		}
	}

	/******************************************************
	* Show thresholded Image
	*******************************************************/
	if (this->show_screen)
	{
		cv::imshow("fidfinder",fiducial_image_zoomed);
		cv::imshow(this->name,thres);
		is_fiducial_display_shown = true;
	}
	else if(is_fiducial_display_shown)
	{
		cv::destroyWindow("fidfinder");
		is_fiducial_display_shown = false;
	}
}

void MarkerFinder::RepportOSC()
{
	if(!this->IsEnabled())return;
	to_remove.clear();
	for(FiducialMap::iterator it = fiducial_map.begin(); it!= fiducial_map.end(); it++)
	{
		if(it->second->IsUpdated())
		{
			//send tuio message!!!!
			TuioServer::Instance().Add3DObjectMessage(
				it->first,
				0,
				it->second->GetFiducialID(),
				it->second->GetX()/Globals::width,//Globals::GetX(it->second->xpos),//it->second->xpos,
				it->second->GetY()/Globals::height,//Globals::GetY(it->second->ypos),//it->second->ypos,
				it->second->zpos,//Globals::GetZValue(it->second->zpos),
				it->second->yaw,
				it->second->pitch,
				it->second->roll,
				it->second->r11,
				it->second->r12,
				it->second->r13,
				it->second->r21,
				it->second->r22,
				it->second->r23,
				it->second->r31,
				it->second->r32,
				it->second->r33
				);
		}
		else
		{
			to_remove.push_back(it->first);
		}
	}

	for(std::vector<unsigned int>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
	{
		fiducial_map.erase(*it);
	}
}


void MarkerFinder::SquareDetector(std::vector<candidate>& MarkerCanditates,std::vector<candidate>& dest)
{
	dest.clear();
	///sort the points in anti-clockwise order
	std::valarray<bool> swapped(false,MarkerCanditates.size());//used later
	for ( unsigned int i=0;i<MarkerCanditates.size();i++ )
	{
		//trace a line between the first and second point.
		//if the thrid point is at the right side, then the points are anti-clockwise
		double dx1 = MarkerCanditates[i].points[1].x - MarkerCanditates[i].points[0].x;
		double dy1 =  MarkerCanditates[i].points[1].y - MarkerCanditates[i].points[0].y;
		double dx2 = MarkerCanditates[i].points[2].x - MarkerCanditates[i].points[0].x;
		double dy2 = MarkerCanditates[i].points[2].y - MarkerCanditates[i].points[0].y;
		double o = ( dx1*dy2 )- ( dy1*dx2 );
		if ( o  < 0.0 )		 //if the third point is in the left side, then sort in anti-clockwise order
		{
			std::swap ( MarkerCanditates[i].points[1],MarkerCanditates[i].points[3] );
			swapped[i]=true;
			//sort the contour points
	//  	    reverse(MarkerCanditates[i].contour.begin(),MarkerCanditates[i].contour.end());//????
		}
	}
	/// remove these elements whise corners are too close to each other
	//first detect candidates
	std::vector<std::pair<int,int>  > TooNearCandidates;
	for ( unsigned int i=0;i<MarkerCanditates.size();i++ )
	{
		// 	cout<<"Marker i="<<i<<MarkerCanditates[i]<<endl;
		//calculate the average distance of each corner to the nearest corner of the other marker candidate
		for ( unsigned int j=i+1;j<MarkerCanditates.size();j++ )
		{
			float dist=0;
			for ( int c=0;c<4;c++ )
				dist+= sqrt (	( MarkerCanditates[i].points[c].x-MarkerCanditates[j].points[c].x ) * 
								( MarkerCanditates[i].points[c].x-MarkerCanditates[j].points[c].x ) + 
								( MarkerCanditates[i].points[c].y-MarkerCanditates[j].points[c].y ) * 
								( MarkerCanditates[i].points[c].y-MarkerCanditates[j].points[c].y ) );
			dist/=4;
			//if distance is too small
			if ( dist< 10 )
			{
				TooNearCandidates.push_back ( std::pair<int,int> ( i,j ) );
			}
		}
	}
	//mark for removal the element of  the pair with smaller perimeter
	std::valarray<bool> toRemove ( false,MarkerCanditates.size() );
	for ( unsigned int i=0;i<TooNearCandidates.size();i++ )
	{
		if ( perimeter ( MarkerCanditates[TooNearCandidates[i].first ].points ) > perimeter ( MarkerCanditates[ TooNearCandidates[i].second].points ) )
			toRemove[TooNearCandidates[i].second]=true;
		else toRemove[TooNearCandidates[i].first]=true;
	}
	for (size_t i=0;i<MarkerCanditates.size();i++) 
	{
		if (!toRemove[i]) 
		{
			dest.push_back(MarkerCanditates[i]);
		}
	}
}

int MarkerFinder::perimeter ( std::vector<cv::Point2f> &a )
{
    float sum=0.0f;
    for ( unsigned int i=0;i<a.size();i++ )
    {
        int i2= ( i+1 ) %a.size();
        sum+= sqrt ( ( a[i].x-a[i2].x ) * ( a[i].x-a[i2].x ) + ( a[i].y-a[i2].y ) * ( a[i].y-a[i2].y ) ) ;
    }
    return (int)sum;
}

void MarkerFinder::BuildGui(bool force)
{
	if(force)
	{
		cv::destroyWindow(name);
		cv::namedWindow(name,CV_WINDOW_AUTOSIZE);
	}
	cv::createTrackbar("Enable", name,&enable_processor, 1, NULL);
	cv::createTrackbar("Use Adaptive", name,&use_adaptive_bar_value, 1, NULL);
	if(use_adaptive_bar_value == 1)
	{
		cv::createTrackbar("block_size", name,&block_size, 200, NULL);
		cv::createTrackbar("C", name,&threshold_C, 20, NULL);
	}
	else
	{
		cv::createTrackbar("th.value", name,&Threshold_value, 255, NULL);
	}
}