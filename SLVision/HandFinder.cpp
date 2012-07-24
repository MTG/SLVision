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

#include "HandFinder.h"
#include "GlobalConfig.h"
#include "TuioServer.h"

#define MINIMUM_CENTROID_DISTANCE 80

HandFinder::HandFinder(void):FrameProcessor("HandFinder")
{
	firstcontour=NULL;
	polycontour=NULL;
	blob_moments = (CvMoments*)malloc( sizeof(CvMoments) );
	main_processed_image = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);		//allocates 1-channel memory frame for the image
	main_processed_contour = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);	//allocates 1-channel memory frame for the image
	//contour data allocation
	main_storage = cvCreateMemStorage (0);											//Creates a memory main_storage and returns pointer to it /param:/Size of the main_storage blocks in bytes. If it is 0, the block size is set to default value - currently it is 64K.
	main_storage_poligon = cvCreateMemStorage (0);

	int cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:enable",1);
	if(cf_enabled == 1) Enable(true);
	else Enable(false);

	threshold_value = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:threshold_value",100);

	max_area = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:max_hand_area",800);
	min_area = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:min_hand_area",200);
	min_pinch_blob_size = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:min_pinching_blobsize",1000);

	guiMenu->AddBar("0-Enable",0,1,1);
	guiMenu->AddBar("1-Threshold",0,255,1);
	guiMenu->AddBar("2-Min_Area",400,5000,10);
	guiMenu->AddBar("3-Max_Area",500,50000,10);
	guiMenu->AddBar("4-Min_pinch_blob",500,50000,100);

	guiMenu->SetValue("0-Enable",(float)cf_enabled);
	guiMenu->SetValue("1-Threshold",(float)threshold_value);
	guiMenu->SetValue("2-Min_Area",(float)min_area);
	guiMenu->SetValue("3-Max_Area",(float)max_area);
	guiMenu->SetValue("4-Min_pinch_blob",(float)min_pinch_blob_size);
}

void HandFinder::UpdatedValuesFromGui()
{
	int &cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:enable",1);
	int &cf_threshold = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:threshold_value",100);
	int &cf_min_area = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:min_hand_area",200);
	int &cf_max_area = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:max_hand_area",800);
	int &cf_min_pinch_blob = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:min_pinching_blobsize",1000);

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

	threshold_value = (int)ceil(guiMenu->GetValue("1-Threshold"));
	cf_threshold = threshold_value;

	min_area = (int)guiMenu->GetValue("2-Min_Area");
	max_area = (int)guiMenu->GetValue("2-Max_Area");
	cf_min_area = min_area;
	cf_max_area = max_area;

	min_pinch_blob_size = (int) guiMenu->GetValue("4-Min_pinch_blob");
	cf_min_pinch_blob = min_pinch_blob_size;
}

HandFinder::~HandFinder(void)
{
}

AliveList HandFinder::GetAlive()
{
	AliveList toreturn;
	if(this->IsEnabled())
	{
		for(std::map<unsigned long, Hand*>::iterator it = hands.begin(); it != hands.end(); it++)
			toreturn.push_back(it->first);
	}
	return toreturn;
}

void HandFinder::KeyInput(char key)
{
}

