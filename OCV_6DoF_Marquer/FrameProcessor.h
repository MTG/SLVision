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

