#pragma once
#include "frameprocessor.h"
class HandFinder :
	public FrameProcessor
{
	IplImage*		main_processed_image;
	IplImage*		main_processed_contour;
	CvMemStorage*	main_storage;
	CvMemStorage*	main_storage_poligon;
	CvMoments*		blob_moments;
	CvSeq*			firstcontour;
	CvSeq*			polycontour;

	int				threshold_value;
public:
	HandFinder(void);
	~HandFinder(void);

	AliveList GetAlive();
protected:
	void KeyInput(char key);
	void UpdatedValuesFromGui(); 
	IplImage* Process(IplImage*	main_image);
};

