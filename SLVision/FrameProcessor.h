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
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

typedef std::vector<unsigned long> AliveList;

class FrameProcessor
{
private:
	bool enable;
protected:
	int & enable_processor;
	std::string name;
	bool show_screen;
	virtual void Process(cv::Mat&	main_image)=0;
	virtual void BuildGui(bool force = false)=0;
	virtual void RepportOSC()=0;
public:
	FrameProcessor(std::string name);
	virtual ~FrameProcessor(void);
	virtual AliveList GetAlive()=0;
	void ProcessFrame(cv::Mat&	main_image);
	bool IsEnabled();
	void Enable(bool enable = true);
	//void SendOSCData();
	void ShowScreen(int state);
};

