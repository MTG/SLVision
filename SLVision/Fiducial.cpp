/*
	Daniel Gallardo Grassot
	daniel.gallardo@upf.edu
	Barcelona 2011

	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
*/

#include "Fiducial.h"

#define DISTANCE_OFFSET 30
#define AREA_OFFSET 1000
#define REMOVE_OFFSET 500

Fiducial::Fiducial(void):x(0),y(0),area(0),a(cv::Point2f(0,0)),b(cv::Point2f(0,0)),c(cv::Point2f(0,0)),d(cv::Point2f(0,0)),fiducial_id(-1),orientation(0),is_updated(false),removed_time(-1)
{
	yaw=0;
	pitch=0;
	roll=0;
}


Fiducial::~Fiducial(void)
{
}

Fiducial::Fiducial(const Fiducial &copy):removed_time(-1)
{
	x = copy.x;
	y = copy.y;
	a = cv::Point2f(copy.a.x,copy.a.y);
	b = cv::Point2f(copy.b.x,copy.b.y);
	c = cv::Point2f(copy.c.x,copy.c.y);
	d = cv::Point2f(copy.d.x,copy.d.y);
	fiducial_id = copy.fiducial_id;
	orientation = copy.orientation;
	area = copy.area;
	is_updated = true;

	yaw=0;
	pitch=0;
	roll=0;
}

