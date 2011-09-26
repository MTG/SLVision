#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "Globals.h"

typedef std::vector<unsigned long> AliveList;

class FrameProcessor
{
public:
	FrameProcessor(void);
	virtual ~FrameProcessor(void);
	virtual void ProcessFrame(IplImage*	main_image)=0;
	virtual AliveList GetAlive()=0;
};

