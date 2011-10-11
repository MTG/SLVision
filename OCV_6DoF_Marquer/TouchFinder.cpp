#include "TouchFinder.h"
#include "GlobalConfig.h"


TouchFinder::TouchFinder(void):FrameProcessor("TouchFinder")
{
	main_processed_image = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);		//allocates 1-channel memory frame for the image
	main_processed_contour = cvCreateImage(cvGetSize(Globals::screen),IPL_DEPTH_8U,1);	//allocates 1-channel memory frame for the image

	int cf_enabled = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:enable",1);
	if(cf_enabled == 1) Enable(true);
	else Enable(false);	
	threshold_value = datasaver::GlobalConfig::getRef("FrameProcessor:Touch_Finder:threshold_value",100);
	//populate gui
	guiMenu->AddBar("Threshold",0,255,1);
	guiMenu->AddBar("Enable",0,1,1);
	guiMenu->SetValue("Enable",(float)cf_enabled);
	guiMenu->SetValue("Threshold",(float)threshold_value);
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
}

IplImage* TouchFinder::Process(IplImage* main_image)
{
	cvThreshold(main_image,main_processed_image,threshold_value,255, CV_THRESH_BINARY);
	return main_processed_image;
}

AliveList TouchFinder::GetAlive()
{
	AliveList temp;

	return temp;
}