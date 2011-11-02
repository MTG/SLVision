#include "HandFinder.h"
#include "GlobalConfig.h"


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

	guiMenu->AddBar("0-Enable",0,1,1);
	guiMenu->AddBar("1-Threshold",0,255,1);

	guiMenu->SetValue("0-Enable",(float)cf_enabled);
	guiMenu->SetValue("1-Threshold",(float)threshold_value);
}

void HandFinder::UpdatedValuesFromGui()
{
	int &cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:enable",1);
	int &cf_threshold = datasaver::GlobalConfig::getRef("FrameProcessor:hand_finder:threshold_value",100);

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
}

HandFinder::~HandFinder(void)
{
}

AliveList HandFinder::GetAlive()
{
	AliveList toreturn;

	return toreturn;
}

void HandFinder::KeyInput(char key)
{
}

#include <iostream>
IplImage* HandFinder::Process(IplImage*	main_image)
{
	cvClearMemStorage(main_storage);
	cvClearMemStorage(main_storage_poligon);

	cvThreshold(main_image,main_processed_image,threshold_value,255, CV_THRESH_BINARY);
	cvCopy(main_processed_image,main_processed_contour);	
	cvFindContours (main_processed_contour, main_storage, &firstcontour, sizeof (CvContour), CV_RETR_CCOMP);

	if(firstcontour != NULL)
	{
		polycontour=cvApproxPoly(firstcontour,sizeof(CvContour),main_storage_poligon,CV_POLY_APPROX_DP,100,1);
		for(CvSeq* c=polycontour;c!=NULL;c=c->h_next)
		{
			//if((cvContourPerimeter(c)<2000)&&(cvContourPerimeter(c)>60))
			{
				if(Globals::is_view_enabled)cvDrawContours(Globals::screen,c,CV_RGB(255,255,0),CV_RGB(200,255,255),0,3);
				CvSeq* hull;
				hull = cvConvexHull2(c, 0, CV_CLOCKWISE, 0 );
				cvMoments( c, blob_moments );
				hand_centroid.x = (blob_moments->m10 / blob_moments->m00);
				hand_centroid.y = (blob_moments->m01 / blob_moments->m00);

				if(Globals::is_view_enabled)cvDrawContours(Globals::screen,hull,CV_RGB(0,255,0),CV_RGB(200,255,255),0,3);
			}
		}
	}

	return main_processed_image;
}