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
//#include "TuioServer.h"

HandFinder::HandFinder(void):
	Threshold_value(datasaver::GlobalConfig::getRef("FrameProcessor:HandFinder:threshold:threshold_value",33)),
	min_area(datasaver::GlobalConfig::getRef("FrameProcessor:HandFinder:threshold:minimum_tpuch_area",200)),
	FrameProcessor("HandFinder")
{
	if(enable_processor == 1) Enable(true);
	else Enable(false);

	//create gui
	BuildGui();

	hands.clear();
//	firstcontour=NULL;
//	polycontour=NULL;
//	blob_moments = (CvMoments*)malloc( sizeof(CvMoments) );
//	main_processed_image = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);		//allocates 1-channel memory frame for the image
//	main_processed_contour = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);	//allocates 1-channel memory frame for the image
//	//contour data allocation
//	main_storage = cvCreateMemStorage (0);											//Creates a memory main_storage and returns pointer to it /param:/Size of the main_storage blocks in bytes. If it is 0, the block size is set to default value - currently it is 64K.
//	main_storage_poligon = cvCreateMemStorage (0);
//
//	int cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:enable",1);
//	if(cf_enabled == 1) Enable(true);
//	else Enable(false);
//
//	threshold_value = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:threshold_value",100);
//
//	max_area = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:max_hand_area",800);
//	min_area = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:min_hand_area",200);
//	min_pinch_blob_size = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:min_pinching_blobsize",1000);
//
//	guiMenu->AddBar("0-Enable",0,1,1);
//	guiMenu->AddBar("1-Threshold",0,255,1);
//	guiMenu->AddBar("2-Min_Area",400,5000,10);
//	guiMenu->AddBar("3-Max_Area",500,50000,10);
//	guiMenu->AddBar("4-Min_pinch_blob",500,50000,100);
//
//	guiMenu->SetValue("0-Enable",(float)cf_enabled);
//	guiMenu->SetValue("1-Threshold",(float)threshold_value);
//	guiMenu->SetValue("2-Min_Area",(float)min_area);
//	guiMenu->SetValue("3-Max_Area",(float)max_area);
//	guiMenu->SetValue("4-Min_pinch_blob",(float)min_pinch_blob_size);
}

HandFinder::~HandFinder(void)
{
}


//void HandFinder::UpdatedValuesFromGui()
//{
//	int &cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:enable",1);
//	int &cf_threshold = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:threshold_value",100);
//	int &cf_min_area = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:min_hand_area",200);
//	int &cf_max_area = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:max_hand_area",800);
//	int &cf_min_pinch_blob = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:min_pinching_blobsize",1000);
//
//	if(guiMenu->GetValue("0-Enable") == 0)
//	{
//		Enable(false);
//		cf_enabled = 0;
//	}
//	else if(guiMenu->GetValue("0-Enable") == 1) 
//	{
//		Enable(true);
//		cf_enabled = 1;
//	}	
//
//	threshold_value = (int)ceil(guiMenu->GetValue("1-Threshold"));
//	cf_threshold = threshold_value;
//
//	min_area = (int)guiMenu->GetValue("2-Min_Area");
//	max_area = (int)guiMenu->GetValue("2-Max_Area");
//	cf_min_area = min_area;
//	cf_max_area = max_area;
//
//	min_pinch_blob_size = (int) guiMenu->GetValue("4-Min_pinch_blob");
//	cf_min_pinch_blob = min_pinch_blob_size;
//}


//bool HandFinder::TouchInHand(float x, float y)
//{
//	if(this->IsEnabled())
//	{
//		for(std::map<unsigned long, Hand*>::iterator it = hands.begin(); it != hands.end(); it++)
//		{
//			if(it->second->IsFinger(x,y))
//				return true;
//		}
//	}
//	return false;
//}
//
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
//
//void HandFinder::KeyInput(char key)
//{
//}
//