IplImage* HandFinder::Process(IplImage*	main_image)
{
	cvClearMemStorage(main_storage);
	cvClearMemStorage(main_storage_poligon);

	cvThreshold(main_image,main_processed_image,threshold_value,255, CV_THRESH_BINARY);
	cvCopy(main_processed_image,main_processed_contour);	
	cvFindContours (main_processed_contour, main_storage, &firstcontour, sizeof (CvContour), CV_CHAIN_APPROX_SIMPLE/*CV_RETR_CCOMP*/);

	if(firstcontour != NULL)
	{
		polycontour=cvApproxPoly(firstcontour,sizeof(CvContour),main_storage_poligon,CV_POLY_APPROX_DP,4,1);

		for(CvSeq* c=polycontour;c!=NULL;c=c->h_next)
		{
			area = 0;
			length = 0;
			area = (float)fabs ( cvContourArea(c,CV_WHOLE_SEQ));

			if(area >  min_area/* && area < max_area*/)
			{
				CvSeq* hull;
				hull = cvConvexHull2(c, 0, CV_CLOCKWISE, 0 );
				cvMoments( c, blob_moments );
				hand_centroid.x = (int) ceil((blob_moments->m10 / blob_moments->m00));
				hand_centroid.y = (int) ceil((blob_moments->m01 / blob_moments->m00));
				Hand * hand = 0;

				//Get target hand
				float dist = 999;
				float tmp_dist;
				unsigned long sessionID = 0;

				for(std::map<unsigned long, Hand*>::iterator it = hands.begin(); it != hands.end(); it++)
				{
					tmp_dist = it->second->Distance(hand_centroid);
					if(tmp_dist < dist)
					{
						dist = tmp_dist;
						hand = it->second;
						sessionID = it->first;
					}
				}
				if(dist > MINIMUM_CENTROID_DISTANCE || hand == 0)
				{
					sessionID = Globals::ssidGenerator++;
					hands[sessionID] = new Hand(sessionID,hand_centroid);
					hand = hands[sessionID];
				}

				//alive.push_back(hand->GetSessionID());
				hand->Update(hand_centroid);
				length = (float)cvArcLength( c );

				hand->Clear();
				CvPoint           pt;
				CvSeqReader       reader;
				cvStartReadSeq( c, &reader, 0 );
    			for( int j=0; j < c->total; j++ )
    			{
					CV_READ_SEQ_ELEM( pt, reader );
					hand->AddVertex(pt.x, pt.y);
				}

				///retreive the hull path
				int             hullcount = hull->total;
				float          aa, bb;
				CvPoint         pt0;
				CvRect rect	= cvBoundingRect( c, 0 );
				for(int j = 0; j < hullcount; j++)
				{
					pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, j );
					if(j == hullcount-1)
						pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, 0 );
					else
						pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, j+1 );
					aa = (float)pt0.x - (float)pt.x;
					bb = (float)pt0.y - (float)pt.y;
					if (  sqrtf ( (aa*aa) + (bb*bb) ) > (float)rect.width / 10.0)
						hand->AddVertexConvex(pt0.x, pt0.y);
				}

				hand->ComputeHand(area, length);
				hand->draw();

				//compute hand holes-->pinching gestures
				CvSeq* c_vnext = c->v_next;
				CvSeq* candidate = NULL;
				double max_area=0;
				if( c_vnext != NULL)
				{
					for(CvSeq* h=c_vnext;h!=NULL;h=h->h_next)
					{
						area = (float)fabs ( cvContourArea(h,CV_WHOLE_SEQ));
						if(max_area < area)
						{
							max_area = area;
							candidate = h;
						}
					}
				}

				if ( max_area > min_pinch_blob_size)
				{
					hand->SetPinch(candidate);
				}
				else
					hand->SetPinch(NULL);

			}
		}

	}

	to_remove.clear();
	for(std::map<unsigned long, Hand*>::iterator it = hands.begin(); it != hands.end(); it++)
	{
		if(it->second->IsUpdated())
		{
			//send OSC MEssage
			TuioServer::Instance().AddHand(
				it->first,
				it->second->IsConfirmedAsHand(),
				it->second->IsOpened(),
				Globals::GetX(it->second->TCentroidX()),//it->second->TCentroidX(), 
				Globals::GetY(it->second->TCentroidY()),//it->second->TCentroidY(), 
				it->second->GetArea());			
			TuioServer::Instance().AddHandPath(it->first,it->second->vertexs);
			if(it->second->IsPinching() || it->second->IsPinchingEnd())
			{
				TuioServer::Instance().AddHandPinch(it->first,it->second->hand_hole);
			}
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

	return main_processed_image;
}