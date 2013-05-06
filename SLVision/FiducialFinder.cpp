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

#include "FiducialFinder.h"
#include "Globals.h"
#include "GlobalConfig.h"
#include <sstream>
#include <iostream>

FiducialFinder::FiducialFinder(int _fiducial_window_size)
{
	fiducial_window_size = _fiducial_window_size;
	InitFID();
}

FiducialFinder::~FiducialFinder()
{
}

vector_nodes FiducialFinder::GetLevel (int level, int id, std::vector<cv::Vec4i>& hierarchy)
{
	vector_nodes input;
	input.push_back(node(level,id));
	if(hierarchy[id][2] != -1)
	{
		vector_nodes temp = GetLevel (level+1, hierarchy[id][2], hierarchy);
		for(unsigned int i = 0; i < temp.size(); i++)
			input.push_back(temp[i]);
	}
	if(hierarchy[id][0] != -1)
	{
		vector_nodes temp = GetLevel (level, hierarchy[id][0], hierarchy);
		for(unsigned int i = 0; i < temp.size(); i++)
			input.push_back(temp[i]);
	}
	return input;
}

int FiducialFinder::DecodeFiducial(cv::Mat& src, Fiducial & candidate)
{
	float x, y;
	float bx, by;
	int counter,axis;
	fiducial_nodes.clear();
	bx = 0; by =0;
	counter =0;
	/******************************************************
	* Find the fiducial hierarchy
	*******************************************************/
	cv::Mat fid;
	src.copyTo ( fid );
	cv::findContours(fid,contours,hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE );

	if(hierarchy.size()!= 0)
	{
		//for(int i = 0; i < hierarchy.size(); i++)
		//{
		//	std::cout << i << "  h0: " << hierarchy[i][0]<< "  h1: " << hierarchy[i][1]<< "  h2: " << hierarchy[i][2]<< "  h3: " << hierarchy[i][3]<< std::endl;
		//}
		//std::cout << std::endl;
		//std::cout << "getlevel" << std::endl;
		/******************************************************
		* Retrieve hierarchy data: nodes, orientation and fiducial code
		*******************************************************/
		vector_nodes nodes = GetLevel(0,0,hierarchy);
		for(unsigned int i = 0; i< nodes.size(); i++)
		{
			if(nodes[i].first > 1)
			{
				int node_level = abs(nodes[i].first-3);
				//std::cout << node_level << "oo" << nodes[i].second << "  " ;
				fiducial_nodes.push_back(node_level);
				if(node_level == 1)
				{
					cv::Moments fiducial_blob_moments = cv::moments(contours[nodes[i].second],true);
					x = (float)(fiducial_blob_moments.m10 / fiducial_blob_moments.m00);
					y = (float)(fiducial_blob_moments.m01 / fiducial_blob_moments.m00);
					counter++;
					bx += x;
					by += y;
				}
			}
		}
		bx = bx / counter;
		by = by / counter;
		axis = 0;
		//Get the fiducial orientation
		GetMinDistToFiducialAxis(axis,bx,by);

		
		//std::cout <<axis << " " << bx << " " << by<< std::endl;
		//for(intList::iterator it = fiducial_nodes.begin(); it != fiducial_nodes.end(); it++)
		//{
		//	std::cout << (*it);
		//}
		//std::cout << std::endl;

		/******************************************************
		* Fiducial Decode
		*******************************************************/
		int tmpid = GetId(BinaryListToInt(fiducial_nodes));
		if(tmpid >= 0)
		{
			candidate.SetId(tmpid);
			candidate.SetOrientation(axis);
			candidate.SetSize((float)GetSize(BinaryListToInt(fiducial_nodes)));
			candidate.OritentateCorners();
			candidate.CalculateIntrinsics();
			return tmpid;
		}
		//std::cout <<tmpid<< std::endl;
		//std::cout <<std::endl;			
	}
	candidate.OritentateCorners();
	candidate.CalculateIntrinsics();
	return -1;	
}


