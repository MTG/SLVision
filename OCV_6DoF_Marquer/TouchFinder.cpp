#include "TouchFinder.h"
#include "GlobalConfig.h"
#include "Fiducial.h"
#include "TuioServer.h"
#define DISTANCE_OFFSET 50

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
	to_be_removed.clear();
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
				float tmp_dist = fabs(fnsqdist(temp_touch.x,temp_touch.y,it->second->x,it->second->y));
				if( temp_minimum_distance > tmp_dist)
				{
					candidate_id = it->first;
					temp_minimum_distance = tmp_dist;
				}
			}

			if(temp_minimum_distance > DISTANCE_OFFSET)
				candidate_id = 0;
			if(candidate_id == 0) //new touch
			{
				unsigned int new_id = Globals::ssidGenerator++;
				pointmap[new_id] = new Touch(temp_touch);
			}
			else //update touch
			{
				pointmap[candidate_id]->Update(temp_touch);
			}
		}
	}

	for(Pointmap::iterator it = pointmap.begin(); it != pointmap.end(); it++)
	{
		if (it->second->IsUpdated())
		{
			//tuio message
			//(unsigned int sid, unsigned int uid, unsigned int cid, float x, float y, float width, float press)
			TuioServer::Instance().AddPointerMessage(it->first, 0, 0, it->second->x/Globals::width, it->second->y/Globals::height, it->second->area, 0);
		}
		else
		{
			//tuiomessage_remove
			to_be_removed.push_back(it->first);
		}

		if(Globals::is_view_enabled)
		{
			sprintf_s(text,"%i",it->first); 
			Globals::Font::Write(Globals::screen,text,cvPoint((int)it->second->x, (int)it->second->y),FONT_HELP,0,255,0);
		}
	}

	for(std::vector<unsigned int>::iterator it = to_be_removed.begin(); it != to_be_removed.end(); it++)
	{
		pointmap.erase(*it);
	}

	return main_processed_image;
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

/********************************
* TOUCH METHODS
*********************************/

Touch::Touch():x(0),y(0),is_updated(false),area(0)
{}

Touch::Touch(const Touch &copy):x(copy.x),y(copy.y),is_updated(true),area(copy.area)
{}

void Touch::Update(float x, float y, float area)
{
	this->x = x;
	this->y = y;
	this->area = area;
	is_updated = true;
}

bool Touch::CanUpdate( const Touch &tch, float & minimum_distance)
{
	/*//if ( fabs((float)(tch.area-area)) <= AREA_OFFSET )
	{
		float tmp = fabs(fnsqdist(tch.x,tch.y,x,y));
		if(tmp <= DISTANCE_OFFSET && tmp <= minimum_distance)
		{
			minimum_distance = tmp;
			return true;
		}
	}*/
	return false;
}

bool Touch::IsUpdated()
{
	if(is_updated)
	{
		is_updated = false;
		return true;
	}
	return false;
}

void Touch::Update(const Touch &copy)
{
	this->Update(copy.x,copy.y,copy.area);
}