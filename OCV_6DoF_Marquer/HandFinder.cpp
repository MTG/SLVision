#include "HandFinder.h"
#include "GlobalConfig.h"

#define MIN_HAND_SIZE 20
#define MAX_HAND_SIZE 500
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
	cvFindContours (main_processed_contour, main_storage, &firstcontour, sizeof (CvContour), CV_CHAIN_APPROX_SIMPLE/*CV_RETR_CCOMP*/);

	if(firstcontour != NULL)
	{
		polycontour=cvApproxPoly(firstcontour,sizeof(CvContour),main_storage_poligon,CV_POLY_APPROX_DP,4,1);
		for(CvSeq* c=firstcontour;c!=NULL;c=c->h_next)
		{
			area = 0;
			length = 0;
			area = (float)fabs ( cvContourArea(c,CV_WHOLE_SEQ));
			if(area >  MIN_HAND_SIZE && area < MAX_HAND_SIZE)
			//if((cvContourPerimeter(c)<2000)&&(cvContourPerimeter(c)>60))
			{
				//std::cout << area << std::endl;
				if(Globals::is_view_enabled)cvDrawContours(Globals::screen,c,CV_RGB(255,255,0),CV_RGB(200,255,255),0,3);
				CvSeq* hull;
				hull = cvConvexHull2(c, 0, CV_CLOCKWISE, 0 );
				cvMoments( c, blob_moments );
				hand_centroid.x = (int) ceil((blob_moments->m10 / blob_moments->m00));
				hand_centroid.y = (int) ceil((blob_moments->m01 / blob_moments->m00));
				Hand * hand = 0;
				//cont'e hulls
				//int hullcount = hull->total;
				//std::cout << hullcount << std::endl;
				



				
				//Get target hand
				float dist = 999;
				float tmp_dist;
				for(unsigned int h = 0; h < hands.size(); h++)
				{
					tmp_dist = hands[h].Distance(hand_centroid);
				   // std::cout << "gg " << tmp_dist << std::endl;
					if(tmp_dist < dist)
					{
						dist = tmp_dist;
						hand = &hands[h];
					}
				}
				//if target doesnt mathc, creates a new one
				if(dist > MINIMUM_CENTROID_DISTANCE || hand == 0)
				{
					unsigned long sid = Globals::ssidGenerator++;
					hands.insert(std::pair<unsigned long, Hand>(sid,Hand(sid,hand_centroid) ));
					hand = &hands[sid];
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
			}
		}
	}

	return main_processed_image;
}