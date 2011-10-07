#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "Globals.h"
#include "GuiMenu.h"

typedef std::vector<unsigned long> AliveList;

class FrameProcessor
{
private:
	bool process_key;
protected:
	bool enable;

	bool can_process_key();
	virtual void KeyInput(char key)=0;
	GuiMenu* guiMenu;
	virtual void UpdatedValuesFromGui()=0; 
	virtual void Process(IplImage*	main_image)=0;
public:
	FrameProcessor(std::string name);
	virtual ~FrameProcessor(void);
	void ProcessFrame(IplImage*	main_image);
	virtual AliveList GetAlive()=0;
	void ProcessKey(char key);
	void EnableKeyProcessor(bool en = true);
	void DrawMenu(IplImage* img);
};

