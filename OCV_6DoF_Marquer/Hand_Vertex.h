#pragma once
#include <cv.h>

class Hand_Vertex
{
public:
	CvPoint data;
    unsigned int position_path;
    bool isHull;
    bool isValley;
    bool compute;
    float valley_distance;
    int extreme;
    bool isFinger;

	Hand_Vertex(int x, int y, int position);
	~Hand_Vertex(void);
	float Distance(const Hand_Vertex & vect);
	float Distance(int x, int y);
	int GetDescription();
	float GetDistortionatedX();
	float GetDistortionatedY();
private:
	int GetDescription(bool is_hand, bool is_finger, bool is_valley, bool is_convex);
};