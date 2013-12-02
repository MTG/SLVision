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
	handID(-1)
{
}

Touch::Touch(const Touch &copy):
	x(copy.x),
	y(copy.y),
	is_updated(true),
	area(copy.area),
	is_on_the_air(copy.is_on_the_air),
	handID(copy.handID)
{
}

void Touch::Update(float x, float y, float area)
{
	this->x = x;
	this->y = y;
	this->area = area;
	is_updated = true;
	is_on_the_air = false;
}

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
	this->x = copy.x;
	this->y = copy.y;
	this->area = copy.area;
	this->is_on_the_air = copy.is_on_the_air;
	this->handID = copy.handID;
	this->is_updated = true;
}

bool Touch::IsOnTheAir()
{
	return this->is_on_the_air;
}

void Touch::SetHandData(int handID, float x, float y)
{
	this->handID = handID;
	if(!this->is_updated)
		this->is_on_the_air = true;
	this->x = x;
	this->y = y;
	is_updated = true;
}

float Touch::GetX()
{
	return x;
}

float Touch::GetY()
{
	return y;
}
