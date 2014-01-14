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

#include "HandFinder.h"
#include "GlobalConfig.h"
#include "Globals.h"
#include "TuioServer.h"

HandFinder::HandFinder(void):
	Threshold_value(datasaver::GlobalConfig::getRef("FrameProcessor:HandFinder:threshold:threshold_value",33)),
	min_area(datasaver::GlobalConfig::getRef("FrameProcessor:HandFinder:threshold:minimum_tpuch_area",200)),
	FrameProcessor("HandFinder")
{
	if(enable_processor == 1) Enable(true);
	else Enable(false);

	touch_finder = NULL;
	//create gui
	BuildGui();
	hands.clear();
}

HandFinder::~HandFinder(void)
{
}

AliveList HandFinder::GetAlive()
{
	AliveList toreturn;
	if(this->IsEnabled())
	{
		for(std::map<unsigned long, Hand>::iterator it = hands.begin(); it != hands.end(); it++)
			toreturn.push_back(it->first);
	}
	return toreturn;
}

void HandFinder::Process(cv::Mat&	main_image)
{
	/******************************************************
	* Convert image to graycsale
	*******************************************************/
	grey=main_image;
	/******************************************************
	* Apply threshold
	*******************************************************/
	cv::threshold(grey,thres,Threshold_value,255,cv::THRESH_BINARY/* | CV_THRESH_OTSU*/);

	thres_contours = thres.clone();
	/******************************************************
	* Find contours
	*******************************************************/
	cv::findContours ( thres_contours , contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE );
	unsigned int idx = 0;
    //******************************************************
	//* Find Hand candidates
	//*******************************************************
	
	for( ; idx >= 0; idx = hierarchy[idx][0] )
    {
		//******************************************************
		// Filter by area
		//*******************************************************
		if (contours.size() <= idx)break;
		float area = (float)cv::contourArea(contours[idx]);
		if(area >= /*min_area*/1000)
		{
			//******************************************************
			//* reduce contour complexity
			//*******************************************************
			cv::vector<cv::Point> approxCurve;
			cv::approxPolyDP (  contours[idx]  ,approxCurve , 2, true );
			//******************************************************
			//* calculates centroid
			//*******************************************************
			cv::Moments hand_blob_moments = cv::moments(contours[idx],true);
			float x = (float)(hand_blob_moments.m10 / hand_blob_moments.m00);
			float y = (float)(hand_blob_moments.m01 / hand_blob_moments.m00);
			//******************************************************
			//* Find Hand Candidate
			//*******************************************************
			unsigned long candidate = 0;
			for ( std::map<unsigned long, Hand>::iterator it = hands.begin(); it != hands.end(); it++)
			{
				if(it->second.IsItTheSame(cv::Point((int)x,(int)y)))
					candidate = it->first;
			}
			if( candidate == 0)
			{
				std::cout << "hand start" << area << std::endl;
				unsigned long newsid = Globals::ssidGenerator++;
				hands[newsid] = Hand(newsid,cv::Point((int)x,(int)y),area);
				candidate = newsid;
				std::cout << "hand end" << std::endl;
			}
			//******************************************************
			//* Update Hand
			//*******************************************************
			if(candidate != 0 && hands.find(candidate) != hands.end())
			{
				hands[candidate].UpdateData(cv::Point((int)x,(int)y),approxCurve,area,touch_finder);
				//******************************************************
				//* Find pinch
				//*******************************************************
				if(hierarchy[idx][2] != -1) //in a future, if match navigate through all candidates
				{
					float pinch_area = (float)cv::contourArea(contours[hierarchy[idx][2]]);
					if(pinch_area > 1000)
					{
						cv::vector<cv::Point> approxCurve;
						cv::approxPolyDP (  contours[hierarchy[idx][2]]  ,approxCurve , 2, true );
						hands[candidate].AddPinch(approxCurve,pinch_area);
					}
				}
			}
			if(Globals::is_view_enabled)
			{
				for ( std::map<unsigned long, Hand>::iterator it = hands.begin(); it != hands.end(); it++)
					it->second.Draw(touch_finder);
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

void HandFinder::RepportOSC()
{
	if(!this->IsEnabled())return;
	to_remove.clear();
	for(std::map<unsigned long, Hand>::iterator it = hands.begin(); it != hands.end(); it++)
	{
		if(it->second.IsUpdated()&& it->second.IsValid())
		{
			//send OSC MEssage
			//id, centroidx, centroidy, area, 
			//startarmx, startarmy, endarmx, endarmy, 
			//handx, handy, handinfluence,
			//pinchx, pinchy, pinchinfluence, 
			//numfingers
			TuioServer::Instance().AddHand(
				it->first,
				(float)it->second.GetCentroid().x, (float)it->second.GetCentroid().y, (float)it->second.GetArea(),
				(float)it->second.GetStartArm().x, (float)it->second.GetStartArm().y,
				(float)it->second.GetEndArm().x, (float)it->second.GetEndArm().y,
				(float)it->second.GetHandPoint().x, (float)it->second.GetHandPoint().y, (float)it->second.GetHandInfluence(),
				(float)it->second.GetPinchPoint().x, (float)it->second.GetPinchPoint().y, (float)it->second.GetPinchInfluence(),
				it->second.GetNumFingers()
				);		

			//id blob path
			TuioServer::Instance().AddHandPath(it->first,it->second.GetPath());

			//Check finger integrity and mix data
			//id, finger0x, finger0y, ... , finger4y
		}
		else
		{
			to_remove.push_back(it->first);
		}
	}

	for( std::vector<unsigned long>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
	{
		hands.erase(*it);
	}
}

void HandFinder::BuildGui(bool force)
{
	cv::createTrackbar("Enable", name,&enable_processor, 1, NULL);
	cv::createTrackbar("th.value", name,&Threshold_value, 255, NULL);
}
