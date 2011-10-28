#pragma once

#include <list>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "Fiducial.h"

#define MIN_DIST_TO_SIDE	10

#define AXIS_UP				0
#define AXIS_LEFT			1
#define AXIS_RIGHT			2
#define AXIS_DOWN			3
#define FIDUCIAL_WIN_SIZE	70
#define MIN_DIST_TO_SIDE	10
//datatypes
typedef std::list<int>		intList;
typedef std::map<unsigned long, int> IDMap;

class FiducialFinder
{
	int				fiducial_window_size;
	CvMoments*      fiducial_blob_moments;
	IplImage*		fiducial_processed_image;
	CvMemStorage*	fiducial_storage;
	intList			fiducial_nodes;
	int				nodecount;
	IDMap			idmap;

public:
	FiducialFinder(int _fiducial_window_size);
	~FiducialFinder();
	int DecodeFiducial(IplImage* src, Fiducial & candidate);
protected:
	unsigned int BinaryListToInt(const intList &data);
	unsigned int StringBinaryListToInt(const char* data);
	float GetMinDistToFiducialAxis(int &axis, float x, float y);
	int GetId(unsigned int candidate);
	void InitFID();
	void LoadFiducialList ();
};