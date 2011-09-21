#pragma once

#include <cv.h>
#include <cxcore.h>

float vect_point_dist(float ax, float ay, float bx, float by, float cx, float cy);
float nsqdist(float x, float y, float a, float b);
float nsqdist(int x, int y, int a, int b);
float nsqdist(const CvPoint &a, const CvPoint &b);
double IsLeft(double ax, double ay, double bx, double by, float px, float py);

class Fiducial
{
	CvPoint a,b,c,d;
	float x, y;
	float area;
	unsigned int fiducial_id;
	int orientation;
	static unsigned int id_generator;
	bool is_updated;
	double removed_time;
public:
	Fiducial(void);
	Fiducial(const Fiducial &copy);
	~Fiducial(void);
	void clear();
	void SetId(unsigned int id);
	void Update(float x, float y,CvPoint a,CvPoint b,CvPoint c,CvPoint d, float area, int orientation);
	void Update(const Fiducial &copy);
	bool Is_inside(const Fiducial &f);
	bool CanUpdate(const Fiducial &f, float & minimum_distance);
	int GetOrientation();
	unsigned int GetFiducialID();
	float GetX();
	float GetY();
	bool IsUpdated();
	bool CanBeRemoved(double actual_time);
	void RemoveStart(double actual_time);
	void SetOrientation(int o);
	//fiducial id generator
	static unsigned int GetNewId();
};

