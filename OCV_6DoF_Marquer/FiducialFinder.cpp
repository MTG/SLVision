////#include "StdAfx.h"
#include "FiducialFinder.h"
#include "Globals.h"
#include "GlobalConfig.h"
#include <iostream>
#include <sstream>

FiducialFinder::FiducialFinder(int _fiducial_window_size)
{
	fiducial_window_size = _fiducial_window_size;
	fiducial_storage = cvCreateMemStorage (0);
	fiducial_processed_image = cvCreateImage(cvSize(fiducial_window_size,fiducial_window_size),IPL_DEPTH_8U,1);
	fiducial_blob_moments = (CvMoments*)malloc( sizeof(CvMoments) );
	InitFID();
}

FiducialFinder::~FiducialFinder()
{
	free(fiducial_blob_moments);
	cvReleaseImage(&fiducial_processed_image);
	cvReleaseMemStorage(&fiducial_storage);
}

int FiducialFinder::DecodeFiducial(IplImage* src, Fiducial & candidate)
{
	float x, y;
	float bx, by;
	int counter;
	int axis;

	if(src->nChannels!=1 || src->width!=src->height)
		return -1;

	CvSeq *contour ;

	cvCopy(src,fiducial_processed_image);
//	Globals::fiducial_image_marker = src;

	cvFindContours (fiducial_processed_image, fiducial_storage, &contour, sizeof (CvContour), CV_RETR_CCOMP);
	fiducial_nodes.clear();
	bx = 0; by =0;
	counter =0;
	for(CvSeq* c=contour;c!=NULL;c=c->h_next)
	{
		//fiducial_blob_moments
		cvMoments( c, fiducial_blob_moments );
		x = (float)(fiducial_blob_moments->m10 / fiducial_blob_moments->m00);
		y = (float)(fiducial_blob_moments->m01 / fiducial_blob_moments->m00);
		if(	x > MIN_DIST_TO_SIDE && 
			y > MIN_DIST_TO_SIDE && 
			x <70-MIN_DIST_TO_SIDE && 
			y<70-MIN_DIST_TO_SIDE)
		{
			//if(show_fid_processor)cvDrawContours(screen,c,CV_RGB(255,255,0),CV_RGB(200,255,255),0);	
			CvSeq * temp;
			temp = c;
			nodecount = 0;
			if(temp->v_next != NULL)
			{
				temp = temp->v_next;
				for(CvSeq* h=temp;h!=NULL;h=h->h_next)
				{
					//if(show_fid_processor)cvDrawContours(screen,h,CV_RGB(255,0,0),CV_RGB(200,0,255),0);
					nodecount ++;
				}
			}
			else
			{
				counter ++;
				bx+=x;
				by+=y;
			}
			fiducial_nodes.push_back(nodecount);
		}
	}
	bx /= counter;
	by /= counter;
	axis = 0;
	//Get the fiducial orientation
	GetMinDistToFiducialAxis(axis,bx,by);
	if(counter != 0)
	candidate.SetOrientation(axis);
	//Get the fiducial ID
	fiducial_nodes.sort();	
	int tmpid = GetId(BinaryListToInt(fiducial_nodes));
	if(tmpid != -1) candidate.SetId(tmpid);

	return 1;	
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
		//std::cout << axis << std::endl;
		return l;
	}
	else if (r < l && r < u && r < d)
	{
		axis = AXIS_RIGHT;//std::cout << axis << std::endl;
		return r;
	}
	else if (u < l && u < r && u < d)
	{
		axis = AXIS_UP;//std::cout << axis << std::endl;
		return u;
	}
	else if (d < l && d < u && d < r)
	{
		axis = AXIS_DOWN;//std::cout << axis << std::endl;
		return d;
	}
	
	return 0;
}

unsigned int FiducialFinder::BinaryListToInt(const intList &data)
{
	int cnt = 0;
	unsigned int candidate = 0;
	for(intList::const_iterator it = data.begin(); it != data.end(); it++)
	{
		for (int i = 0; i < (*it); i++)
			cnt++;
		candidate += 1<<cnt;
		cnt++;
	}
	return candidate;
}

unsigned int FiducialFinder::StringBinaryListToInt(const char* data)
{
	intList list;
	int i = 0;
	unsigned int candidate = 0;
	while(data[i] != '\0')
	{
		if(data[i] == '1')
			list.push_back(1);
		else list.push_back(0);
		i++;
	}
	i = 0;
	for(intList::reverse_iterator it = list.rbegin(); it != list.rend(); it++)
	{
		if(*it ==1)
		{
			candidate += 1<<i;
		}
		i++;
	}
	return candidate;
}

int FiducialFinder::GetId(unsigned int candidate)
{
	if(idmap.find(candidate) != idmap.end())
	{
		return idmap[candidate];
	}
	return -1;
}

void FiducialFinder::InitFID()
{
	LoadFiducialList();

	std::cout << "Fiducial id transcoded:" << std::endl;
	for ( IDMap::iterator it = idmap.begin(); it != idmap.end();it++)
	{
		std::cout << it->first << "   " << it->second << std::endl;
	}
	
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
					"10011\0",
					"100000011111\0", 
					"100000001111\0",
					"100000101111\0",
					"100010011111\0",
					"100010101111\0",
					"100001011111\0",
					"100100101111\0",
					"100100111111\0",
					"100101011111\0",
					"100001001111\0",
					"101010101111\0",
					"100010001111\0",
					"o"};
		int i = 0;
		while( id[i][0] != 'o' )
		{
			idmap[StringBinaryListToInt(id[i])] = i;
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
			idmap[StringBinaryListToInt(temporal.c_str())] = i;
		}
	}
}