void HandFinder::Process(cv::Mat&	main_image)
{
	/******************************************************
	* Redraw GUI if needed
	*******************************************************/
	/*if(use_adaptive_bar_value == 0 && use_adaptive_threshold == true)
	{
		use_adaptive_threshold = false;
		BuildGui(true);
	}
	else if (use_adaptive_bar_value == 1 && use_adaptive_threshold == false)
	{
		use_adaptive_threshold = true;
		BuildGui(true);
	}*/
	/******************************************************
	* Convert image to graycsale
	*******************************************************/
	//if ( main_image.type() ==CV_8UC3 )   cv::cvtColor ( main_image,grey,CV_BGR2GRAY );
	//else     grey=main_image;
	grey=main_image;
	/******************************************************
	* Apply threshold
	*******************************************************/
	/*if(use_adaptive_threshold)
	{
		if (block_size<3) block_size=3;
		if (block_size%2!=1) block_size++;
		cv::adaptiveThreshold ( grey,thres,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY,block_size,threshold_C );
	}
	else */
		cv::threshold(grey,thres,Threshold_value,255,cv::THRESH_BINARY | CV_THRESH_OTSU);

	thres_contours = thres.clone();
	/******************************************************
	* Find contours
	*******************************************************/
	cv::findContours ( thres_contours , contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE );
	int idx = 0;
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
		if(area >= min_area)
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
//			cv::circle(Globals::CameraFrame,cv::Point(x,y),10,cv::Scalar(255,0,0),5);
			//******************************************************
			//* Find Hand Candidate
			//*******************************************************
			unsigned long candidate = 0;
			for ( std::map<unsigned long, Hand>::iterator it = hands.begin(); it != hands.end(); it++)
			{
				if(it->second.IsItTheSame(cv::Point(x,y)))
					candidate = it->first;
			}
			if( candidate == 0)
			{
				unsigned long newsid = Globals::ssidGenerator++;
				hands[newsid] = Hand(newsid,cv::Point(x,y));
				candidate = newsid;
			}
			//******************************************************
			//* Update Hand
			//*******************************************************
			if(candidate != 0 && hands.find(candidate) != hands.end())
			{
				hands[candidate].UpdateData(cv::Point(x,y),approxCurve);
			}

			


			

			//******************************************************
			//* Find pinch
			//*******************************************************

		}

		to_be_removed.clear();
		for ( std::map<unsigned long, Hand>::iterator it = hands.begin(); it != hands.end(); it++)
		{
			if(!it->second.is_updated) to_be_removed.push_back(it->first);
			it->second.is_updated = false;
		}
		for (std::vector<unsigned long>::iterator it = to_be_removed.begin(); it != to_be_removed.end(); it++)
		{
			hands.erase(*it);
		}

		if(Globals::is_view_enabled)
		{
			for ( std::map<unsigned long, Hand>::iterator it = hands.begin(); it != hands.end(); it++)
				it->second.Draw();
		}

	}




