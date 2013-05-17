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

#include "TouchFinder.h"
#include "GlobalConfig.h"
#include "Globals.h"
#include "Fiducial.h"
#include "TuioServer.h"
#define DISTANCE_OFFSET 50
//#include "HandFinder.h"
//
TouchFinder::TouchFinder(void):
	Threshold_value (datasaver::GlobalConfig::getRef("FrameProcessor:TouchFinder:threshold:threshold_value",33)),
	max_area(datasaver::GlobalConfig::getRef("FrameProcessor:TouchFinder:threshold:maximum_touch_area",800)),
	min_area(datasaver::GlobalConfig::getRef("FrameProcessor:TouchFinder:threshold:minimum_tpuch_area",200)),
	FrameProcessor("TouchFinder")
{
	if(enable_processor == 1) Enable(true);
	else Enable(false);

	//create gui
	BuildGui();
}

TouchFinder::~TouchFinder(void)
{
}

///
///TODO: feet the blobcandidate into a ellipse and filter by radius instead filter by area
///
void TouchFinder::Process(cv::Mat&	main_image)
{
	/******************************************************
	* Convert image to graycsale
	*******************************************************/
	//if ( main_image.type() ==CV_8UC3 )   cv::cvtColor ( main_image,grey,CV_BGR2GRAY );
	//else     grey=main_image;
	grey=main_image;
	/******************************************************
	* Apply threshold
	*******************************************************/
	cv::threshold(grey,thres,Threshold_value,255,cv::THRESH_BINARY);
	thres_contours = thres.clone();
	/******************************************************
	* Find contours
	*******************************************************/
	cv::findContours ( thres_contours , contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE );
	int idx = 0;
    /******************************************************
	* Find touch candidates
	*******************************************************/
	for( ; idx >= 0; idx = hierarchy[idx][0] )
    {
		//calculates the area
		float area = (float)cv::contourArea(contours[idx]);
		if(area >= min_area && area <= max_area)
		{
			/******************************************************
			* Retreive relevant data
			*******************************************************/
			drawContours( Globals::CameraFrame, contours, idx, cv::Scalar(0,255,255), CV_FILLED, 8, hierarchy );
			//calculates the centroid
			cv::Moments fiducial_blob_moments = cv::moments(contours[idx],true);
			float x = (float)(fiducial_blob_moments.m10 / fiducial_blob_moments.m00);
			float y = (float)(fiducial_blob_moments.m01 / fiducial_blob_moments.m00);
			temp_touch.Update(	x, y, area);
			temp_minimum_distance = 99999999.0f;
			candidate_id = 0;
			/******************************************************
			* Check for existing touches and retreive the ID
			*******************************************************/
			for(Pointmap::iterator it = pointmap.begin(); it != pointmap.end(); it++)
			{
				float tmp_dist = fabs(fnsqdist(temp_touch.GetX(),temp_touch.GetY(),it->second->GetX(),it->second->GetY()));
				if( temp_minimum_distance > tmp_dist)
				{
					candidate_id = it->first;
					temp_minimum_distance = tmp_dist;
				}
			}
			if(temp_minimum_distance > DISTANCE_OFFSET) candidate_id = 0;
			if(candidate_id == 0) //new touch
			{
				unsigned int new_id = Globals::ssidGenerator++;
				pointmap[new_id] = new Touch(temp_touch);
				candidate_id = new_id;
			}
			else //update touch
			{
				pointmap[candidate_id]->Update(temp_touch);
			}
			if(Globals::is_view_enabled)
			{
				/******************************************************
				* Draw contour and data
				*******************************************************/
				drawContours( Globals::CameraFrame, contours, idx, cv::Scalar(0,255,255), CV_FILLED, 8, hierarchy );
#ifdef DRAW_EVERYTHING
				std::stringstream s;
				s << candidate_id << "  (" <<pointmap[candidate_id]->GetX() << ", " << pointmap[candidate_id]->GetY() <<")";
				cv::putText(Globals::CameraFrame, s.str(), cv::Point(x,y), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0,0,255));
#endif
			}
		}
	}
	/******************************************************
	* Show thresholded Image
	*******************************************************/
	if (this->show_screen)
	{
		cv::imshow(this->name,thres);
	}


