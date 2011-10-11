#include "MenuBar.h"
#include "Globals.h"

MenuBar::MenuBar(float min, float max, float step):min_parameter(min), max_parameter(max), step_parameter(step)
{
	if(min > max && step_parameter >0)
	{
		float temp = min;
		min = max;
		max = temp;
	}
	parameter = min;
	is_selected = false;
}

MenuBar::~MenuBar(void)
{
}

void MenuBar::DrawMenu(IplImage* img, int x, int y)
{
	if(is_selected)
		cvRectangle(img, cvPoint(x,y), cvPoint(x+110,y+20), CV_RGB(0,255,0), CV_FILLED);
	else
		cvRectangle(img, cvPoint(x,y), cvPoint(x+110,y+20), CV_RGB(100,150,100), CV_FILLED);
	int longitude = (int)ceil(((parameter - min_parameter)*100) / (max_parameter-min_parameter));
	cvRectangle(img, cvPoint(x+5,y+5), cvPoint(x+5+longitude,y+15), CV_RGB(0,0,0), CV_FILLED);
	//Globals::Font::Write(img,
	sprintf_s(text,"%f", parameter); 
	Globals::Font::Write(img,text,cvPoint(x+110, y+15),FONT_HELP,0,255,0);
}

void MenuBar::SetValue(float value)
{
	parameter = value;
	if(parameter > max_parameter ) parameter = max_parameter;
	if(parameter < min_parameter ) parameter = min_parameter;

}

void MenuBar::StepUp()
{
	SetValue(parameter+step_parameter);
}

void MenuBar::StepDown()
{
	SetValue(parameter-step_parameter);
}

float MenuBar::GetParameter()
{
	return parameter;
}

void MenuBar::SetSelected(bool select)
{
	is_selected = select;
}

