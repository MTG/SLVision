#pragma once
#include <cv.h>
#include "Fiducial.h"


class Hand
{
	class Vertex
    {
        public:
            CvPoint data;
            unsigned int position_path;
            bool isHull;
            bool isValley;
            bool compute;
            float valley_distance;
            int extreme;
            bool isFinger;
            Vertex(int x, int y, int position):
              //  data(cvPoint(x,y)),
                position_path(position),
                isHull(false),
                isValley(false),
                compute(true),
                valley_distance(0),
                extreme(0),
                isFinger(false)
                {
					data = cvPoint(x,y);
				}
            float Distance(const Vertex & vect){return insqdist(data.x,data.y,vect.data.x,vect.data.y);}
            float Distance(int x, int y){return insqdist(data.x,data.y,x,y);}
    };
protected:
	///vars
    std::vector <Vertex>     vertexs;
    std::vector <Vertex*>    hull_vertexs;
    std::vector <Vertex*>    hand_vertexs;
    bool                is_hand_detected;
    unsigned long       sessionID;
    float               area;
    float               length;
    CvPoint             centroid;
    CvPoint             centroid_hand;
    CvPoint             from, to;
    bool                first_update;
	bool				is_open;
	bool				confirmed_hand;
	int edge; //-1 none 0 down 1 up 2 left 3 right
	bool updated;
    ///Methods
    Vertex* GetNearest(int x, int y);
    Vertex* GetNext(Vertex* v);
    bool FindHandFrom(int indexplus);
    void GetNextIndexVertex(int & actual);
    void GetPreviousIndexVertex(int & actual);
public:
	Hand(void);
	Hand(unsigned long _sessionID, const CvPoint & _centroid);
	~Hand(void);
    float Distance(const CvPoint & _centroid);
    unsigned long GetSessionID();
    void Update(const CvPoint & _centroid);

    void Clear();
    void AddVertex(int x, int y);
    void AddVertexConvex(int x, int y);
    void ComputeHand(float area, float length);
    void draw(float x = 0, float y = 0);

	bool IsUpdated();
	int IsConfirmedAsHand();
	int IsOpened();
	CvPoint GetCentroid();
	float GetArea();
};

