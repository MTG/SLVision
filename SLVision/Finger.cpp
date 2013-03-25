/*
	Daniel Gallardo Grassot
	daniel.gallardo@upf.edu
	Barcelona 2013

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

#include "Finger.h"
#include <iostream>

Finger::Finger():
	x(0),
	y(0),
	is_updated(false),
	area(-1),
	is_on_the_air(false),
	handID(-1)//,
//	xcoord(LowPass(4)),
//	ycoord(LowPass(4))
{
//	xcoord.Reset();
//	ycoord.Reset();
}

Finger::Finger(const Finger &copy):
	x(copy.x),
	y(copy.y),
	is_updated(true),
	area(copy.area),
	is_on_the_air(copy.is_on_the_air),
	handID(copy.handID)//,
//	xcoord(LowPass(copy.xcoord)),
//	ycoord(LowPass(copy.ycoord))
{
}

void Finger::Update(float x, float y, float area)
{
	this->x = x;
	this->y = y;
	this->area = area;
	is_updated = true;
//	xcoord.addvalue(x);
//	ycoord.addvalue(y);
}

/*bool Finger::CanUpdate( const Finger &tch, float & minimum_distance)
{
	//if ( fabs((float)(tch.area-area)) <= AREA_OFFSET )
	//{
	//	float tmp = fabs(fnsqdist(tch.x,tch.y,x,y));
	//	if(tmp <= DISTANCE_OFFSET && tmp <= minimum_distance)
	//	{
	//		minimum_distance = tmp;
	//		return true;
	//	}
	//}
	return false;
}*/

bool Finger::IsUpdated(bool keep_flag)
{
	if(is_updated)
	{
		if(!keep_flag) 
			is_updated = false;
		return true;
	}
	return false;
}

void Finger::Update(const Finger &copy)
{
	this->Update(copy.x,copy.y,copy.area);
}

bool Finger::IsOnTheAir()
{
	return this->is_on_the_air;
}

void Finger::SetHandData(int handID, bool on_the_air)
{
	this->handID = handID;
	this->is_on_the_air = on_the_air;
}

float Finger::GetX()
{
	/*if(this->is_on_the_air)
	{
		std::cout << "X: " << xcoord.getvalue() << std::endl;
		return xcoord.getvalue();
	}*/
	return x;
}

float Finger::GetY()
{
	/*if(this->is_on_the_air)
	{
		std::cout << "Y: " << ycoord.getvalue() << std::endl;
		return ycoord.getvalue();
	}*/
	return y;
}