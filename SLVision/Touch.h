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

#pragma once
#include "LowPass.h"

class Touch
{
protected:
	bool is_updated;
	bool is_on_the_air;
	int handID;
	float x, y;
public:
	float area;
	Touch();
	Touch(const Touch &copy);
	void Update(float x, float y, float area);
	void Update(const Touch &copy);
	//bool CanUpdate( const Touch &tch, float & minimum_distance);
	bool IsUpdated(bool keep_flag=false);
	bool IsOnTheAir();
	void SetHandData(int handID, float x, float y);
	float GetX();
	float GetY();
	int GetHandID(){return handID;}
};

