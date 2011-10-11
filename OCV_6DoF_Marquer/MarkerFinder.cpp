////#include "StdAfx.h"
#include "MarkerFinder.h"
#include "TuioServer.h"
#include "GlobalConfig.h"
#include <iostream>

MarkerFinder::MarkerFinder():FrameProcessor("6DoF MarkerFinder")
{
	ssidGenerator = 1;
	fiducial_finder = new FiducialFinder(FIDUCIAL_IMAGE_SIZE);
	firstcontour=NULL;
	polycontour=NULL;
	InitFrames(Globals::screen);
	InitGeometry();

	int cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:enable",1);
	if(cf_enabled == 1) Enable(true);
	else Enable(false);
	
	threshold_value = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:threshold_value",100);

	//populate gui
	guiMenu->AddBar("Threshold",0,255,1);
	guiMenu->AddBar("Enable",0,1,1);
	guiMenu->SetValue("Enable",(float)cf_enabled);
	guiMenu->SetValue("Threshold",(float)threshold_value);

}

void MarkerFinder::UpdatedValuesFromGui()
{
	int &cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:enable",1);
	int &cf_threshold = datasaver::GlobalConfig::getRef("FrameProcessor:6DoF_fiducial_finder:threshold_value",100);
	if(guiMenu->GetValue("Enable") == 0)
	{
		Enable(false);
		cf_enabled = 0;
	}
	else if(guiMenu->GetValue("Enable") == 1) 
	{
		Enable(true);
		cf_enabled = 1;
	}	 
	threshold_value = (int)ceil(guiMenu->GetValue("Threshold"));
	cf_threshold = threshold_value;
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
	Globals::intrinsic = (CvMat*)cvLoad(M_PATH_INTRINSIC);
	Globals::distortion = (CvMat*)cvLoad(M_PATH_DISTORTION);
	if(Globals::intrinsic == NULL || Globals::distortion == NULL) Globals::LoadDefaultDistortionMatrix();

	rotation = cvCreateMat (1, 3, CV_32FC1);
	rotationMatrix = cvCreateMat (3, 3, CV_32FC1);
	translation = cvCreateMat (1 , 3, CV_32FC1);
	
	srcPoints3D = cvCreateMat (4, 1, CV_32FC3);
	dstPoints2D = cvCreateMat (4, 1, CV_32FC3);

	baseMarkerPoints[0].x =(float) 0 * MARKER_SIZE;
	baseMarkerPoints[0].y =(float) 0 * MARKER_SIZE;
	baseMarkerPoints[0].z = 0.0;
		
	baseMarkerPoints[1].x =(float) 0 * MARKER_SIZE;
	baseMarkerPoints[1].y =(float) 1 * MARKER_SIZE;
	baseMarkerPoints[1].z = 0.0;

	baseMarkerPoints[2].x =(float) 1 * MARKER_SIZE;
	baseMarkerPoints[2].y =(float) 1 * MARKER_SIZE;
	baseMarkerPoints[2].z = 0.0;
 
	baseMarkerPoints[3].x =(float) 1 * MARKER_SIZE;
	baseMarkerPoints[3].y =(float) 0 * MARKER_SIZE;
	baseMarkerPoints[3].z = 0.0;

	for ( i=0;i<4;i++)
	{	
		switch (i)
		{
			case 0:	srcPoints3D->data.fl[0]     =0;
				    srcPoints3D->data.fl[1]     =0;
					srcPoints3D->data.fl[2]     =0;
					break;
			case 1:	srcPoints3D->data.fl[0+i*3] =(float)MARKER_SIZE;
					srcPoints3D->data.fl[1+i*3] =0;
					srcPoints3D->data.fl[2+i*3] =0;
					break;
			case 2:	srcPoints3D->data.fl[0+i*3] =0;
					srcPoints3D->data.fl[1+i*3] =(float)MARKER_SIZE;
					srcPoints3D->data.fl[2+i*3] =0;
					break;
			case 3:	srcPoints3D->data.fl[0+i*3] =0;
					srcPoints3D->data.fl[1+i*3] =0;
					srcPoints3D->data.fl[2+i*3] =-(float)MARKER_SIZE;;
					break;
		
		}
	}

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

	//cvAdaptiveThreshold(main_image,main_processed_image,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,55,2);//CV_ADAPTIVE_THRESH_MEAN_C
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
			if(c->v_next!=NULL && (c->v_next->total==4))
			{
				CvSeq* c_vnext=c->v_next;
				cvMoments( c, blob_moments );
				//inside square processing
				float xlist[4];
				float ylist[4];
				for(int n=0;n<4;n++)
				{
					CvPoint* p=CV_GET_SEQ_ELEM(CvPoint,c->v_next,n);
					tmp_pnt[n].x=(float)p->x;
					tmp_pnt[n].y=(float)p->y;	
					xlist[n]=(float)p->x;
					ylist[n]=(float)p->y;
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

							//int notzeroCount=0;
					int maxCount=0;
					int markerDirection=0;
					cvResize(fiducial_image,fiducial_image_zoomed);
					fiducial_finder->DecodeFiducial(fiducial_image, temporal);
					//notzeroCount=cvCountNonZero(tempmask);

					//std::cout << "temp_ssid " << tmp_ssid << "  ssidGenerator " << ssidGenerator << std::endl;
					if(tmp_ssid == 0)
					{
						if(temporal.GetFiducialID() == -1) break;
						//add fiducial
						fiducial_map[ssidGenerator++] = new Fiducial(temporal);
						tmp_ssid = ssidGenerator-1;
//std::cout << "new" << std::endl;
					}
					else if( tmp_ssid > 0)
					{
						fiducial_map[tmp_ssid]->Update(temporal);
//std::cout << "update" << std::endl;
					}

					markerDirection = fiducial_map[tmp_ssid]->GetOrientation();
					
					if(markerDirection==0)
					{
						src_pnt[0].x = tmp_pnt[0].x;	src_pnt[1].x = tmp_pnt[3].x;	src_pnt[2].x = tmp_pnt[2].x;	src_pnt[3].x = tmp_pnt[1].x;
						src_pnt[0].y = tmp_pnt[0].y;	src_pnt[1].y = tmp_pnt[3].y;	src_pnt[2].y = tmp_pnt[2].y;	src_pnt[3].y = tmp_pnt[1].y;
					}
					else if(markerDirection==1)//90
					{
						src_pnt[0].x = tmp_pnt[1].x;	src_pnt[1].x = tmp_pnt[0].x;	src_pnt[2].x = tmp_pnt[3].x;	src_pnt[3].x = tmp_pnt[2].x;
						src_pnt[0].y = tmp_pnt[1].y;	src_pnt[1].y = tmp_pnt[0].y;	src_pnt[2].y = tmp_pnt[3].y;	src_pnt[3].y = tmp_pnt[2].y;
					}
					else if(markerDirection==3)//180
					{
						src_pnt[0].x = tmp_pnt[2].x;	src_pnt[1].x = tmp_pnt[1].x;	src_pnt[2].x = tmp_pnt[0].x;	src_pnt[3].x = tmp_pnt[3].x;
						src_pnt[0].y = tmp_pnt[2].y;	src_pnt[1].y = tmp_pnt[1].y;	src_pnt[2].y = tmp_pnt[0].y;	src_pnt[3].y = tmp_pnt[3].y;
					}
					else if(markerDirection==2)//270
					{
						src_pnt[0].x = tmp_pnt[3].x;	src_pnt[1].x = tmp_pnt[2].x;	src_pnt[2].x = tmp_pnt[1].x;	src_pnt[3].x = tmp_pnt[0].x;
						src_pnt[0].y = tmp_pnt[3].y;	src_pnt[1].y = tmp_pnt[2].y;	src_pnt[2].y = tmp_pnt[1].y;	src_pnt[3].y = tmp_pnt[0].y;
					}
					
					cvInitMatHeader (&image_points, 4, 1, CV_32FC2, src_pnt);
					cvInitMatHeader (&object_points, 4, 3, CV_32FC1, baseMarkerPoints);
					cvFindExtrinsicCameraParams2(&object_points,&image_points,Globals::intrinsic,Globals::distortion,rotation,translation);
					//to get rotation matrix /http://www.emgu.com/wiki/files/2.0.0.0/html/9c6a2a7e-e973-20d3-9638-954a4a0a80a6.htm
					cvProjectPoints2(srcPoints3D,rotation,translation,Globals::intrinsic,Globals::distortion,dstPoints2D);
					//
					//
					CV_MAT_ELEM( *rotation, float, 0, 0) = -rotation->data.fl[0];
					CV_MAT_ELEM( *rotation, float, 0, 1) = -rotation->data.fl[1];
					cvRodrigues2(rotation,rotationMatrix);
					fiducial_map[tmp_ssid]->yaw = atan2(rotationMatrix->data.fl[3],rotationMatrix->data.fl[0]); //atan2([1,0], [0,0])
					fiducial_map[tmp_ssid]->pitch = atan2(-rotationMatrix->data.fl[6],sqrt( rotationMatrix->data.fl[7]*rotationMatrix->data.fl[7] + rotationMatrix->data.fl[8]*rotationMatrix->data.fl[8])); //atan2([2,0], sqrt([2,1]'2 + [2,2]'2))
					fiducial_map[tmp_ssid]->roll = atan2(rotationMatrix->data.fl[7],rotationMatrix->data.fl[8]); //atan2([2,1], [2,2])
					fiducial_map[tmp_ssid]->xpos = ((translation->data.fl[0] + Globals::width )/ 2)/Globals::width;
					fiducial_map[tmp_ssid]->ypos = ((translation->data.fl[1] + Globals::height )/ 2)/Globals::height;
					fiducial_map[tmp_ssid]->zpos = translation->data.fl[2];
					
					//std::cout << 
					//	"yaw: " << fiducial_map[tmp_ssid]->yaw << std::endl<<
					//	"pitch: " << fiducial_map[tmp_ssid]->pitch << std::endl<<
					//	"roll: " << fiducial_map[tmp_ssid]->roll << std::endl;

					//xpos, ypos, zpos
					if(Globals::is_view_enabled)
					{
						CvPoint startpoint;
						CvPoint endpoint;
						startpoint=cvPoint((int)dstPoints2D->data.fl[0], (int)dstPoints2D->data.fl[1]);
						for(j=1;j<4;j++)
						{
							endpoint=  cvPoint((int)dstPoints2D->data.fl[(j)*3],(int)dstPoints2D->data.fl[1+(j)*3]);
							if(j==1)
							{
								cvLine(Globals::screen,startpoint,endpoint,CV_RGB(255,0,0),2,8,0);
								Globals::Font::Write(Globals::screen,"X",endpoint,FONT_AXIS,255,0,0);
							}
							if(j==2)
							{
								cvLine(Globals::screen,startpoint,endpoint,CV_RGB(0,255,0),2,8,0);
								Globals::Font::Write(Globals::screen,"Y",endpoint,FONT_AXIS,0,255,0);
							}
							if(j==3)
							{
								cvLine(Globals::screen,startpoint,endpoint,CV_RGB(0,0,255),2,8,0);
								Globals::Font::Write(Globals::screen,"Z",endpoint,FONT_AXIS,0,0,255);
							}
						}
					}
				}
			}
		}
	}

	to_remove.clear();
	for(FiducialMap::iterator it = fiducial_map.begin(); it!= fiducial_map.end(); it++)
	{
		if(it->second->IsUpdated())
		{
			TuioServer::Instance().Add3DObjectMessage(it->first,0,it->second->GetFiducialID(),it->second->xpos,it->second->ypos,it->second->zpos,it->second->yaw,it->second->pitch,it->second->roll);
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
		}
	}
	//std::vector<unsigned long>
	return to_return;
}

void MarkerFinder::KeyInput(char key)
{
}