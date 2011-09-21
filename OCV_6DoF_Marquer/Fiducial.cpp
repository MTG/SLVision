#include "StdAfx.h"
#include "Fiducial.h"
#include <iostream>

#define DISTANCE_OFFSET 30
#define AREA_OFFSET 1000
#define REMOVE_OFFSET 500

unsigned int Fiducial::id_generator = 0;

Fiducial::Fiducial(void):x(0),y(0),area(0),a(cvPoint(0,0)),b(cvPoint(0,0)),c(cvPoint(0,0)),d(cvPoint(0,0)),fiducial_id(-1),orientation(0),is_updated(false),removed_time(-1)
{
}


Fiducial::~Fiducial(void)
{
}

Fiducial::Fiducial(const Fiducial &copy):removed_time(-1)
{
	x = copy.x;
	y = copy.y;
	a = cvPoint(copy.a.x,copy.a.y);
	b = cvPoint(copy.b.x,copy.b.y);
	c = cvPoint(copy.c.x,copy.c.y);
	d = cvPoint(copy.d.x,copy.d.y);
	fiducial_id = GetNewId();
	orientation = copy.orientation;
	area = copy.area;
	is_updated = true;
}

void Fiducial::clear()
{
	x = 0;
	y = 0;
	a = cvPoint(0,0);
	b = cvPoint(0,0);
	c = cvPoint(0,0);
	d = cvPoint(0,0);
	fiducial_id = -1;
	orientation = 0;
	is_updated = true;
}

void Fiducial::SetId(unsigned int id)
{
	this->fiducial_id = id;
}

void Fiducial::Update(const Fiducial &copy)
{
	Update(copy.x, copy.y, copy.a, copy.b, copy.c, copy.d, copy.area, copy.orientation);
}

void Fiducial::Update(float _x, float _y,CvPoint _a,CvPoint _b,CvPoint _c,CvPoint _d, float _area, int orientation)
{
	this->x = _x;
	this->y = _y;
	this->a = cvPoint(_a.x, _a.y);
	this->b = cvPoint(_b.x, _b.y);
	this->c = cvPoint(_c.x, _c.y);
	this->d = cvPoint(_d.x, _d.y);
	this->area = fabs(_area);
	is_updated = true;
	this->orientation = orientation;
}

bool Fiducial::Is_inside(const Fiducial &f)
{
	if( f.area  < area)
	{
		if(!(	IsLeft(a.x, a.y, b.x, b.y, f.x, f.y) < 0 ||
				IsLeft(b.x, b.y, c.x, c.y, f.x, f.y) < 0 ||
				IsLeft(c.x, c.y, d.x, d.y, f.x, f.y) < 0 ||
				IsLeft(d.x, d.y, a.x, a.y, f.x, f.y) < 0 ))
		{
			return true;
		}
	}
	return false;
}


bool Fiducial::CanUpdate(const Fiducial &f, float & minimum_distance)
{
	if ( fabs((float)(f.area-area)) <= AREA_OFFSET )
	{
		float tmp = fabs(nsqdist(f.x,f.y,x,y));
		if(tmp <= DISTANCE_OFFSET && tmp < minimum_distance)
		{
			minimum_distance = tmp;
			return true;
		}
	}
	return false;
}

int Fiducial::GetOrientation()
{
	return orientation;
}

unsigned int Fiducial::GetFiducialID()
{
	return fiducial_id;
}

float Fiducial::GetX()
{
	return x;
}

float Fiducial::GetY()
{
	return y;
}

bool Fiducial::IsUpdated()
{
	if(is_updated)
	{
		is_updated = false;
		return true;
	}
	return false;
}

bool Fiducial::CanBeRemoved(double actual_time)
{
	if(removed_time != -1 && ((actual_time - removed_time)/(cvGetTickFrequency()*1000.) >= REMOVE_OFFSET) )
		return true;
	return false;
}

void Fiducial::RemoveStart(double actual_time)
{
	removed_time = actual_time;
}

void Fiducial::SetOrientation(int o)
{
	orientation = o;
}

/****************************************
* Fiduail ID Generator
*****************************************/
///id generator
unsigned int Fiducial::GetNewId()
{
	return id_generator++;
}


/****************************************
* general purpose functions
*****************************************/

float nsqdist(const CvPoint &a, const CvPoint &b)
{
	return nsqdist(a.x,a.y,b.x,b.y);
}

float nsqdist(float x, float y, float a, float b)
{
	float uu = (float)(a-x);
	float vv = (float)(b-y);
	return sqrt(uu*uu + vv*vv);
}

float nsqdist(int x, int y, int a, int b)
{
	float uu = (float)(a-x);
	float vv = (float)(b-y);
	return sqrt(uu*uu + vv*vv);
}

//vector AB  point C
float vect_point_dist(float ax, float ay, float bx, float by, float cx, float cy)
{
	float abx = bx-ax;
	float aby = by-ay;
	float acx = cx-ax;
	float acy = cy-ay;
	return ( ( abx*acy - aby*acx) / sqrt(abx*abx +aby*aby));
}

/// Givven a segment A(ax,ay)--B(bx,by) returns if the point P(px,py) is at the left side of the segment.
double IsLeft(double ax, double ay, double bx, double by, float px, float py)
{
	return ((bx - ax) * (py - ay) - (px - ax) * (by - ay));
}