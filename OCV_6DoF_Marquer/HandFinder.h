#pragma once
#include "frameprocessor.h"
#include "Hand.h"
#include <map>

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
	CvPoint			hand_centroid;
	std::map<unsigned long, Hand*> hands;

	int				threshold_value;
	float area;
	float length;
	int max_area;
	int min_area;
public:
	HandFinder(void);
	~HandFinder(void);

	AliveList GetAlive();
protected:
	void KeyInput(char key);
	void UpdatedValuesFromGui(); 
	IplImage* Process(IplImage*	main_image);
};

