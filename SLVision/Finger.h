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

#pragma once
#include "LowPass.h"

class Finger
{
protected:
	bool is_updated;
	bool is_on_the_air;
	int handID;
//	LowPass xcoord, ycoord;
	float x, y;
public:
	
	float area;

	Finger();
	Finger(const Finger &copy);
	void Update(float x, float y, float area);
	void Update(const Finger &copy);
	//bool CanUpdate( const Finger &tch, float & minimum_distance);
	bool IsUpdated(bool keep_flag=false);
	bool IsOnTheAir();
	void SetHandData(int handID, bool on_the_air);
	float GetX();
	float GetY();
};

