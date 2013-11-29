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
#define DRAW_EVERYTHING 1

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
	
public:
	virtual void RepportOSC()=0;
	FrameProcessor(std::string name);
	virtual ~FrameProcessor(void);
	virtual AliveList GetAlive()=0;
	void ProcessFrame(cv::Mat&	main_image);
	bool IsEnabled();
	void Enable(bool enable = true);
	//void SendOSCData();
	void ShowScreen(int state);
};