//	if(firstcontour != NULL)
//	{
//		polycontour=cvApproxPoly(firstcontour,sizeof(CvContour),main_storage_poligon,CV_POLY_APPROX_DP,4,1);
//
//		for(CvSeq* c=polycontour;c!=NULL;c=c->h_next)
//		{
//			area = 0;
//			length = 0;
//			area = (float)fabs ( cvContourArea(c,CV_WHOLE_SEQ));
//
//			if(area >  min_area/* && area < max_area*/)
//			{
//				CvSeq* hull;
//				hull = cvConvexHull2(c, 0, CV_CLOCKWISE, 0 );
//				cvMoments( c, blob_moments );
//				hand_centroid.x = (int) ceil((blob_moments->m10 / blob_moments->m00));
//				hand_centroid.y = (int) ceil((blob_moments->m01 / blob_moments->m00));
//				Hand * hand = 0;
//
//				//Get target hand
//				float dist = 999;
//				float tmp_dist;
//				unsigned long sessionID = 0;
//
//				for(std::map<unsigned long, Hand*>::iterator it = hands.begin(); it != hands.end(); it++)
//				{
//					tmp_dist = it->second->Distance(hand_centroid);
//					if(tmp_dist < dist)
//					{
//						dist = tmp_dist;
//						hand = it->second;
//						sessionID = it->first;
//					}
//				}
//				if(dist > MINIMUM_CENTROID_DISTANCE || hand == 0)
//				{
//					sessionID = Globals::ssidGenerator++;
//					hands[sessionID] = new Hand(sessionID,hand_centroid);
//					hand = hands[sessionID];
//				}
//
//				//alive.push_back(hand->GetSessionID());
//				hand->Update(hand_centroid);
//				length = (float)cvArcLength( c );
//
//				hand->Clear();
//				CvPoint           pt;
//				CvSeqReader       reader;
//				cvStartReadSeq( c, &reader, 0 );
//    			for( int j=0; j < c->total; j++ )
//    			{
//					CV_READ_SEQ_ELEM( pt, reader );
//					hand->AddVertex(pt.x, pt.y);
//				}
//
//				///retreive the hull path
//				int             hullcount = hull->total;
//				float          aa, bb;
//				CvPoint         pt0;
//				CvRect rect	= cvBoundingRect( c, 0 );
//				for(int j = 0; j < hullcount; j++)
//				{
//					pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, j );
//					if(j == hullcount-1)
//						pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, 0 );
//					else
//						pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, j+1 );
//					aa = (float)pt0.x - (float)pt.x;
//					bb = (float)pt0.y - (float)pt.y;
//					if (  sqrtf ( (aa*aa) + (bb*bb) ) > (float)rect.width / 10.0)
//						hand->AddVertexConvex(pt0.x, pt0.y);
//				}
//
//				hand->ComputeHand(area, length);
//				hand->draw();
//
//				//compute hand holes-->pinching gestures
//				CvSeq* c_vnext = c->v_next;
//				CvSeq* candidate = NULL;
//				double max_area=0;
//				if( c_vnext != NULL)
//				{
//					for(CvSeq* h=c_vnext;h!=NULL;h=h->h_next)
//					{
//						area = (float)fabs ( cvContourArea(h,CV_WHOLE_SEQ));
//						if(max_area < area)
//						{
//							max_area = area;
//							candidate = h;
//						}
//					}
//				}
//
//				if ( max_area > min_pinch_blob_size)
//				{
//					hand->SetPinch(candidate);
//				}
//				else
//					hand->SetPinch(NULL);
//			}
//		}
//	}
//	return main_processed_image;
	
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
//	to_remove.clear();
//	for(std::map<unsigned long, Hand*>::iterator it = hands.begin(); it != hands.end(); it++)
//	{
//		if(it->second->IsUpdated())
//		{
//			//send OSC MEssage
//			TuioServer::Instance().AddHand(
//				it->first,
//				it->second->IsConfirmedAsHand(),
//				it->second->IsOpened(),
//				it->second->TCentroidX(), //Globals::GetX(it->second->TCentroidX()),//it->second->TCentroidX(), 
//				it->second->TCentroidY(), //Globals::GetY(it->second->TCentroidY()),//it->second->TCentroidY(), 
//				it->second->GetArea());		
//			
//			TuioServer::Instance().AddHandPath(it->first,it->second->vertexs);
//			if(it->second->IsPinching() || it->second->IsPinchingEnd())
//			{
//				TuioServer::Instance().AddHandPinch(it->first,it->second->hand_hole);
//			}
//		}
//		else
//		{
//			to_remove.push_back(it->first);
//		}
//	}
//
//	for( std::vector<unsigned long>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
//	{
//		hands.erase(*it);
//	}
}

//
//std::map<unsigned long, Hand*>* HandFinder::GetHands()
//{
//	return &hands;
//}

void HandFinder::BuildGui(bool force)
{
	cv::createTrackbar("Enable", name,&enable_processor, 1, NULL);
	cv::createTrackbar("th.value", name,&Threshold_value, 255, NULL);
	/*cv::createTrackbar("Enable", name,&enable_processor, 1, NULL);
	cv::createTrackbar("Use Adaptive", name,&use_adaptive_bar_value, 1, NULL);
	if(use_adaptive_bar_value == 1)
	{
		cv::createTrackbar("block_size", name,&block_size, 255, NULL);
		cv::createTrackbar("C", name,&threshold_C, 40, NULL);
	}
	else
	{
		cv::createTrackbar("th.value", name,&Threshold_value, 255, NULL);
	}*/
}
