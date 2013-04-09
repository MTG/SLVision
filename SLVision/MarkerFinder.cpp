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
#include "TuioServer.h"
#include "GlobalConfig.h"
#include <iostream>


MarkerFinder::MarkerFinder():FrameProcessor("6DoF MarkerFinder")
{
	//ssidGenerator = 1;
	fiducial_finder = new FiducialFinder(FIDUCIAL_IMAGE_SIZE);
	firstcontour=NULL;
	polycontour=NULL;
	InitFrames(Globals::screen);
	InitGeometry();

	int cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:enable",1);
	if(cf_enabled == 1) Enable(true);
	else Enable(false);

	int cf_adaptive_threshold = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:adaptive_threshold:enable",1);
	if(cf_adaptive_threshold == 1) use_adaptive_threshold = true;
	else use_adaptive_threshold = false;

	adaptive_block_size = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:adaptive_threshold:block_size",55);
	
	threshold_value = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:threshold_value",100);
	
	if((int)datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:invert_rotation_matrix",1))
		invert_rotation_matrix = true;
	else
		invert_rotation_matrix = false;
	//populate gui
	
	guiMenu->AddBar("0-Enable",0,1,1);
	guiMenu->AddBar("1-Threshold",0,255,1);
	guiMenu->AddBar("2-Enable_adaptive_threshold",0,1,1);
	guiMenu->AddBar("3-Adaptive_threshold_block_size",3,101,4);
	guiMenu->AddBar("4-Invert_rotation_matrix",0,1,1);

	guiMenu->SetValue("0-Enable",(float)cf_enabled);
	guiMenu->SetValue("1-Threshold",(float)threshold_value);
	guiMenu->SetValue("2-Enable_adaptive_threshold",(float)use_adaptive_threshold);
	guiMenu->SetValue("3-Adaptive_threshold_block_size",(float)adaptive_block_size);
	guiMenu->SetValue("4-Invert_rotation_matrix",(float)(int)datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:invert_rotation_matrix",1));

}

void MarkerFinder::UpdatedValuesFromGui()
{
	int &cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:enable",1);
	int &cf_threshold = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:threshold_value",100);
	int &cf_adaptive_threshold = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:adaptive_threshold:enable",1);
	int &cf_adaptive_block_size = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:adaptive_threshold:block_size",55);
	int &cf_invert_rotation_matrix = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:invert_rotation_matrix",1);

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

	if(guiMenu->GetValue("2-Enable_adaptive_threshold") == 1 ) 
		use_adaptive_threshold = true;
	else
		use_adaptive_threshold = false;
	//use_adaptive_threshold = (int)ceil();
	cf_adaptive_threshold = use_adaptive_threshold;

	adaptive_block_size =  (int)ceil(guiMenu->GetValue("3-Adaptive_threshold_block_size"));
	cf_adaptive_block_size = adaptive_block_size;

	if(guiMenu->GetValue("4-Invert_rotation_matrix") == 1)
	{
		cf_invert_rotation_matrix = 1;
		invert_rotation_matrix = true;
	}
	else
	{
		cf_invert_rotation_matrix = 0;
		invert_rotation_matrix = false;
	}
}

MarkerFinder::~MarkerFinder(void)
{
	cvReleaseImage(&main_processed_image);	
	cvReleaseImage(&main_processed_contour);
	cvReleaseImage(&fiducial_image);	
	cvReleaseMat (&map_matrix);
	cvReleaseMemStorage(&main_storage_poligon);
	cvReleaseMemStorage(&main_storage);
	delete(fiducial_finder);
}

void MarkerFinder::InitGeometry()
{
/*	Globals::intrinsic = (CvMat*)cvLoad(M_PATH_INTRINSIC);
	Globals::distortion = (CvMat*)cvLoad(M_PATH_DISTORTION);
	if(Globals::intrinsic == NULL || Globals::distortion == NULL) Globals::LoadDefaultDistortionMatrix();
*/
	
	/*rotation = cvCreateMat (1, 3, CV_32FC1);
	rotationMatrix = cvCreateMat (3, 3, CV_32FC1);
	translation = cvCreateMat (1 , 3, CV_32FC1);
	
	srcPoints3D = cvCreateMat (4, 1, CV_32FC3);
	dstPoints2D = cvCreateMat (4, 1, CV_32FC3);

	baseMarkerPoints[0].x =(float) 0 * datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
	baseMarkerPoints[0].y =(float) 0 * datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
	baseMarkerPoints[0].z = 0.0;
		
	baseMarkerPoints[1].x =(float) 0 * datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
	baseMarkerPoints[1].y =(float) 1 * datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
	baseMarkerPoints[1].z = 0.0;

	baseMarkerPoints[2].x =(float) 1 * datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
	baseMarkerPoints[2].y =(float) 1 * datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
	baseMarkerPoints[2].z = 0.0;
 
	baseMarkerPoints[3].x =(float) 1 * datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
	baseMarkerPoints[3].y =(float) 0 * datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
	baseMarkerPoints[3].z = 0.0;

	for ( i=0;i<4;i++)
	{	
		switch (i)
		{
			case 0:	srcPoints3D->data.fl[0]     =0;
				    srcPoints3D->data.fl[1]     =0;
					srcPoints3D->data.fl[2]     =0;
					break;
			case 1:	srcPoints3D->data.fl[0+i*3] =(float)datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
					srcPoints3D->data.fl[1+i*3] =0;
					srcPoints3D->data.fl[2+i*3] =0;
					break;
			case 2:	srcPoints3D->data.fl[0+i*3] =0;
					srcPoints3D->data.fl[1+i*3] =(float)datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);
					srcPoints3D->data.fl[2+i*3] =0;
					break;
			case 3:	srcPoints3D->data.fl[0+i*3] =0;
					srcPoints3D->data.fl[1+i*3] =0;
					srcPoints3D->data.fl[2+i*3] =-(float)datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:FiducialSize[mm]",90);;
					break;
		
		}
	}*/

	//std::cout << "size: " << fiducial_image->width << "  " << fiducial_image->height << std::endl;
	dst_pnt[0] = cvPoint2D32f (0, 0);
	dst_pnt[1] = cvPoint2D32f (fiducial_image->width, 0);
	dst_pnt[2] = cvPoint2D32f (fiducial_image->width, fiducial_image->height);
    dst_pnt[3] = cvPoint2D32f (0, fiducial_image->height);
	map_matrix = cvCreateMat  (3, 3, CV_32FC1);
}

void MarkerFinder::InitFrames(IplImage*	main_image)
{
	blob_moments = (CvMoments*)malloc( sizeof(CvMoments) );
	main_processed_image = cvCreateImage(cvGetSize(main_image),IPL_DEPTH_8U,1);		//allocates 1-channel memory frame for the image
	main_processed_contour = cvCreateImage(cvGetSize(main_image),IPL_DEPTH_8U,1);	//allocates 1-channel memory frame for the image
	//contour data allocation
	main_storage = cvCreateMemStorage (0);											//Creates a memory main_storage and returns pointer to it /param:/Size of the main_storage blocks in bytes. If it is 0, the block size is set to default value - currently it is 64K.
	main_storage_poligon = cvCreateMemStorage (0);
	//fiducial frame allocation
	fiducial_image = cvCreateImage(cvSize(FIDUCIAL_IMAGE_SIZE,FIDUCIAL_IMAGE_SIZE),IPL_DEPTH_8U,1);
	fiducial_image_zoomed = cvCreateImage(cvSize(fiducial_image->width*2,fiducial_image->height*2),IPL_DEPTH_8U,1);
}


IplImage* MarkerFinder::Process(IplImage*	main_image)
{
	
	cvClearMemStorage(main_storage);
	cvClearMemStorage(main_storage_poligon);

	if(use_adaptive_threshold)
		cvAdaptiveThreshold(main_image,main_processed_image,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,adaptive_block_size,2);//CV_ADAPTIVE_THRESH_MEAN_C
	else 
		cvThreshold(main_image,main_processed_image,threshold_value,255, CV_THRESH_BINARY);

	cvNot(main_processed_image,main_processed_image); //invert colors

	cvCopy(main_processed_image,main_processed_contour);	
	cvFindContours (main_processed_contour, main_storage, &firstcontour, sizeof (CvContour), CV_RETR_CCOMP);
	//find squares
	polycontour=cvApproxPoly(firstcontour,sizeof(CvContour),main_storage_poligon,CV_POLY_APPROX_DP,3,1);
	
	for(CvSeq* c=polycontour;c!=NULL;c=c->h_next)
	{
		if((cvContourPerimeter(c)<2000)&&(cvContourPerimeter(c)>60)&&(c->total==4))
		{
			//if(Globals::is_view_enabled)cvDrawContours(Globals::screen,c,CV_RGB(255,0,0),CV_RGB(200,255,255),0);
			if(c->v_next!=NULL && (c->v_next->total==4))
			{
				CvSeq* c_vnext=c->v_next;
				cvMoments( c, blob_moments );
				//inside square processing
				float xlist[4];
				float ylist[4];

				float exlist[4];
				float eylist[4];
				for(int n=0;n<4;n++)
				{
					CvPoint* p=CV_GET_SEQ_ELEM(CvPoint,c->v_next,n);
					tmp_pnt[n].x=(float)p->x;
					tmp_pnt[n].y=(float)p->y;	
					xlist[n]=(float)p->x;
					ylist[n]=(float)p->y;

					CvPoint* ep = CV_GET_SEQ_ELEM(CvPoint,c,n);
					exlist[n]=(float)ep->x;
					eylist[n]=(float)ep->y;
				}

				


				temporal.clear();
				//float area = fabs(cvContourArea( c, CV_WHOLE_SEQ ));
				//x = (float)(fiducial_blob_moments->m10 / fiducial_blob_moments->m00);
				//y = (float)(fiducial_blob_moments->m01 / fiducial_blob_moments->m00);
				temporal.Update((float)(blob_moments->m10 / blob_moments->m00),(float)(blob_moments->m01 / blob_moments->m00),
								cvPoint((int)xlist[0],(int)ylist[0]),
								cvPoint((int)xlist[1],(int)ylist[1]),
								cvPoint((int)xlist[2],(int)ylist[2]),
								cvPoint((int)xlist[3],(int)ylist[3]),
								(float)fabs(cvContourArea( c, CV_WHOLE_SEQ )),0 );

				to_process = true;
				tmp_ssid = 0;
				minimum_distance = 99999999.0f;
				for(FiducialMap::iterator it = fiducial_map.begin(); it!= fiducial_map.end(); it++)
				{
					if( it->second->CanUpdate(temporal,minimum_distance) )
					{
						tmp_ssid = it->first;
					}
					else if( it->second->Is_inside(temporal) )
					{
						to_process = false;
						break;
					}
				}

				if(to_process)
				{
					if(Globals::is_view_enabled)cvDrawContours(Globals::screen,c,CV_RGB(255,255,0),CV_RGB(200,255,255),0);
					if(Globals::is_view_enabled)cvDrawContours(Globals::screen,c_vnext,CV_RGB(255,0,0),CV_RGB(0,255,255),0);
					
					cvGetPerspectiveTransform (tmp_pnt, dst_pnt, map_matrix);
					cvWarpPerspective (main_processed_image, fiducial_image, map_matrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll (0));

					int maxCount=0;
					int markerDirection=0;
					cvResize(fiducial_image,fiducial_image_zoomed);

					fiducial_finder->DecodeFiducial(fiducial_image, temporal);
					//notzeroCount=cvCountNonZero(tempmask);

					if(tmp_ssid == 0)
					{
						if(temporal.GetFiducialID() == -1) break;
						//add fiducial
						//fiducial_map[ssidGenerator++] = new Fiducial(temporal);
						fiducial_map[Globals::ssidGenerator++] = new Fiducial(temporal);
						tmp_ssid = Globals::ssidGenerator-1;
					}
					else if( tmp_ssid > 0)
					{
						fiducial_map[tmp_ssid]->Update(temporal);
					}



#ifdef ENABLEPOSE
					
					markerDirection = fiducial_map[tmp_ssid]->GetOrientation();
					if(markerDirection==0)
					{
						src_pnt[0].x = fiducial_map[tmp_ssid]->a.x;		
						src_pnt[1].x = fiducial_map[tmp_ssid]->d.x;
						src_pnt[2].x = fiducial_map[tmp_ssid]->c.x;
						src_pnt[3].x = fiducial_map[tmp_ssid]->b.x;

						src_pnt[0].y = fiducial_map[tmp_ssid]->a.y;
						src_pnt[1].y = fiducial_map[tmp_ssid]->d.y;
						src_pnt[2].y = fiducial_map[tmp_ssid]->c.y;
						src_pnt[3].y = fiducial_map[tmp_ssid]->b.y;
#ifdef USEEIGHTPOINTS
						src_pnt[4].x = exlist[0];		
						src_pnt[5].x = exlist[3];
						src_pnt[6].x = exlist[2];
						src_pnt[7].x = exlist[1];

						src_pnt[4].y = eylist[0];
						src_pnt[5].y = eylist[3];;
						src_pnt[6].y = eylist[2];;
						src_pnt[7].y = eylist[1];;
#endif
					}
					else if(markerDirection==1)//90
					{
						src_pnt[0].x = fiducial_map[tmp_ssid]->b.x;		
						src_pnt[1].x = fiducial_map[tmp_ssid]->a.x;
						src_pnt[2].x = fiducial_map[tmp_ssid]->d.x;
						src_pnt[3].x = fiducial_map[tmp_ssid]->c.x;

						src_pnt[0].y = fiducial_map[tmp_ssid]->b.y;
						src_pnt[1].y = fiducial_map[tmp_ssid]->a.y;
						src_pnt[2].y = fiducial_map[tmp_ssid]->d.y;
						src_pnt[3].y = fiducial_map[tmp_ssid]->c.y;
#ifdef USEEIGHTPOINTS
						src_pnt[4].x = exlist[1];		
						src_pnt[5].x = exlist[0];
						src_pnt[6].x = exlist[3];
						src_pnt[7].x = exlist[2];

						src_pnt[4].y = eylist[1];
						src_pnt[5].y = eylist[0];
						src_pnt[6].y = eylist[3];
						src_pnt[7].y = eylist[2];
#endif
					}
					else if(markerDirection==3)//180
					{
						src_pnt[0].x = fiducial_map[tmp_ssid]->c.x;		
						src_pnt[1].x = fiducial_map[tmp_ssid]->b.x;
						src_pnt[2].x = fiducial_map[tmp_ssid]->a.x;
						src_pnt[3].x = fiducial_map[tmp_ssid]->d.x;

						src_pnt[0].y = fiducial_map[tmp_ssid]->c.y;
						src_pnt[1].y = fiducial_map[tmp_ssid]->b.y;
						src_pnt[2].y = fiducial_map[tmp_ssid]->a.y;
						src_pnt[3].y = fiducial_map[tmp_ssid]->d.y;
#ifdef USEEIGHTPOINTS
						src_pnt[4].x = exlist[2];		
						src_pnt[5].x = exlist[1];
						src_pnt[6].x = exlist[0];
						src_pnt[7].x = exlist[3];

						src_pnt[4].y = eylist[2];
						src_pnt[5].y = eylist[1];
						src_pnt[6].y = eylist[0];
						src_pnt[7].y = eylist[3];
#endif
					}
					else if(markerDirection==2)//270
					{
						src_pnt[0].x = fiducial_map[tmp_ssid]->d.x;		
						src_pnt[1].x = fiducial_map[tmp_ssid]->c.x;
						src_pnt[2].x = fiducial_map[tmp_ssid]->b.x;
						src_pnt[3].x = fiducial_map[tmp_ssid]->a.x;

						src_pnt[0].y = fiducial_map[tmp_ssid]->d.y;
						src_pnt[1].y = fiducial_map[tmp_ssid]->c.y;
						src_pnt[2].y = fiducial_map[tmp_ssid]->b.y;
						src_pnt[3].y = fiducial_map[tmp_ssid]->a.y;
#ifdef USEEIGHTPOINTS
						src_pnt[4].x = exlist[3];		
						src_pnt[5].x = exlist[2];
						src_pnt[6].x = exlist[1];
						src_pnt[7].x = exlist[0];

						src_pnt[4].y = eylist[3];
						src_pnt[5].y = eylist[2];
						src_pnt[6].y = eylist[1];
						src_pnt[7].y = eylist[0];
#endif
					}
					
					cvInitMatHeader (&image_points, 4, 1, CV_32FC2, src_pnt);
					cvInitMatHeader (&object_points, 4, 3, CV_32FC1, baseMarkerPoints);

					CvPoint a;
					a.x = src_pnt[0].x; a.y = src_pnt[0].y;
					Globals::Font::Write(Globals::screen,"a",a,FONT_AXIS,255,0,0);
					a.x = src_pnt[1].x; a.y = src_pnt[1].y;
					Globals::Font::Write(Globals::screen,"b",a,FONT_AXIS,255,0,0);
					a.x = src_pnt[2].x; a.y = src_pnt[2].y;
					Globals::Font::Write(Globals::screen,"c",a,FONT_AXIS,255,0,0);
					a.x = src_pnt[3].x; a.y = src_pnt[3].y;
					Globals::Font::Write(Globals::screen,"d",a,FONT_AXIS,255,0,0);

#ifdef USEEIGHTPOINTS
					a.x = src_pnt[4].x; a.y = src_pnt[4].y;
					Globals::Font::Write(Globals::screen,"a",a,FONT_AXIS,255,255,0);
					a.x = src_pnt[5].x; a.y = src_pnt[5].y;
					Globals::Font::Write(Globals::screen,"b",a,FONT_AXIS,255,255,0);
					a.x = src_pnt[6].x; a.y = src_pnt[6].y;
					Globals::Font::Write(Globals::screen,"c",a,FONT_AXIS,255,255,0);
					a.x = src_pnt[7].x; a.y = src_pnt[7].y;
					Globals::Font::Write(Globals::screen,"d",a,FONT_AXIS,255,255,0);
#endif
					/*for (int i = 0; i < 4; i++)
					{
						a.x = exlist[i]; a.y = eylist[i];
						Globals::Font::Write(Globals::screen,"o",a,FONT_AXIS,255,0,0);
					}*/
					

					/*************************************
					* Find extrinsic fiducial params
					**************************************/
					int fidsize = 5;
					double halfSize=fidsize;///2.;

#ifdef USEEIGHTPOINTS
					cv::Mat ObjPoints(8,3,CV_32FC1);
#else
					cv::Mat ObjPoints(4,3,CV_32FC1);
#endif
					ObjPoints.at<float>(0,0)=0;
					ObjPoints.at<float>(0,1)=0;
					ObjPoints.at<float>(0,2)=0;
					ObjPoints.at<float>(1,0)=0;
					ObjPoints.at<float>(1,1)=halfSize;
					ObjPoints.at<float>(1,2)=0;
					ObjPoints.at<float>(2,0)=halfSize;
					ObjPoints.at<float>(2,1)=halfSize;
					ObjPoints.at<float>(2,2)=0;
					ObjPoints.at<float>(3,0)=halfSize;
					ObjPoints.at<float>(3,1)=0;
					ObjPoints.at<float>(3,2)=0;
					/*ObjPoints.at<float>(1,0)=-halfSize;
					ObjPoints.at<float>(1,1)=halfSize;
					ObjPoints.at<float>(1,2)=0;
					ObjPoints.at<float>(2,0)=halfSize;
					ObjPoints.at<float>(2,1)=halfSize;
					ObjPoints.at<float>(2,2)=0;
					ObjPoints.at<float>(3,0)=halfSize;
					ObjPoints.at<float>(3,1)=-halfSize;
					ObjPoints.at<float>(3,2)=0;
					ObjPoints.at<float>(0,0)=-halfSize;
					ObjPoints.at<float>(0,1)=-halfSize;
					ObjPoints.at<float>(0,2)=0;*/
#ifdef USEEIGHTPOINTS
					ObjPoints.at<float>(4,0)=-0.5f;
					ObjPoints.at<float>(4,1)=-0.5f;
					ObjPoints.at<float>(4,2)=0;
					ObjPoints.at<float>(5,0)=-0.5f;
					ObjPoints.at<float>(5,1)=halfSize+0.5f;
					ObjPoints.at<float>(5,2)=0;
					ObjPoints.at<float>(6,0)=halfSize+0.5f;
					ObjPoints.at<float>(6,1)=halfSize+0.5f;
					ObjPoints.at<float>(6,2)=0;
					ObjPoints.at<float>(7,0)=halfSize+0.5f;
					ObjPoints.at<float>(7,1)=-0.5f;
					ObjPoints.at<float>(7,2)=0;
#endif

#ifdef USEEIGHTPOINTS
					cv::Mat ImagePoints(8,2,CV_32FC1);
					int numpoints = 8;
#else
					cv::Mat ImagePoints(4,2,CV_32FC1);
					int numpoints = 4;
#endif
					for (int c=0;c<numpoints;c++)
					{
						ImagePoints.at<float>(c,0)=(src_pnt[c].x);
						ImagePoints.at<float>(c,1)=(src_pnt[c].y);
					}

					cv::Mat raux,taux;
					cv::solvePnP(ObjPoints, ImagePoints, Globals::CameraMatrix, Globals::Distortion,raux,taux);
					raux.convertTo(Rvec,CV_32F);
					taux.convertTo(Tvec ,CV_32F);
#endif

#ifdef ENABLEPOSE
					/*************************************
					* Prepare parameters
					**************************************/
					cv::Mat R(3,3,CV_32F);
					Rodrigues(Rvec, R);
					fiducial_map[tmp_ssid]->yaw = atan2(-R.ptr<float>(2)[0],sqrt( R.ptr<float>(2)[1]*R.ptr<float>(2)[1] + R.ptr<float>(2)[2]*R.ptr<float>(2)[2]));
					fiducial_map[tmp_ssid]->pitch = atan2(R.ptr<float>(2)[1],R.ptr<float>(2)[2]);
					fiducial_map[tmp_ssid]->roll = (2.0f*3.141592654f)-atan2(R.ptr<float>(1)[0],R.ptr<float>(0)[0]);

					fiducial_map[tmp_ssid]->xpos = Tvec.ptr<float>(0)[0];
					fiducial_map[tmp_ssid]->ypos = Tvec.ptr<float>(0)[1];
					fiducial_map[tmp_ssid]->zpos = Tvec.ptr<float>(0)[2];

					fiducial_map[tmp_ssid]->r11 = R.ptr<float>(0)[0];
					fiducial_map[tmp_ssid]->r12 = R.ptr<float>(0)[1];
					fiducial_map[tmp_ssid]->r13 = R.ptr<float>(0)[2];
					fiducial_map[tmp_ssid]->r21 = R.ptr<float>(1)[0];
					fiducial_map[tmp_ssid]->r22 = R.ptr<float>(1)[1];
					fiducial_map[tmp_ssid]->r23 = R.ptr<float>(1)[2];
					fiducial_map[tmp_ssid]->r31 = R.ptr<float>(2)[0];
					fiducial_map[tmp_ssid]->r32 = R.ptr<float>(2)[1];
					fiducial_map[tmp_ssid]->r33 = R.ptr<float>(2)[2];
#endif
					if(Globals::is_view_enabled)
					{
#ifdef DRAWPOSE
					/*************************************
					* Draw pose params  cube
					**************************************/
					cv::Mat objectPoints (8,3,CV_32FC1);
					halfSize=fidsize/2;
					objectPoints.at<float>(0,0)=-halfSize;
					objectPoints.at<float>(0,1)=0;
					objectPoints.at<float>(0,2)=-halfSize;
					objectPoints.at<float>(1,0)=halfSize;
					objectPoints.at<float>(1,1)=0;
					objectPoints.at<float>(1,2)=-halfSize;
					objectPoints.at<float>(2,0)=halfSize;
					objectPoints.at<float>(2,1)=0;
					objectPoints.at<float>(2,2)=halfSize;
					objectPoints.at<float>(3,0)=-halfSize;
					objectPoints.at<float>(3,1)=0;
					objectPoints.at<float>(3,2)=halfSize;

					objectPoints.at<float>(4,0)=-halfSize;
					objectPoints.at<float>(4,1)=fidsize;
					objectPoints.at<float>(4,2)=-halfSize;
					objectPoints.at<float>(5,0)=halfSize;
					objectPoints.at<float>(5,1)=fidsize;
					objectPoints.at<float>(5,2)=-halfSize;
					objectPoints.at<float>(6,0)=halfSize;
					objectPoints.at<float>(6,1)=fidsize;
					objectPoints.at<float>(6,2)=halfSize;
					objectPoints.at<float>(7,0)=-halfSize;
					objectPoints.at<float>(7,1)=fidsize;
					objectPoints.at<float>(7,2)=halfSize;
					cv::vector<cv::Point2f> imagePoints;

					projectPoints( objectPoints, Rvec, Tvec,  Globals::CameraMatrix,Globals::Distortion,   imagePoints);
					
					//draw lines of different colours
					for (int i=0;i<4;i++)
						cvLine(Globals::screen,imagePoints[i],imagePoints[(i+1)%4],CV_RGB(255,255,0),2,8,0);

					for (int i=0;i<4;i++)
						cvLine(Globals::screen,imagePoints[i+4],imagePoints[4+(i+1)%4],CV_RGB(255,0,255),2,8,0);

					for (int i=0;i<4;i++)
						cvLine(Globals::screen,imagePoints[i],imagePoints[i+4],CV_RGB(0,255,255),2,8,0);
					/*************************************
					* Draw pose params  axis
					**************************************/
					{
					float size=fidsize*3;
					cv::Mat objectPoints (4,3,CV_32FC1);
					objectPoints.at<float>(0,0)=0;
					objectPoints.at<float>(0,1)=0;
					objectPoints.at<float>(0,2)=0;
					objectPoints.at<float>(1,0)=size;
					objectPoints.at<float>(1,1)=0;
					objectPoints.at<float>(1,2)=0;
					objectPoints.at<float>(2,0)=0;
					objectPoints.at<float>(2,1)=size;
					objectPoints.at<float>(2,2)=0;
					objectPoints.at<float>(3,0)=0;
					objectPoints.at<float>(3,1)=0;
					objectPoints.at<float>(3,2)=size;

					cv::vector<cv::Point2f> imagePoints;
					cv::projectPoints( objectPoints, Rvec,Tvec, Globals::CameraMatrix,Globals::Distortion,   imagePoints);
					//draw lines of different colours
					cvLine(Globals::screen,imagePoints[0],imagePoints[1],CV_RGB(0,0,255),2,8,0);
					Globals::Font::Write(Globals::screen,"X",imagePoints[1],FONT_AXIS,0,0,255);
					cvLine(Globals::screen,imagePoints[0],imagePoints[2],CV_RGB(0,255,0),2,8,0);
					Globals::Font::Write(Globals::screen,"Y",imagePoints[2],FONT_AXIS,0,255,0);
					cvLine(Globals::screen,imagePoints[0],imagePoints[3],CV_RGB(255,0,0),2,8,0);
					Globals::Font::Write(Globals::screen,"Z",imagePoints[3],FONT_AXIS,255,0,0);
					}
					#endif
					}
					
				}
			}
		}
	}
	return main_processed_image;
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
	//std::vector<unsigned long>
	return to_return;
}

void MarkerFinder::KeyInput(char key)
{
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
				Globals::GetX(it->second->xpos),//it->second->xpos,
				Globals::GetY(it->second->ypos),//it->second->ypos,
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
		if(Globals::is_view_enabled)
		{
			sprintf_s(text,"%i , %i",it->second->GetFiducialID(), it->first); 
			Globals::Font::Write(Globals::screen,text,cvPoint((int)it->second->GetX(), (int)it->second->GetY()),FONT_HELP,0,255,0);
		}
	}

	for(std::vector<unsigned int>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
	{
		fiducial_map.erase(*it);
	}
}