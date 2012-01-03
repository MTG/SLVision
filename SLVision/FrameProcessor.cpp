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