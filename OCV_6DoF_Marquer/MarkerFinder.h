#pragma once
#include "frameprocessor.h"
#include "FiducialFinder.h"

#define FIDUCIAL_IMAGE_SIZE 70

typedef std::map<unsigned int, Fiducial*> FiducialMap;

class MarkerFinder :
	public FrameProcessor
{
	//FIDUCIAL IMAGE Data
	IplImage*		fiducial_image;
	IplImage*		fiducial_image_zoomed;
	//MAIN IMAGE Data
	IplImage*		main_processed_image;
	IplImage*		main_processed_contour;
	CvMemStorage*	main_storage;
	CvMemStorage*	main_storage_poligon;
	CvMoments*  blob_moments;

	CvSeq *firstcontour;
	CvSeq *polycontour;
	//
	FiducialFinder* fiducial_finder;

	unsigned int ssidGenerator;
	FiducialMap fiducial_map;
	
	unsigned int tmp_ssid;
	bool to_process;
	float minimum_distance;
	std::vector<unsigned int> to_remove;
	Fiducial temporal;

	//
	CvMat *map_matrix;
	CvPoint2D32f src_pnt[4], dst_pnt[4], tmp_pnt[4];

	CvMat *intrinsic;
	CvMat *distortion;
	int i, j, k;
	CvMat object_points;
	CvMat image_points;
	CvMat point_counts;
	CvMat *rotation;
	CvMat *translation;
	CvMat *srcPoints3D;
	CvMat *dstPoints2D;
	CvPoint3D32f baseMarkerPoints[4];

	char text[100];

public:
	MarkerFinder(void);
	~MarkerFinder(void);
	void ProcessFrame(IplImage*	main_image);
protected:
	void InitGeometry();
	void InitFrames(IplImage*	main_image);
};

