/*
 * Copyright (C) 2011-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of SLVision
 *
 * SLVision is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/*
 *	Daniel Gallardo Grassot
 *	daniel.gallardo@upf.edu
 *	Barcelona 2011
 */

#include "FrameProcessor.h"
#include "GlobalConfig.h"

FrameProcessor::FrameProcessor(std::string name):
	enable_processor(datasaver::GlobalConfig::getRef("FrameProcessor:"+name+":enable",1))
{
	enable = true;
	show_screen = false;
	this->name = name;
}

FrameProcessor::~FrameProcessor(void)
{
}

void FrameProcessor::ProcessFrame(cv::Mat&	main_image)
{
	if(enable_processor == 1 && !this->IsEnabled())
	{
		Enable(true);
	}
	else if(enable_processor == 0 && this->IsEnabled())
	{
		Enable(false);
	}

	if(enable)
		Process(main_image);
	else if( show_screen)
	{
		cv::imshow(this->name,main_image);
	}
}

void FrameProcessor::ShowScreen(int state)
{
	if(show_screen == false && state == 1)
	{
		cv::namedWindow(name,CV_WINDOW_AUTOSIZE);
		show_screen = true;
		BuildGui();
	}
	else if(show_screen == true && state == 0)
	{
		cv::destroyWindow(name);
		show_screen = false;
	}
}
//void FrameProcessor::EnableKeyProcessor(bool en)
//{
//	process_key = en;
//}

//bool FrameProcessor::can_process_key()
//{
//	return process_key;
//}
//
//void FrameProcessor::ProcessKey(char key)
//{
//	if(process_key)
//	{
//		switch(key)
//		{
//			case KEY_MENU_UP_1:
//			case KEY_MENU_UP_2:
//			case KEY_MENU_UP_3:
//				guiMenu->MoveDown();
//				break;
//			case KEY_MENU_DOWN_1:
//			case KEY_MENU_DOWN_2:
//			case KEY_MENU_DOWN_3:
//				guiMenu->MoveUp();
//				break;
//			case KEY_MENU_INCR_1:
//			case KEY_MENU_INCR_2:
//			case KEY_MENU_INCR_3:
//				guiMenu->SelectedUp();
//				UpdatedValuesFromGui();
//				break;
//			case KEY_MENU_DECR_1:
//			case KEY_MENU_DECR_2:
//			case KEY_MENU_DECR_3:
//				guiMenu->SelectedDown();
//				UpdatedValuesFromGui();
//				break;
//		}
//		KeyInput(key);
//	}
//}

//void FrameProcessor::DrawMenu(IplImage* img)
//{
//	if(process_key)guiMenu->Draw(img);
//}

bool FrameProcessor::IsEnabled()
{
	return enable;
}

void FrameProcessor::Enable(bool enable)
{
	this->enable = enable;
}

//void FrameProcessor::SendOSCData()
//{
//	this->RepportOSC();
//}
