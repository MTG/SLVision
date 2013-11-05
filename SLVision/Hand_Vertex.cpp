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

//#include "Hand_Vertex.h"
//#include "Globals.h"
//#include "Fiducial.h"
//
//Hand_Vertex::Hand_Vertex(int x, int y, int position):
//	position_path(position),
//    isHull(false),
//    isValley(false),
//    compute(true),
//    valley_distance(0),
//    extreme(0),
//    isFinger(false)
//    {
//		data = cvPoint(x,y);
//	}
//
//
//Hand_Vertex::~Hand_Vertex(void)
//{
//}
//
//float Hand_Vertex::Distance(const Hand_Vertex & vect)
//{
//	return insqdist(data.x,data.y,vect.data.x,vect.data.y);
//}
//float Hand_Vertex::Distance(int x, int y)
//{
//	return insqdist(data.x,data.y,x,y);
//}
//int Hand_Vertex::GetDescription()
//{
//	return GetDescription(compute,isFinger,isValley,isHull);
//}
//float Hand_Vertex::GetDistortionatedX()
//{
//	return (float)data.x/Globals::width;
//}
//float Hand_Vertex::GetDistortionatedY()
//{
//	return (float)data.y/Globals::height;
//}
//
///*
//* 0000 any
//* 0001 hand
//* 0010 finger
//* 0100 valley
//* 1000 convex
//*/
//int Hand_Vertex::GetDescription(bool is_hand, bool is_finger, bool is_valley, bool is_convex)
//{
//	int description = 0;
//	if(is_hand) description = 1;
//	if(is_finger) description |= 1<<1;
//	if(is_valley) description |= 1<<2;
//	if(is_convex) description |= 1<<3;
//	return description;
//}
