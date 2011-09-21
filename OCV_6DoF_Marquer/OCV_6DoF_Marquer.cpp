// OCV_6DoF_Marquer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <map>
#include <vector>
#include "Fiducial.h"

#include "FiducialFinder.h"

//CAMERA constants
#define CAMERA_ID			0
//VIEW constants
#define VIEW_RAW			0
#define VIEW_THRESHOLD		1
#define VIEW_NONE			2
#define MAIN_TITTLE			"6dof reacTIVision"
#define FIDUCIAL_TITTLE		"Fiducial view"

#define MARKER_SIZE (90)

typedef std::map<unsigned int, Fiducial*> FiducialMap;


//Helping Functions
void SetView(int view);
void ToggleDisplayFIDProcessor();
//FIDUCIAL IMAGE Data
IplImage*		fiducial_image;
IplImage*		fiducial_image_zoomed;
//MAIN IMAGE Data
IplImage*		main_image;
IplImage*		main_processed_image;
IplImage*		main_processed_contour;
CvMemStorage*	main_storage;
CvMemStorage*	main_storage_poligon;
//CAMERA CAPTURE Data
IplImage*		captured_image;
CvCapture*		cv_camera_capture;

int screen_to_show; //0 normal, 1 threshold, 2 none
bool show_fid_processor;
bool is_view_enabled;
IplImage * screen;
double	process_time;
FiducialFinder* fiducial_finder;

unsigned int ssidGenerator;
FiducialMap fiducial_map;
CvMoments*  blob_moments;
unsigned int tmp_ssid;
bool to_process;
float minimum_distance;
std::vector<unsigned int> to_remove;

