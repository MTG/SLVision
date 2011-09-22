#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "Globals.h"

class FrameProcessor
{
public:
	FrameProcessor(void);
	virtual ~FrameProcessor(void);
	virtual void ProcessFrame(IplImage*	main_image)=0;
};

