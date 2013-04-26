/*
	Daniel Gallardo Grassot
	daniel.gallardo@upf.edu
	Barcelona 2011

	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
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