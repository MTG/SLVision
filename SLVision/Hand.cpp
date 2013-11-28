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

#include "Hand.h"
#include "Fiducial.h"
#include "Globals.h"

	Hand::Hand(void)
	{
		Reset();
		centroid = cv::Point(-1,-1);
	}

	Hand::Hand(unsigned long _sessionID, const cv::Point & _centroid, float area)
	{
		Reset();
		sessionID = _sessionID;
		centroid = cv::Point(_centroid);
		center_hand = cv::Point(_centroid);
		this->area = area;
		pinch_area = 0;
	}

	bool Hand::IsItTheSame( cv::Point &point )
	{
		if(fabsf(insqdist(centroid.x, centroid.y, point.x, point.y)) <= HAND_CENTROID_SIMILARITY)
			return true;
		return false;
	}

	void Hand::Reset()
	{
		sessionID = 0;
		startarm = cv::Point(0,0);
		center_hand = cv::Point(0,0);
		fingers[0] = cv::Point(-1,-1);
		fingers[1] = cv::Point(-1,-1);
		fingers[2] = cv::Point(-1,-1);
		fingers[3] = cv::Point(-1,-1);
		fingers[4] = cv::Point(-1,-1);
		is_updated = false;
		hull.clear();
		blobPath.clear();
		defects.clear();
		pinch_area = 0;
	}

	void Hand::UpdateData( cv::Point &point, cv::vector<cv::Point> &path ,  float area )
	{
		this->area = area;
		is_updated = true;
		this->center_hand = cv::Point(point);
		this->blobPath = cv::vector<cv::Point>(path);
		//******************************************************
		//* Find convex Hull
		//*******************************************************
		hull.clear();
		cv::convexHull( cv::Mat(blobPath), hull, false );
		//******************************************************
		//* Find defects (Hull valleys)
		//*******************************************************
		defects.clear();
		cv::convexityDefects(cv::Mat(blobPath), hull, defects);
		//******************************************************
		//* Find fingers and startarm
		//*******************************************************
		//Start arm calculation
		auxiliar_distance = 99;
		edge_index = -1;
		for(unsigned int i = 0; i < defects.size(); i++)
		{
			temp_dist = IsNearEdge( blobPath[defects[i][1]] );
			if( temp_dist < auxiliar_distance)
			{
				auxiliar_distance = temp_dist;
				edge_index = defects[i][1];
			}
		}

		//Check for a propper hand
		if(edge_index == -1) //not a hand (misssed arm)
		{
			is_hand = false;
			return;
		}
		else is_hand = true;

		startarm = cv::Point(blobPath[edge_index].x, blobPath[edge_index].y);

		///Find further point of the path
		further_index = -1;
		auxiliar_distance = 0;
		for(unsigned int i = 0; i < defects.size(); i++)
		{
			if(defects[i][1] != edge_index)
			{
				temp_dist =  insqdist(startarm.x, startarm.y, blobPath[defects[i][1]].x, blobPath[defects[i][1]].y);
				if(temp_dist > auxiliar_distance)
				{
					auxiliar_distance = temp_dist;
					further_index = defects[i][1];
				}
			}
		}
		//This value gonna be used as crop distance 
		auxiliar_distance = auxiliar_distance /2.0f;
		//

		endarm = cv::Point(blobPath[further_index].x,blobPath[further_index].y);

		center_hand.x = 0;
		center_hand.y = 0;
		finger_candidates.clear();
		int q=0;
		for(unsigned int i = 0; i < defects.size(); i++)
		{
			if( insqdist(endarm.x, 
						 endarm.y, 
						 blobPath[defects[i][1]].x, 
						 blobPath[defects[i][1]].y) < auxiliar_distance)
			{
				center_hand.x += blobPath[defects[i][1]].x;
				center_hand.y += blobPath[defects[i][1]].y;
				q++;
				finger_candidates.push_back(i);
			}

			if( insqdist(endarm.x, 
						 endarm.y, 
						 blobPath[defects[i][2]].x, 
						 blobPath[defects[i][2]].y) < auxiliar_distance)
			{
				center_hand.x += blobPath[defects[i][2]].x;
				center_hand.y += blobPath[defects[i][2]].y;
				q++;
			}
		}

		center_hand.x = (int)floor((float)center_hand.x/(float)q);
		center_hand.y = (int)floor((float)center_hand.y/(float)q);

		//Check influence_hand_radius
		influence_radius = 0;
		for(unsigned int i = 0; i < finger_candidates.size(); i++)
		{
			temp_dist = insqdist(center_hand.x, 
						 center_hand.y, 
						 blobPath[defects[finger_candidates[i]][1]].x, 
						 blobPath[defects[finger_candidates[i]][1]].y);

			if( temp_dist > influence_radius)
			{
				influence_radius = temp_dist;
			}
		}

		//Asign fingers
		fingers[0] = cv::Point(-1,-1);
		fingers[1] = cv::Point(-1,-1);
		fingers[2] = cv::Point(-1,-1);
		fingers[3] = cv::Point(-1,-1);
		fingers[4] = cv::Point(-1,-1);
		if(finger_candidates.size() <= 5)
		{
			for(unsigned int i = 0; i < finger_candidates.size(); i++)
			{
				fingers[i] = cv::Point(blobPath[defects[finger_candidates[i]][1]]);
			}
		}
		else
		{  //future_work check distance with theis inmediate deffect (valley) to discriminate the nonfinger candidates
			for(int i = 0; i < 5; i++)
			{
				fingers[i] = cv::Point(blobPath[defects[finger_candidates[i]][1]]);
			}
		}

		//Cross_reference_fingers??
		pinch_area = 0;
	}

	void Hand::AddPinch( cv::vector<cv::Point> &path, float area )
	{
		int q = 0;
		pinch_centre.x = 0;
		pinch_centre.y = 0;
		for( unsigned int i = 0; i < path.size(); i++)
		{
			pinch_centre.x += path[i].x;
			pinch_centre.y += path[i].y;
			q++;
		}
		pinch_centre.x = (int)floor((float)pinch_centre.x/(float)q);
		pinch_centre.y = (int)floor((float)pinch_centre.y/(float)q);
		
		pinch_area = 0;
		for( unsigned int i = 0; i < path.size(); i++)
		{
			temp_dist = insqdist(pinch_centre.x, 
						 pinch_centre.y, 
						 path[i].x, 
						 path[i].y);

			if( temp_dist > pinch_area)
			{
				pinch_area = temp_dist;
			}
		}
	}

	float Hand::IsNearEdge( cv::Point & p )
	{
		float shortest = 9000;
		if(p.x  < shortest) shortest = (float)p.x;
		if(p.y  < shortest) shortest = (float)p.y;
		if(p.x - Globals::CamSize.width >= 0 &&  p.x - Globals::CamSize.width < shortest) shortest = (float)(p.x - Globals::CamSize.width);
		if(p.y - Globals::CamSize.height >= 0 &&  p.y - Globals::CamSize.height < shortest) shortest = (float)(p.y - Globals::CamSize.height);

		if ( p.x > 10 && p.y > 10 && p.x <= Globals::CamSize.width-10 && p.y <= Globals::CamSize.height-10)
			return 99;
		return shortest;
	}

	void Hand::Draw(bool force)
	{
		if(!force && !is_hand) return;

		//draw blob path
		for(unsigned int i = 0; i < blobPath.size(); i++)
		{
			if(i+1 != blobPath.size())
				cv::line(Globals::CameraFrame,blobPath[i],blobPath[i+1],cv::Scalar(0,255,255,255),1,CV_AA);
			else
				cv::line(Globals::CameraFrame,blobPath[i],blobPath[0],cv::Scalar(0,255,255,255),1,CV_AA);
		}

		//draw startarm and endarm
		cv::circle(Globals::CameraFrame,startarm,10,cv::Scalar(255,0,0),5);
		cv::circle(Globals::CameraFrame,endarm,10,cv::Scalar(255,0,255),5);

		//draw hand influence
		cv::circle(Globals::CameraFrame,center_hand,(int)floor(influence_radius),cv::Scalar(0,0,255),3);

		//draw fingers
		for(int i = 0; i < 5; i++)
		{
			if(fingers[i].x != -1)
			{
				cv::circle(Globals::CameraFrame,fingers[i],5,cv::Scalar(0,255,0),5);
			}
		}

		//draw pinch
		if(pinch_area != 0)
		{
			cv::circle(Globals::CameraFrame,pinch_centre,(int)floor(pinch_area),cv::Scalar(0,255,255),3);
		}

	}

	unsigned long Hand::GetSID()
	{
		return sessionID;
	}

	bool Hand::IsValid()
	{
		if(centroid.x == -1 && centroid.y == -1)
			return false;
		return true;
	}