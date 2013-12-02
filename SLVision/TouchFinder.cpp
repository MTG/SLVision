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

#include "TouchFinder.h"
#include "GlobalConfig.h"
#include "Globals.h"
#include "Fiducial.h"
#include "TuioServer.h"
#define DISTANCE_OFFSET 50


TouchFinder::TouchFinder(void):
	Threshold_value (datasaver::GlobalConfig::getRef("FrameProcessor:TouchFinder:threshold:threshold_value",205)),
	max_area(datasaver::GlobalConfig::getRef("FrameProcessor:TouchFinder:threshold:maximum_touch_area",1209)),
	min_area(datasaver::GlobalConfig::getRef("FrameProcessor:TouchFinder:threshold:minimum_tpuch_area",261)),
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
	cv::findContours ( thres_contours , contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
	int idx = 0;
    /******************************************************
	* Find touch candidates
	*******************************************************/
	if(hierarchy.size()!= 0)
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
			
			///////
			UpdateCandidate(&temp_touch);
			///////
			if(Globals::is_view_enabled)
			{
				/******************************************************
				* Draw contour and data
				*******************************************************/
				drawContours( Globals::CameraFrame, contours, idx, cv::Scalar(255,0,255), CV_FILLED, 8, hierarchy );
#ifdef DRAW_EVERYTHING
				std::stringstream s;
				s << candidate_id << "  (" <<pointmap[candidate_id]->GetX() << ", " << pointmap[candidate_id]->GetY() <<")";
				cv::putText(Globals::CameraFrame, s.str(), cv::Point(x,y), cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(255,255,255));
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
}

unsigned int TouchFinder::UpdateCandidate(Touch* temp_touch)
{
	int candidate_id= 0;
	float temp_minimum_distance = 99999999.0f;
	/******************************************************
	* Check for existing touches and retreive the ID
	*******************************************************/
	for(Pointmap::iterator it = pointmap.begin(); it != pointmap.end(); it++)
	{
		float tmp_dist = fabs(fnsqdist(temp_touch->GetX(),temp_touch->GetY(),it->second->GetX(),it->second->GetY()));
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
		pointmap[new_id] = new Touch(*temp_touch);
		candidate_id = new_id;
	}
	else //update touch
	{
		pointmap[candidate_id]->Update(*temp_touch);
	}

	return candidate_id;
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
					it->second->GetX(),
					it->second->GetY(),
					it->second->area, 
					0,
					it->second->IsOnTheAir(),
					it->second->GetHandID());
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

unsigned int TouchFinder::GetTouch(float x, float y, unsigned long handid)
{
	Touch candidate = Touch();
	candidate.SetHandData(handid,x,y);
	return UpdateCandidate(&candidate);
}

Touch* TouchFinder::GetTouch(unsigned int id)
{
	if (pointmap.find(id) != pointmap.end())
	{
		return pointmap[id];
	}
	return NULL;
}