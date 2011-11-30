#include "Hand_Vertex.h"
#include "Globals.h"
#include "Fiducial.h"

Hand_Vertex::Hand_Vertex(int x, int y, int position):
	position_path(position),
    isHull(false),
    isValley(false),
    compute(true),
    valley_distance(0),
    extreme(0),
    isFinger(false)
    {
		data = cvPoint(x,y);
	}


Hand_Vertex::~Hand_Vertex(void)
{
}

float Hand_Vertex::Distance(const Hand_Vertex & vect)
{
	return insqdist(data.x,data.y,vect.data.x,vect.data.y);
}
float Hand_Vertex::Distance(int x, int y)
{
	return insqdist(data.x,data.y,x,y);
}
int Hand_Vertex::GetDescription()
{
	return GetDescription(compute,isFinger,isValley,isHull);
}
float Hand_Vertex::GetDistortionatedX()
{
	return (float)data.x/Globals::width;
}
float Hand_Vertex::GetDistortionatedY()
{
	return (float)data.y/Globals::height;
}

/*
* 0000 any
* 0001 hand
* 0010 finger
* 0100 valley
* 1000 convex
*/
int Hand_Vertex::GetDescription(bool is_hand, bool is_finger, bool is_valley, bool is_convex)
{
	int description = 0;
	if(is_hand) description = 1;
	if(is_finger) description |= 1<<1;
	if(is_valley) description |= 1<<2;
	if(is_convex) description |= 1<<3;
	return description;
}