int _tmain(int argc, _TCHAR* argv[])
{	
	//pose
	/****/
	CvMat *intrinsic = (CvMat*)cvLoad("intrinsic.xml");
	CvMat *distortion = (CvMat*)cvLoad("distortion.xml");
	int i,j,k;

	CvMat object_points;
	CvMat image_points;
	CvMat point_counts;

	CvMat *rotation = cvCreateMat (1, 3, CV_32FC1);
	CvMat *translation = cvCreateMat (1 , 3, CV_32FC1);
	
	CvMat *srcPoints3D = cvCreateMat (4, 1, CV_32FC3);
	CvMat *dstPoints2D = cvCreateMat (4, 1, CV_32FC3);

	CvPoint3D32f baseMarkerPoints[4];

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

	/****/
	//
	ssidGenerator = 1;
	Fiducial temporal;
	blob_moments = (CvMoments*)malloc( sizeof(CvMoments) );
	//

	cv_camera_capture = NULL;
	char presskey;
	
	//Camera initialization
	cv_camera_capture = cvCaptureFromCAM(CAMERA_ID);								//allocates and initialized the CvCapture structure for reading a video stream from the camera
	if(cv_camera_capture == NULL) return -1;
	captured_image=cvQueryFrame(cv_camera_capture);									//grabs a frame from camera
	
	//main frame allocation
	main_image = cvCreateImage(cvGetSize(captured_image),IPL_DEPTH_8U,3);			//allocates 3-channel memory for the image
	cvCopy(captured_image,main_image);												//copy data into captured image
	main_processed_image = cvCreateImage(cvGetSize(main_image),IPL_DEPTH_8U,1);		//allocates 1-channel memory frame for the image
	main_processed_contour = cvCreateImage(cvGetSize(main_image),IPL_DEPTH_8U,1);	//allocates 1-channel memory frame for the image
	//contour data allocation
	main_storage = cvCreateMemStorage (0);											//Creates a memory main_storage and returns pointer to it /param:/Size of the main_storage blocks in bytes. If it is 0, the block size is set to default value - currently it is 64K.
	main_storage_poligon = cvCreateMemStorage (0);
	//fiducial frame allocation
	fiducial_image = cvCreateImage(cvSize(70,70),IPL_DEPTH_8U,1);
	fiducial_image_zoomed = cvCreateImage(cvSize(fiducial_image->width*2,fiducial_image->height*2),IPL_DEPTH_8U,1);

	fiducial_finder = new FiducialFinder(70);

	//Font initialization
	CvFont dfont;
    float hscale      = 0.5f;
    float vscale      = 0.5f;
    float italicscale = 0.0f;
    int  thickness    = 1;
    char text[255] = "";
    cvInitFont (&dfont, CV_FONT_HERSHEY_SIMPLEX , hscale, vscale, italicscale, thickness, CV_AA);
	//
	CvFont axisfont;
    float axhscale      = 0.8f;
    float axvscale      = 0.8f;
    cvInitFont (&axisfont, CV_FONT_HERSHEY_SIMPLEX , axhscale, axvscale, italicscale, thickness, CV_AA);

	
	//
	CvSeq *firstcontour=NULL;
	CvSeq *polycontour=NULL;
	
		
	CvMat *map_matrix;
	CvPoint2D32f src_pnt[4], dst_pnt[4], tmp_pnt[4];
	dst_pnt[0] = cvPoint2D32f (0, 0);
	dst_pnt[1] = cvPoint2D32f (fiducial_image->width, 0);
	dst_pnt[2] = cvPoint2D32f (fiducial_image->width, fiducial_image->height);
    dst_pnt[3] = cvPoint2D32f (0, fiducial_image->height);
	map_matrix = cvCreateMat (3, 3, CV_32FC1);

	//Window Setup
	cvNamedWindow (MAIN_TITTLE, CV_WINDOW_AUTOSIZE);
	SetView(VIEW_RAW);
	show_fid_processor = false;
	ToggleDisplayFIDProcessor();

	while(1)
	{
		process_time = (double)cvGetTickCount();
		captured_image=cvQueryFrame(cv_camera_capture);
		cvCopy(captured_image,main_image);
		//
		//
			cvClearMemStorage(main_storage);
			cvClearMemStorage(main_storage_poligon);
			//source image to grayscale
			cvCvtColor(main_image,main_processed_image,CV_BGR2GRAY); //converts input image from one color space to another

			cvSmooth(main_processed_image,main_processed_image,CV_GAUSSIAN,3);//Smooths the image in one of several ways  // CV_GAUSSIAN (gaussian blur) - convolving image with param1×param2 Gaussian kernel.

///!!!! agafar el model de thershold d'openframeworks hand recognition....
			cvThreshold (main_processed_image, main_processed_image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); //Applies fixed-level threshold to array elements

			cvNot(main_processed_image,main_processed_image); //invert colors

			//contour finder:
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
							if(is_view_enabled)cvDrawContours(screen,c,CV_RGB(255,255,0),CV_RGB(200,255,255),0);
							if(is_view_enabled)cvDrawContours(screen,c_vnext,CV_RGB(255,0,0),CV_RGB(0,255,255),0);

							cvGetPerspectiveTransform (tmp_pnt, dst_pnt, map_matrix);
							cvWarpPerspective (main_processed_image, fiducial_image, map_matrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll (0));

							//int notzeroCount=0;
						
							int maxCount=0;
							int markerDirection=0;
							cvResize(fiducial_image,fiducial_image_zoomed);
							fiducial_finder->DecodeFiducial(fiducial_image, temporal);

							//notzeroCount=cvCountNonZero(tempmask);

							//std::cout << "temp_ssid " << tmp_ssid << "  ssidGenerator " << ssidGenerator << std::endl;
							if(tmp_ssid == 0 /*&& temporal.GetFiducialID() != -1*/)
							{
								//add fiducial
								fiducial_map[ssidGenerator++] = new Fiducial(temporal);
								tmp_ssid = ssidGenerator-1;
								std::cout << "new" << std::endl;
							}
							else if( tmp_ssid > 0)
							{
								fiducial_map[tmp_ssid]->Update(temporal);
								//std::cout << "update" << std::endl;
								
							}

							/**************/
							markerDirection = fiducial_map[tmp_ssid]->GetOrientation();
							std::cout << markerDirection <<std::endl;
							if(markerDirection==0)
							{
								src_pnt[0].x=tmp_pnt[0].x;
								src_pnt[0].y=tmp_pnt[0].y;
								src_pnt[1].x=tmp_pnt[3].x;
								src_pnt[1].y=tmp_pnt[3].y;
								src_pnt[2].x=tmp_pnt[2].x;
								src_pnt[2].y=tmp_pnt[2].y;
								src_pnt[3].x=tmp_pnt[1].x;
								src_pnt[3].y=tmp_pnt[1].y;
							}
							else if(markerDirection==1)//90
							{
								src_pnt[0].x=tmp_pnt[1].x;
								src_pnt[0].y=tmp_pnt[1].y;
								src_pnt[1].x=tmp_pnt[0].x;
								src_pnt[1].y=tmp_pnt[0].y;
								src_pnt[2].x=tmp_pnt[3].x;
								src_pnt[2].y=tmp_pnt[3].y;
								src_pnt[3].x=tmp_pnt[2].x;
								src_pnt[3].y=tmp_pnt[2].y;
							
							}
							else if(markerDirection==3)//180
							{
								src_pnt[0].x=tmp_pnt[2].x;
								src_pnt[0].y=tmp_pnt[2].y;
								src_pnt[1].x=tmp_pnt[1].x;
								src_pnt[1].y=tmp_pnt[1].y;
								src_pnt[2].x=tmp_pnt[0].x;
								src_pnt[2].y=tmp_pnt[0].y;
								src_pnt[3].x=tmp_pnt[3].x;
								src_pnt[3].y=tmp_pnt[3].y;
							}
							else if(markerDirection==2)//270
							{
								src_pnt[0].x=tmp_pnt[3].x;
								src_pnt[0].y=tmp_pnt[3].y;
								src_pnt[1].x=tmp_pnt[2].x;
								src_pnt[1].y=tmp_pnt[2].y;
								src_pnt[2].x=tmp_pnt[1].x;
								src_pnt[2].y=tmp_pnt[1].y;
								src_pnt[3].x=tmp_pnt[0].x;
								src_pnt[3].y=tmp_pnt[0].y;
							}
							cvInitMatHeader (&image_points, 4, 1, CV_32FC2, src_pnt);
							cvInitMatHeader (&object_points, 4, 3, CV_32FC1, baseMarkerPoints);
							cvFindExtrinsicCameraParams2(&object_points,&image_points,intrinsic,distortion,rotation,translation);
							cvProjectPoints2(srcPoints3D,rotation,translation,intrinsic,distortion,dstPoints2D);
							//
							CvPoint startpoint;
							CvPoint endpoint;

							startpoint=cvPoint((int)dstPoints2D->data.fl[0], (int)dstPoints2D->data.fl[1]);
							for(j=1;j<4;j++)
							{
								endpoint=  cvPoint((int)dstPoints2D->data.fl[(j)*3],(int)dstPoints2D->data.fl[1+(j)*3]);
						
								if(j==1)
								{
									cvLine(screen,startpoint,endpoint,CV_RGB(255,0,0),2,8,0);
									cvPutText(screen, "X", endpoint, &axisfont,CV_RGB(255,0,0));
								}
								if(j==2)
								{
									cvLine(screen,startpoint,endpoint,CV_RGB(0,255,0),2,8,0);
									cvPutText(screen, "Y", endpoint, &axisfont,CV_RGB(0,255,0));
								}
								if(j==3)
								{
									cvLine(screen,startpoint,endpoint,CV_RGB(0,0,255),2,8,0);
									cvPutText(screen, "Z", endpoint, &axisfont,CV_RGB(0,0,255));
								}
							}
							/**************/

						}
					}
				}

			}

		to_remove.clear();
		for(FiducialMap::iterator it = fiducial_map.begin(); it!= fiducial_map.end(); it++)
		{
			if(it->second->IsUpdated())
			{
			}
			else
			{
				//it->second->RemoveStart(process_time);
				to_remove.push_back(it->first);
			}

			sprintf_s(text,"%i",it->first); 
			cvPutText(screen, text, cvPoint((int)it->second->GetX(), (int)it->second->GetY()), &dfont, CV_RGB(0, 0, 255));

			/*if( it ->second->CanBeRemoved(process_time) )
				to_remove.push_back(it->first);*/
		}

		for(std::vector<unsigned int>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
		{
			std::cout << "removed " << *it << std::endl;
			fiducial_map.erase(*it);
		}

		//
		//
		process_time = (double)cvGetTickCount()-process_time;
		if(is_view_enabled)
		{
			sprintf_s(text,"process_time %gms", process_time/(cvGetTickFrequency()*1000.)); 
			cvPutText(screen, text, cvPoint(10, 40), &dfont, CV_RGB(0, 255, 0));
			cvShowImage(MAIN_TITTLE,screen);
			if(show_fid_processor)cvShowImage(FIDUCIAL_TITTLE,fiducial_image);
		}
		
		//Key Check
		presskey=cvWaitKey (100);
		if(presskey==27) break;
		else if(presskey=='v')
		{
			screen_to_show ++;
			if(screen_to_show > VIEW_NONE) screen_to_show = 0;
			SetView(screen_to_show);
		}
		else if(presskey == 'V')
			ToggleDisplayFIDProcessor();
	}

	cvReleaseCapture(&cv_camera_capture);
	cvReleaseImage(&main_image);
	cvReleaseImage(&main_processed_image);	
	cvReleaseImage(&main_processed_contour);
	cvReleaseImage(&fiducial_image);	
	//cvReleaseImage(&tmp_img);
	//cvReleaseImage(&mask0);
	//cvReleaseImage(&mask90);
	//cvReleaseImage(&mask180);
	//cvReleaseImage(&mask270);
	//cvReleaseImage(&tempmask);
	cvReleaseMat (&map_matrix);
	cvReleaseMemStorage(&main_storage_poligon);
	cvReleaseMemStorage(&main_storage);
	delete(fiducial_finder);
	

	if(show_fid_processor)cvDestroyWindow(FIDUCIAL_TITTLE);
	cvDestroyWindow(MAIN_TITTLE);
    return 0;
}

void SetView(int view)
{
	if(view < 0 || view >= VIEW_NONE)
	{
		screen_to_show = VIEW_NONE;
		is_view_enabled = false;
	}
	else
	{
		screen_to_show = view;
		is_view_enabled = true;
		if(screen_to_show == VIEW_RAW)
			screen = main_image;
		else if(screen_to_show == VIEW_THRESHOLD)
			screen = main_processed_image;
	}
}

void ToggleDisplayFIDProcessor()
{
	show_fid_processor = ! show_fid_processor;
	if(show_fid_processor)
	{
		cvNamedWindow (FIDUCIAL_TITTLE, CV_WINDOW_AUTOSIZE);
	}
	else
	{
		cvDestroyWindow(FIDUCIAL_TITTLE);
	}
}