//
//	////check handfinder on the air fingers
//	
//	if(HandFinder::instance != NULL && HandFinder::instance->IsEnabled())
//	{
//		std::map<unsigned long, Hand*> hands = *HandFinder::instance->GetHands();
//		for(std::map<unsigned long, Hand*>::iterator it_hand = hands.begin(); it_hand != hands.end(); it_hand++)
//		{
//			for (std::vector <CvPoint>::iterator it_Touch = it_hand->second->fingers.begin(); 
//				it_Touch != it_hand->second->Touchs.end(); 
//				it_Touch++)
//			{
//				temp_minimum_distance = 99999999.0f;
//				candidate_id = 0;
//				temp_touch.Update((*it_Touch).x,(*it_Touch).y,-1);
//				for(Pointmap::iterator it_point = pointmap.begin(); it_point != pointmap.end(); it_point++)
//				{
//					float tmp_dist = fabs(fnsqdist(temp_touch.GetX(),temp_touch.GetY(),it_point->second->GetX(),it_point->second->GetY()));
//					if( temp_minimum_distance > tmp_dist)
//					{
//						candidate_id = it_point->first;
//						temp_minimum_distance = tmp_dist;
//					}
//				}
//				if(temp_minimum_distance > DISTANCE_OFFSET) candidate_id = 0;
//				if(candidate_id == 0) //new touch
//				{
//						unsigned int new_id = Globals::ssidGenerator++;
//						temp_touch.SetHandData(it_hand->first, true);
//						pointmap[new_id] = new Touch(temp_touch);
//				}
//				else //update touch
//				{
//					if(pointmap[candidate_id]->IsUpdated(true))
//					{
//						pointmap[candidate_id]->SetHandData(it_hand->first,false);
//						//pointmap[candidate_id]->Update(temp_touch.GetX(),temp_touch.GetY(),pointmap[candidate_id]->area);
//					}
//					else
//					{
//						pointmap[candidate_id]->SetHandData(it_hand->first,true);
//						pointmap[candidate_id]->Update(temp_touch.GetX(),temp_touch.GetY(),-1);
//					}
//				}
//			}
//		}
//	}
//	return main_processed_image;
}

void TouchFinder::RepportOSC()
{
	if(!this->IsEnabled()) return;
	to_be_removed.clear();
	for(Pointmap::iterator it = pointmap.begin(); it != pointmap.end(); it++)
	{
			if (it->second->IsUpdated())
			{
				//tuio message
				TuioServer::Instance().AddPointerMessage(
					it->first, 
					0, 
					0,
					((it->second->GetX()/1.333f)/Globals::width)+0.2f,//Globals::GetX(it->second->GetX()),// 
					it->second->GetY()/Globals::height, //Globals::GetY(it->second->GetY()),//
					it->second->area, 
					0);
			}
			else
			{
				//tuiomessage_remove
				to_be_removed.push_back(it->first);
			}
	}

	for(std::vector<unsigned int>::iterator it = to_be_removed.begin(); it != to_be_removed.end(); it++)
	{
		pointmap.erase(*it);
	}
}

AliveList TouchFinder::GetAlive()
{
	AliveList to_return;
	if(IsEnabled())
	{
		for(Pointmap::iterator it = pointmap.begin(); it!= pointmap.end(); it++)
		{
			to_return.push_back(it->first);
		}
	}
	return to_return;
}

void TouchFinder::BuildGui(bool force)
{
	if(force)
	{
		cv::destroyWindow(name);
		cv::namedWindow(name,CV_WINDOW_AUTOSIZE);
	}
	cv::createTrackbar("Enable", name,&enable_processor, 1, NULL);
	cv::createTrackbar("th.value", name,&Threshold_value, 255, NULL);
	cv::createTrackbar("minblob", name,&min_area, 2000, NULL);
	cv::createTrackbar("max.blob", name,&max_area, 2000, NULL);
}