#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

class MenuBar
{
	float min_parameter, max_parameter, step_parameter, parameter;
	char text[90];
	bool is_selected;
public:
	MenuBar(float min, float max, float step=1.0f);
	~MenuBar(void);
	void DrawMenu(IplImage* img, int x, int y);
	void SetValue(float value);
	void StepUp();
	void StepDown();
	float GetParameter();
	void SetSelected(bool select = true);
};

