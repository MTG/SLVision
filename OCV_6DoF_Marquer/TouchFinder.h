#pragma once
#include "frameprocessor.h"
class TouchFinder :
	public FrameProcessor
{
protected:
	IplImage*		main_processed_image;
	IplImage*		main_processed_contour;
	int				threshold_value;
	CvMemStorage*	main_storage;
	CvSeq*			firstcontour;
	int				max_blob_size;
	int				min_blob_size;

	void KeyInput(char key);
	void UpdatedValuesFromGui(); 
	IplImage* Process(IplImage*	main_image);
public:
	TouchFinder(void);
	~TouchFinder(void);
	AliveList GetAlive();
};