void Fiducial::clear()
{
	x = 0;
	y = 0;
	a = cv::Point2f(0,0);
	b = cv::Point2f(0,0);
	c = cv::Point2f(0,0);
	d = cv::Point2f(0,0);
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

void Fiducial::Update(float _x, float _y,cv::Point2f _a,cv::Point2f _b,cv::Point2f _c,cv::Point2f _d, float _area, int orientation)
{
	this->x = _x;
	this->y = _y;
	this->a = cv::Point2f(_a.x, _a.y);
	this->b = cv::Point2f(_b.x, _b.y);
	this->c = cv::Point2f(_c.x, _c.y);
	this->d = cv::Point2f(_d.x, _d.y);
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
		float tmp = fabs(fnsqdist(f.x,f.y,x,y));
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
/*///id generator
unsigned int Fiducial::GetNewId()
{
	return id_generator++;
}*/


/****************************************
* general purpose functions
*****************************************/

float nsqdist(const cv::Point2f &a, const cv::Point2f &b)
{
	return insqdist(a.x,a.y,b.x,b.y);
}

float fnsqdist(float x, float y, float a, float b)
{
	float uu = (float)(a-x);
	float vv = (float)(b-y);
	return sqrt(uu*uu + vv*vv);
}

float insqdist(int x, int y, int a, int b)
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

float ivect_point_dist(int ax, int ay, int bx, int by, int cx, int cy)
{
	float abx = (float)(bx-ax);
	float aby = (float)(by-ay);
	float acx = (float)(cx-ax);
	float acy = (float)(cy-ay);
	return ( ( abx*acy - aby*acx) / sqrt(abx*abx +aby*aby));
}

/// Givven a segment A(ax,ay)--B(bx,by) returns if the point P(px,py) is at the left side of the segment.
double IsLeft(double ax, double ay, double bx, double by, float px, float py)
{
	return ((bx - ax) * (py - ay) - (px - ax) * (by - ay));
}

cv::Point2f Fiducial::GetCorner(int corner)
{
	switch(corner)
	{
	case 0:
		return a;
		break;
	case 1:
		return b;
		break;
	case 2:
		return c;
		break;
	case 3:
		return d;
		break;
	}
	return a;
}

void Fiducial::OritentateCorners()
{
	
	int markerDirection = GetOrientation();
	std::cout << markerDirection<< std::endl;
	cv::Point2f ta,tb,tc,td;
	ta = cv::Point2f(a); tb = cv::Point2f(b); tc = cv::Point2f(c); td = cv::Point2f(d);
	//a0 b1 c2 d3
	switch(markerDirection)
	{
	case 0:
		a = tb;
		b = tc;
		c = td;
		d = ta;
		break;
	case 1:
		a = tc;
		b = td;
		c = ta;
		d = tb;
		break;
	case 2:

		break;
	case 3:
		
		a = td;
		b = ta;
		c = tb;
		d = tc;
		break;
	}
}



/*//					if(markerDirection==0)
//					{
//						src_pnt[0].x = xlist[0];		
//						src_pnt[1].x = xlist[1];
//						src_pnt[2].x = xlist[2];
//						src_pnt[3].x = xlist[3];
//
//						src_pnt[0].y = ylist[0];
//						src_pnt[1].y = ylist[1];
//						src_pnt[2].y = ylist[2];
//						src_pnt[3].y = ylist[3];
//#ifdef USEEIGHTPOINTS
//						src_pnt[4].x = exlist[0];		
//						src_pnt[5].x = exlist[3];
//						src_pnt[6].x = exlist[2];
//						src_pnt[7].x = exlist[1];
//
//						src_pnt[4].y = eylist[0];
//						src_pnt[5].y = eylist[3];
//						src_pnt[6].y = eylist[2];
//						src_pnt[7].y = eylist[1];
//#endif
//					}
//					else if(markerDirection==1)//90
//					{
//						src_pnt[0].x = xlist[3];		
//						src_pnt[1].x = xlist[0];
//						src_pnt[2].x = xlist[1];
//						src_pnt[3].x = xlist[2];
//
//						src_pnt[0].y = ylist[3];
//						src_pnt[1].y = ylist[0];
//						src_pnt[2].y = ylist[1];
//						src_pnt[3].y = ylist[2];
//#ifdef USEEIGHTPOINTS
//						src_pnt[4].x = exlist[1];		
//						src_pnt[5].x = exlist[0];
//						src_pnt[6].x = exlist[3];
//						src_pnt[7].x = exlist[2];
//
//						src_pnt[4].y = eylist[1];
//						src_pnt[5].y = eylist[0];
//						src_pnt[6].y = eylist[3];
//						src_pnt[7].y = eylist[2];
//#endif
//					}
//					else if(markerDirection==3)//180
//					{
//						src_pnt[0].x = xlist[2];		
//						src_pnt[1].x = xlist[3];
//						src_pnt[2].x = xlist[0];
//						src_pnt[3].x = xlist[1];
//
//						src_pnt[0].y = ylist[2];
//						src_pnt[1].y = ylist[3];
//						src_pnt[2].y = ylist[0];
//						src_pnt[3].y = ylist[1];
//#ifdef USEEIGHTPOINTS
//						src_pnt[4].x = exlist[2];		
//						src_pnt[5].x = exlist[1];
//						src_pnt[6].x = exlist[0];
//						src_pnt[7].x = exlist[3];
//
//						src_pnt[4].y = eylist[2];
//						src_pnt[5].y = eylist[1];
//						src_pnt[6].y = eylist[0];
//						src_pnt[7].y = eylist[3];
//#endif
//					}
//					else if(markerDirection==2)//270
//					{
//						src_pnt[0].x = xlist[1];		
//						src_pnt[1].x = xlist[2];
//						src_pnt[2].x = xlist[3];
//						src_pnt[3].x = xlist[0];
//
//						src_pnt[0].y = ylist[1];
//						src_pnt[1].y = ylist[2];
//						src_pnt[2].y = ylist[3];
//						src_pnt[3].y = ylist[0];
//#ifdef USEEIGHTPOINTS
//						src_pnt[4].x = exlist[3];		
//						src_pnt[5].x = exlist[2];
//						src_pnt[6].x = exlist[1];
//						src_pnt[7].x = exlist[0];
//
//						src_pnt[4].y = eylist[3];
//						src_pnt[5].y = eylist[2];
//						src_pnt[6].y = eylist[1];
//						src_pnt[7].y = eylist[0];
//#endif
//					}	*/