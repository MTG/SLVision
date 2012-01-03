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

