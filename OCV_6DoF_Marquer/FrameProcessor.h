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
	bool enable;
protected:
	GuiMenu* guiMenu;
	bool can_process_key();
	virtual void KeyInput(char key)=0;
	virtual void UpdatedValuesFromGui()=0; 
	virtual IplImage* Process(IplImage*	main_image)=0;
public:
	FrameProcessor(std::string name);
	virtual ~FrameProcessor(void);
	virtual AliveList GetAlive()=0;
	IplImage* ProcessFrame(IplImage*	main_image);
	void ProcessKey(char key);
	void EnableKeyProcessor(bool en = true);
	void DrawMenu(IplImage* img);
	bool IsEnabled();
	void Enable(bool enable = true);
};

