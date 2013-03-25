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
#include "Fiducial.h"
#include "TuioServer.h"
#define DISTANCE_OFFSET 50
#include "HandFinder.h"

TouchFinder::TouchFinder(void):FrameProcessor("TouchFinder")
{
	main_processed_image = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);		//allocates 1-channel memory frame for the image
	main_processed_contour = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);	//allocates 1-channel memory frame for the image
	
	// Creates a memory main_storage and returns pointer to 
	// it /param:/Size of the main_storage blocks in bytes. 
	// If it is 0, the block size is set to default 
	// value - currently it is 64K.
	main_storage = cvCreateMemStorage (0);

	blob_moments = (CvMoments*)malloc( sizeof(CvMoments) );

	int cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:enable",1);
	if(cf_enabled == 1) Enable(true);
	else Enable(false);	
	threshold_value = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:threshold_value",100);

	max_blob_size = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:maximum_blob_size",60);
	min_blob_size = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:minimum_blob_size",10);

	//populate gui
	guiMenu->AddBar("0-Enable",0,1,1);
	guiMenu->AddBar("1-Threshold",0,255,1);
	guiMenu->AddBar("2-Max_blob_size",10,200,2);
	guiMenu->AddBar("3-Min_blob_size",10,100,2);
	guiMenu->SetValue("0-Enable",(float)cf_enabled);
	guiMenu->SetValue("1-Threshold",(float)threshold_value);
	guiMenu->SetValue("2-Max_blob_size",(float)max_blob_size);
	guiMenu->SetValue("3-Min_blob_size",(float)min_blob_size);
}

TouchFinder::~TouchFinder(void)
{
}

void TouchFinder::KeyInput(char key)
{
}

void TouchFinder::UpdatedValuesFromGui()
{
	int &cf_threshold = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:threshold_value",70);
	threshold_value = (int)ceil(guiMenu->GetValue("1-Threshold"));
	cf_threshold = threshold_value;

	int &cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:enable",1);
	if(guiMenu->GetValue("0-Enable") == 0)
	{
		Enable(false);
		cf_enabled = 0;
	}
	else if(guiMenu->GetValue("0-Enable") == 1) 
	{
		Enable(true);
		cf_enabled = 1;
	}
	
	int &cf_max_blob_size = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:maximum_blob_size",60);;
	max_blob_size = (int)guiMenu->GetValue("2-Max_blob_size");
	cf_max_blob_size = max_blob_size;

	int &cf_min_blob_size = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:minimum_blob_size",10);;
	min_blob_size = (int)guiMenu->GetValue("3-Min_blob_size");
	cf_min_blob_size = min_blob_size;
}

IplImage* TouchFinder::Process(IplImage* main_image)
{
	cvThreshold(main_image,main_processed_image,threshold_value,255, CV_THRESH_BINARY);
	cvCopy(main_processed_image,main_processed_contour);
	cvFindContours (main_processed_contour, main_storage, &firstcontour, sizeof (CvContour), CV_RETR_CCOMP);
	//polycontour=cvApproxPoly(firstcontour,sizeof(CvContour),main_storage_poligon,CV_POLY_APPROX_DP,3,1);
	for(CvSeq* c=firstcontour;c!=NULL;c=c->h_next)
	{
		if( c -> v_next == NULL && 
			(cvContourPerimeter(c) >= min_blob_size) && 
			(cvContourPerimeter(c) <= max_blob_size)) //looking for single blobs
		{
			if(Globals::is_view_enabled)cvDrawContours(Globals::screen,c,CV_RGB(255,255,0),CV_RGB(200,255,255),0,3);
			
			cvMoments( c, blob_moments );
			//temp_area = fabs(cvContourArea( c, CV_WHOLE_SEQ ));
			//temp_x = (float)(blob_moments->m10 / blob_moments->m00);
			//temp_y = (float)(blob_moments->m01 / blob_moments->m00);

			temp_touch.Update(	(float)(blob_moments->m10 / blob_moments->m00), 
								(float)(blob_moments->m01 / blob_moments->m00), 
								(float)fabs(cvContourArea( c, CV_WHOLE_SEQ ))
							 );
			
			temp_minimum_distance = 99999999.0f;
			candidate_id = 0;

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
					pointmap[new_id] = new Finger(temp_touch);
			}
			else //update touch
			{
				pointmap[candidate_id]->Update(temp_touch);
			}
		}
	}

	////check handfinder on the air fingers
	
	if(HandFinder::instance != NULL && HandFinder::instance->IsEnabled())
	{
		std::map<unsigned long, Hand*> hands = *HandFinder::instance->GetHands();
		for(std::map<unsigned long, Hand*>::iterator it_hand = hands.begin(); it_hand != hands.end(); it_hand++)
		{
			for (std::vector <CvPoint>::iterator it_finger = it_hand->second->fingers.begin(); 
				it_finger != it_hand->second->fingers.end(); 
				it_finger++)
			{
				temp_minimum_distance = 99999999.0f;
				candidate_id = 0;
				temp_touch.Update((*it_finger).x,(*it_finger).y,-1);
				for(Pointmap::iterator it_point = pointmap.begin(); it_point != pointmap.end(); it_point++)
				{
					float tmp_dist = fabs(fnsqdist(temp_touch.GetX(),temp_touch.GetY(),it_point->second->GetX(),it_point->second->GetY()));
					if( temp_minimum_distance > tmp_dist)
					{
						candidate_id = it_point->first;
						temp_minimum_distance = tmp_dist;
					}
				}
				if(temp_minimum_distance > DISTANCE_OFFSET) candidate_id = 0;
				if(candidate_id == 0) //new touch
				{
						unsigned int new_id = Globals::ssidGenerator++;
						temp_touch.SetHandData(it_hand->first, true);
						pointmap[new_id] = new Finger(temp_touch);
				}
				else //update touch
				{
					if(pointmap[candidate_id]->IsUpdated(true))
					{
						pointmap[candidate_id]->SetHandData(it_hand->first,false);
						//pointmap[candidate_id]->Update(temp_touch.GetX(),temp_touch.GetY(),pointmap[candidate_id]->area);
					}
					else
					{
						pointmap[candidate_id]->SetHandData(it_hand->first,true);
						pointmap[candidate_id]->Update(temp_touch.GetX(),temp_touch.GetY(),-1);
					}
				}
			}
		}
	}
	return main_processed_image;
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
				//(unsigned int sid, unsigned int uid, unsigned int cid, float x, float y, float width, float press)
				TuioServer::Instance().AddPointerMessage(
					it->first, 
					0, 
					0,
					Globals::GetX(it->second->GetX()),//it->second->x/Globals::width, 
					Globals::GetY(it->second->GetY()),//it->second->y/Globals::height, 
					it->second->area, 
					0);
			}
			else
			{
				//tuiomessage_remove
				to_be_removed.push_back(it->first);
			}

		if(Globals::is_view_enabled)
		{
			sprintf_s(text,"%i",it->first); 
			if(it->second->IsOnTheAir())
				Globals::Font::Write(
					Globals::screen,
					text,
					cvPoint((int)it->second->GetX(), (int)it->second->GetY()),
					FONT_HELP,
					0,0,255);
			else
				Globals::Font::Write(
					Globals::screen,
					text,
					cvPoint((int)it->second->GetX(), (int)it->second->GetY()),
					FONT_HELP,
					0,255,0);
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