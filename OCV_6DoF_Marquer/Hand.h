#pragma once
#include <cv.h>
#include "TuioServer.h"
#include "Hand_Vertex.h"

class Hand
{
	
public:
	std::vector <Hand_Vertex>     vertexs;
	std::vector <Hand_Vertex>    hand_hole;
protected:
	///vars
    
    std::vector <Hand_Vertex*>    hull_vertexs;
    std::vector <Hand_Vertex*>    hand_vertexs;
	
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
	bool				is_pinching;
	bool				sendEndPinching;
	int edge; //-1 none 0 down 1 up 2 left 3 right
	bool updated;
    ///Methods
    Hand_Vertex* GetNearest(int x, int y);
    Hand_Vertex* GetNext(Hand_Vertex* v);
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
	float TCentroidX();
	float TCentroidY();

	bool IsPinching();
	bool IsPinchingEnd();
	void SetPinch(CvSeq* seq);
};

