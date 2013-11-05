/*
 * Copyright (C) 2011-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of SLVision
 *
 * SLVision is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/*
 *	Daniel Gallardo Grassot
 *	daniel.gallardo@upf.edu
 *	Barcelona 2011
 */

#include "Touch.h"
#include <iostream>

Touch::Touch():
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

Touch::Touch(const Touch &copy):
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

void Touch::Update(float x, float y, float area)
{
	this->x = x;
	this->y = y;
	this->area = area;
	is_updated = true;
//	xcoord.addvalue(x);
//	ycoord.addvalue(y);
}

/*bool Touch::CanUpdate( const Touch &tch, float & minimum_distance)
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

bool Touch::IsUpdated(bool keep_flag)
{
	if(is_updated)
	{
		if(!keep_flag) 
			is_updated = false;
		return true;
	}
	return false;
}

void Touch::Update(const Touch &copy)
{
	this->Update(copy.x,copy.y,copy.area);
}

bool Touch::IsOnTheAir()
{
	return this->is_on_the_air;
}

void Touch::SetHandData(int handID, bool on_the_air)
{
	this->handID = handID;
	this->is_on_the_air = on_the_air;
}

float Touch::GetX()
{
	/*if(this->is_on_the_air)
	{
		std::cout << "X: " << xcoord.getvalue() << std::endl;
		return xcoord.getvalue();
	}*/
	return x;
}

float Touch::GetY()
{
	/*if(this->is_on_the_air)
	{
		std::cout << "Y: " << ycoord.getvalue() << std::endl;
		return ycoord.getvalue();
	}*/
	return y;
}
