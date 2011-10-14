#include "TouchFinder.h"
#include "GlobalConfig.h"


TouchFinder::TouchFinder(void):FrameProcessor("TouchFinder")
{
	main_processed_image = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);		//allocates 1-channel memory frame for the image
	main_processed_contour = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);	//allocates 1-channel memory frame for the image
	
	// Creates a memory main_storage and returns pointer to 
	// it /param:/Size of the main_storage blocks in bytes. 
	// If it is 0, the block size is set to default 
	// value - currently it is 64K.
	main_storage = cvCreateMemStorage (0);

	int cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:enable",1);
	if(cf_enabled == 1) Enable(true);
	else Enable(false);	
	threshold_value = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:threshold_value",100);

	max_blob_size = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:maximum_blob_size",60);
	//populate gui
	guiMenu->AddBar("Threshold",0,255,1);
	guiMenu->AddBar("Enable",0,1,1);
	guiMenu->AddBar("Max_blob_size",10,100,2);
	guiMenu->SetValue("Enable",(float)cf_enabled);
	guiMenu->SetValue("Threshold",(float)threshold_value);
	guiMenu->SetValue("Max_blob_size",(float)max_blob_size);
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
	threshold_value = (int)ceil(guiMenu->GetValue("Threshold"));
	cf_threshold = threshold_value;

	int &cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:enable",1);
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
	
	int &cf_max_blob_size = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:maximum_blob_size",60);;
	max_blob_size = guiMenu->GetValue("Max_blob_size");
	max_blob_size = cf_max_blob_size;
}

IplImage* TouchFinder::Process(IplImage* main_image)
{
	cvThreshold(main_image,main_processed_image,threshold_value,255, CV_THRESH_BINARY);
	cvFindContours (main_processed_contour, main_storage, &firstcontour, sizeof (CvContour), CV_RETR_CCOMP);
	for(CvSeq* c=firstcontour;c!=NULL;c=c->h_next)
	{
		if( c -> v_next == NULL && (cvContourPerimeter(c)>max_blob_size) && (cvContourPerimeter(c)>max_blob_size)) //looking for single blobs
		{
			if(Globals::is_view_enabled)cvDrawContours(Globals::screen,c,CV_RGB(255,255,0),CV_RGB(200,255,255),0);
		}
	}
	return main_processed_image;
}

AliveList TouchFinder::GetAlive()
{
	AliveList temp;

	return temp;
}