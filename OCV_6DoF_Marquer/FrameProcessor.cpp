//#include "StdAfx.h"
#include "FrameProcessor.h"


FrameProcessor::FrameProcessor(std::string name)
{
	process_key = false;
	guiMenu = new GuiMenu(name);
	enable = true;
}


FrameProcessor::~FrameProcessor(void)
{
}

IplImage* FrameProcessor::ProcessFrame(IplImage*	main_image)
{
	if(enable)
		return Process(main_image);
	return NULL;
}

void FrameProcessor::EnableKeyProcessor(bool en)
{
	process_key = en;
}

bool FrameProcessor::can_process_key()
{
	return process_key;
}

void FrameProcessor::ProcessKey(char key)
{
	if(process_key)
	{
		switch(key)
		{
			case '8':
				guiMenu->MoveDown();
				break;
			case '2':
				guiMenu->MoveUp();
				break;
			case '+':
				guiMenu->SelectedUp();
				UpdatedValuesFromGui();
				break;
			case '-':
				guiMenu->SelectedDown();
				UpdatedValuesFromGui();
				break;
		}
		KeyInput(key);
	}
}

void FrameProcessor::DrawMenu(IplImage* img)
{
	if(process_key)guiMenu->Draw(img);
}

bool FrameProcessor::IsEnabled()
{
	return enable;
}

void FrameProcessor::Enable(bool enable)
{
	this->enable = enable;
}