float FiducialFinder::GetMinDistToFiducialAxis(int &axis, float x, float y)
{
	float u,l,r,d;
	l = fabs(vect_point_dist(				 0,					0,				   0, FIDUCIAL_WIN_SIZE, x, y));
	r = fabs(vect_point_dist(FIDUCIAL_WIN_SIZE, FIDUCIAL_WIN_SIZE, FIDUCIAL_WIN_SIZE,				  0, x, y));
	u = fabs(vect_point_dist(				 0, FIDUCIAL_WIN_SIZE, FIDUCIAL_WIN_SIZE, FIDUCIAL_WIN_SIZE, x, y));
	d = fabs(vect_point_dist(				 0,					0, FIDUCIAL_WIN_SIZE,				  0, x, y));
	
	if( l < r && l < u && l < d)
	{
		axis = AXIS_LEFT;
		return l;
	}
	else if (r < l && r < u && r < d)
	{
		axis = AXIS_RIGHT;
		return r;
	}
	else if (u < l && u < r && u < d)
	{
		axis = AXIS_UP;
		return u;
	}
	else if (d < l && d < u && d < r)
	{
		axis = AXIS_DOWN;
		return d;
	}
	
	return 0;
}

unsigned int FiducialFinder::BinaryListToInt(const intList &data)
{
	unsigned int candidate = 0;
	int previous = 0;
	for(intList::const_iterator it = data.begin(); it != data.end(); it++)
	{
		if((*it) == 1)
		{
			candidate += previous;
			previous = 1;
		}
		else if((*it) == 0)
		{
			previous *=10;
		}
	}
	candidate += previous;
	return candidate;
}

unsigned int FiducialFinder::StringBinaryListToInt(const char* data, int& size)
{
	intList list;
	int i = 0;
	std::string size_str= "";
	bool foundsize = false;
	while(data[i] != '\0')
	{
		if(foundsize)
		{
			size_str += data[i];
		}
		else
		{
			if(data[i] == '1')
				list.push_back(1);
			else if(data[i] == '0') 
				list.push_back(0);
			else if(data[i] == 's')
				foundsize = true;
		}
		i++;
	}
	if(foundsize)size = atoi(size_str.c_str());
	return BinaryListToInt(list);
}

void FiducialFinder::RepportOSC()
{

}

int FiducialFinder::GetId(unsigned int candidate)
{
	if(idmap.find(candidate) != idmap.end())
	{
		return idmap[candidate].first;
	}
	return -1;
}

int FiducialFinder::GetSize(unsigned int candidate)
{
	if(idmap.find(candidate) != idmap.end())
	{
		return idmap[candidate].second;
	}
	return 50;
}

void FiducialFinder::InitFID()
{
	LoadFiducialList();
}

void FiducialFinder::LoadFiducialList ()
{
	idmap.clear();
	
	//<z_fiducialsID>
	//		<count> n <count/>
	//		<fid0> ... <fid0/>
	//		<fidn> ... <fidn/>
	//<z_fiducialsID>

	int &num_fiducials = datasaver::GlobalConfig::getRef("Z_fiducialsID:count",0);
	if(num_fiducials == 0)
	{
		char* id[] = { 
					"10011s50\0",
					"100111s50\0",
					"10111s50\0",
					"1011s50\0", 
					"111s50\0",
					"100000101111s70\0",
					"100010011111s70\0",
					"100010101111s70\0",
					"100001011111s70\0",
					"100100101111s70\0",
					"100100111111s70\0",
					"100101011111s70\0",
					"100001001111s70\0",
					"101010101111s70\0",
					"100010001111s70\0",
					"o"};
		int i = 0;
		while( id[i][0] != 'o' )
		{
			int size = 50;
			int numfid = StringBinaryListToInt(id[i],size);
			idmap[numfid] = std::pair<int,int>(i, size);
			std::stringstream s;
			s << "Z_fiducialsID:fid_" << i;
			std::string temporal = datasaver::GlobalConfig::getRef<std::string>(s.str().c_str(),std::string(id[i]));
			i++;
		}
		num_fiducials = i;
	}
	else
	{
		for (int i = 0; i < num_fiducials; i++)
		{
			std::stringstream s;
			s << "Z_fiducialsID:fid_" << i;
			std::string temporal = datasaver::GlobalConfig::getRef<std::string>(s.str().c_str(),"0\0");
			int size = 50;
			int numfid = StringBinaryListToInt(temporal.c_str(),size);
			idmap[numfid] = std::pair<int,int>(i, size);;
		}
	}
}