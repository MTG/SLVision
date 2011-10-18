#pragma once
#include "frameprocessor.h"

class Touch
{
protected:
	
	bool is_updated;
	
public:
	float x, y;
	float area;

	Touch();
	Touch(const Touch &copy);
	void Update(float x, float y, float area);
	void Update(const Touch &copy);
	bool CanUpdate( const Touch &tch, float & minimum_distance);
	bool IsUpdated();
};

typedef std::map<unsigned int, Touch*> Pointmap;

class TouchFinder :
	public FrameProcessor
{
private:
	//float temp_x, temp_y, temp_area;
	Touch temp_touch;
	float temp_minimum_distance, test_distance;
	unsigned int candidate_id;
	std::vector<unsigned int> to_be_removed;
	char text[100];
protected:
	IplImage*		main_processed_image;
	IplImage*		main_processed_contour;
	int				threshold_value;
	CvMemStorage*	main_storage;
	CvSeq*			firstcontour;
	int				max_blob_size;
	int				min_blob_size;
	CvMoments*		blob_moments;
	Pointmap		pointmap;

	void KeyInput(char key);
	void UpdatedValuesFromGui(); 
	IplImage* Process(IplImage*	main_image);
public:
	TouchFinder(void);
	~TouchFinder(void);
	AliveList GetAlive();
};

