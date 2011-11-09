#include "Hand.h"
#include "Globals.h"
#define MIN_DIST_EDGES 10

#define FINGER_THRESHOLD 20

Hand::~Hand(void)
{
}

Hand::Hand()
{
    centroid = cvPoint(-1,-1);
    sessionID = 0;
    area = 0.0f;
    length = 0.0f;
    first_update = true;
    edge = -1;
}

Hand::Hand(unsigned long _sessionID, const CvPoint & _centroid)
{
    area 		= 0.0f;
    length 		= 0.0f;
    sessionID   = _sessionID;
    centroid    = _centroid;
    first_update = true;
    edge = -1;
}

float Hand::Distance(const CvPoint & _centroid)
{
    return fabsf(insqdist(_centroid.x, _centroid.y, centroid.x, centroid.y));
}

unsigned long Hand::GetSessionID()
{
    return sessionID;
}

void Hand::Update(const CvPoint & _centroid)
{
    centroid    = _centroid;
}

void Hand::Clear()
{
    vertexs.clear();
    hull_vertexs.clear();
    hand_vertexs.clear();
}

void Hand::AddVertex(int x, int y)
{
    vertexs.push_back(Vertex(x,y,vertexs.size()));
}

void Hand::AddVertexConvex(int x, int y)
{
    Vertex* V= GetNearest(x,y);
    V->isHull = true;
    if(first_update)
    {
		if( !(fabs(ivect_point_dist( 0        ,0         , 0        ,Globals::height  , x,y)) < MIN_DIST_EDGES ||
			fabs(ivect_point_dist( 0        ,Globals::height, Globals::width,Globals::height  , x,y)) < MIN_DIST_EDGES ||
              fabs(ivect_point_dist( Globals::width,Globals::height, Globals::width,0           , x,y)) < MIN_DIST_EDGES ||
              fabs(ivect_point_dist( Globals::width,0         , 0        ,0           , x,y)) < MIN_DIST_EDGES ) )
        {
            V->compute = true;
        }
        else
        {
            V->compute = false;
        }
        first_update = false;
    }
    else
    {
        switch (edge)
        {
            case -1:
                if( !(fabs(ivect_point_dist( 0        ,0         , 0        ,Globals::height  , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( 0        ,Globals::height, Globals::width,Globals::height  , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( Globals::width,Globals::height, Globals::width,0           , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( Globals::width,0         , 0        ,0           , x,y)) < MIN_DIST_EDGES ) )
                     V->compute = true;
                else V->compute = false;
            break;
            case 0:
                if( !(fabs(ivect_point_dist( 0        ,0           , 0        ,Globals::height/4, x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( Globals::width,Globals::height/4, Globals::width,0           , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( Globals::width,0           , 0        ,0          , x,y)) < MIN_DIST_EDGES ) )
                     V->compute = true;
                else V->compute = false;
            break;
            case 1:
                if( !(fabs(ivect_point_dist( 0        ,Globals::height-(Globals::height/4) , 0         ,Globals::height                 , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( 0        ,Globals::height                , Globals::width ,Globals::height                 , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( Globals::width,Globals::height                , Globals::width ,Globals::height-(Globals::height/4)  , x,y)) < MIN_DIST_EDGES ))
                     V->compute = true;
                else V->compute = false;
            break;
            case 2:
                if( !(fabs(ivect_point_dist( 0          ,0         , 0          ,Globals::height  , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( 0          ,Globals::height, Globals::width/4,Globals::height  , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( Globals::width/4,0         , 0          ,0           , x,y)) < MIN_DIST_EDGES ) )
                     V->compute = true;
                else V->compute = false;
            break;
            case 3:
                if( !(fabs(ivect_point_dist( Globals::width-(Globals::width/4), Globals::height , Globals::width              , Globals::height, x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( Globals::width              , Globals::height , Globals::width              , 0         , x,y)) < MIN_DIST_EDGES ||
                      fabs(ivect_point_dist( Globals::width              , 0          , Globals::width-(Globals::width/4), 0         , x,y)) < MIN_DIST_EDGES ) )
                     V->compute = true;
                else V->compute = false;
            break;
        }
    }
    hull_vertexs.push_back(V);
}

Hand::Vertex* Hand::GetNearest(int x, int y)
{
    float tmpdist;
    float mindist = 99;
    Vertex* candidate = NULL;
    for (unsigned int i = 0; i < vertexs.size(); i++)
    {
        tmpdist = vertexs[i].Distance(x,y);
        if(tmpdist < mindist)
        {
            mindist = tmpdist;
            candidate = &vertexs[i];
        }
    }
    return candidate;
}

Hand::Vertex* Hand::GetNext(Hand::Vertex* v)
{
    if(v->position_path == vertexs.size()-1)
        return &vertexs[0];
    else return &vertexs[v->position_path+1];
}

void Hand::ComputeHand(float _area, float _length)
{
    area = _area;
    length = _length;
    ///find hull and valleys
    for (unsigned int i = 0; i < hull_vertexs.size(); i++)
    {
        Vertex* from = hull_vertexs[i];
        Vertex* to;
        if( i != hull_vertexs.size()-1)
            to = hull_vertexs[i+1];
        else
            to = hull_vertexs[0];
        if( from->compute || to->compute)
        {
            Vertex* iterator = GetNext(from);
            Vertex* candidate = iterator;
            float max_dist = 0;
            float tmpdist;
            while(!iterator->isHull)
            {
                tmpdist = fabsf(ivect_point_dist(from->data.x, from->data.y, to->data.x, to->data.y, iterator->data.x, iterator->data.y));
                if(tmpdist > max_dist)
                {
                    max_dist= tmpdist;
                    candidate = iterator;
                }
                iterator = GetNext(iterator);
            }
            candidate->isValley = true;
            candidate->valley_distance = max_dist;
            from->valley_distance = max_dist;
        }
    }
    ///Checking for hand orientationo (to, from) and its path extremes:
    ///find a computable hull vertex:
    int Q = -1;
    for (unsigned int i = 0; i < hull_vertexs.size(); i++)
    {
        if(!hull_vertexs[i]-> compute )
        {
            Q = i;
            break;
        }
    }
    ///if exist any computable hull vertex:
    if(Q != -1)
    {
        int Qfrom = Q;
        from.x = hull_vertexs[Q]->data.x;
        from.y = hull_vertexs[Q]->data.y;
        bool end = false;
        //move Q until the next non_computable hull vertex:
        while(! hull_vertexs[Q]->compute)
        {
            Q++;
            if(Q >= (int)hull_vertexs.size()) Q = 0;
            if( Q == Qfrom)
            {
                end = true;
                break;
            }
        }
        //if there are no _computable hull vertices
        if(!end)
        {
            //move Q to the previous hull vertice (before the non_computable
            if( Q == 0) Q = hull_vertexs.size()-1;
            else Q = Q-1;
            //calculates the from point as the midpoint of the two computable vertices that surrounds the non_computable ones.
            from.x = (from.x + hull_vertexs[Q]->data.x)/2;
            from.y = (from.y + hull_vertexs[Q]->data.y)/2;
            ///find path extremes:
            int extr_min = hull_vertexs[Qfrom]->position_path;
            int extr_max = hull_vertexs[Q]->position_path;
            while(!vertexs[extr_min].isValley)
            {
                GetPreviousIndexVertex(extr_min);
                if(extr_min == (int)hull_vertexs[Qfrom]->position_path) break;
            }
            vertexs[extr_min].extreme = -1;
            while(!vertexs[extr_max].isValley)
            {
                GetNextIndexVertex(extr_max);
                if(extr_max == (int)hull_vertexs[Q]->position_path) break;
            }
            vertexs[extr_max].extreme = 1;
            ///handDetection
            is_hand_detected = false;
            FindHandFrom(extr_max);
        }
        //Detect to
        float distance = 0;
        float tmp_dist;
        for (unsigned int i = 0; i < hull_vertexs.size(); i++)
        {
            tmp_dist = fabsf(insqdist(from.x,from.y,hull_vertexs[i]->data.x,hull_vertexs[i]->data.y));
            if(hull_vertexs[i]-> compute &&  tmp_dist > distance)
            {
                distance = tmp_dist;
                to.x = hull_vertexs[i]->data.x;
                to.y = hull_vertexs[i]->data.y;
            }
        }
        ///detect edge:
        edge = 2;
        distance = fabs(ivect_point_dist( 0        ,0         , 0        ,Globals::height  , from.x,from.y));
        tmp_dist = fabs(ivect_point_dist( 0        ,Globals::height, Globals::width,Globals::height  , from.x,from.y));
        if(tmp_dist < distance)
        {
            distance = tmp_dist;
            edge = 1;
        }
        tmp_dist = fabs(ivect_point_dist( Globals::width,Globals::height, Globals::width,0           , from.x,from.y));
        if(tmp_dist < distance)
        {
            distance = tmp_dist;
            edge = 3;
        }
        tmp_dist = fabs(ivect_point_dist( Globals::width,0         , 0        ,0           , from.x,from.y));
        if(tmp_dist < distance)
        {
            distance = tmp_dist;
            edge = 0;
        }
    }
}

float Hand::GetArea()
{
    return area;
}

bool Hand::FindHandFrom(int indexplus)
{
    centroid_hand.x = 0;
    centroid_hand.y = 0;
    int k = 0;
    if(vertexs[indexplus].extreme != 0 && indexplus > 0 && indexplus < (int)vertexs.size())
    {
        hand_vertexs.push_back(&vertexs[indexplus]);
        centroid_hand.x += vertexs[indexplus].data.x;
        centroid_hand.y += vertexs[indexplus].data.y;
        k++;
        if(vertexs[indexplus].isHull && vertexs[indexplus].valley_distance > FINGER_THRESHOLD) vertexs[indexplus].isFinger = true;
        GetNextIndexVertex(indexplus);
        while(vertexs[indexplus].extreme == 0 )
        {
            hand_vertexs.push_back(&vertexs[indexplus]);
            centroid_hand.x += vertexs[indexplus].data.x;
            centroid_hand.y += vertexs[indexplus].data.y;
            k++;
            if(vertexs[indexplus].isHull && vertexs[indexplus].valley_distance > FINGER_THRESHOLD) vertexs[indexplus].isFinger = true;
            GetNextIndexVertex(indexplus);
        }
        hand_vertexs.push_back(&vertexs[indexplus]);
        centroid_hand.x += vertexs[indexplus].data.x;
        centroid_hand.y += vertexs[indexplus].data.y;
        k++;
        if(vertexs[indexplus].isHull && vertexs[indexplus].valley_distance > FINGER_THRESHOLD) vertexs[indexplus].isFinger = true;
        centroid_hand.x /= k;
        centroid_hand.y /= k;
        return true;
    }
    return false;
}

void Hand::GetNextIndexVertex(int & actual)
{
    actual++;
    if(actual >= (int)vertexs.size())
        actual = 0;
}

void Hand::GetPreviousIndexVertex(int & actual)
{
    actual --;
    if(actual < 0)
        actual = vertexs.size()-1;
}

void Hand::draw(float x, float y)
{
	if(Globals::is_view_enabled)
	{
		for (int i = 0; i < (int)hand_vertexs.size(); i++)
		{
			if( i == 0)
			{
				cvLine(Globals::screen,
					cvPoint( hand_vertexs[hand_vertexs.size()-1]->data.x ,hand_vertexs[hand_vertexs.size()-1]->data.y ) ,
					cvPoint( hand_vertexs[i]->data.x ,hand_vertexs[i]->data.y ),
					CV_RGB(255,255,0));
			}
			else
			{
				cvLine(Globals::screen,
					cvPoint( hand_vertexs[i-1]->data.x ,hand_vertexs[i-1]->data.y ) ,
					cvPoint( hand_vertexs[i]->data.x ,hand_vertexs[i]->data.y ),
					CV_RGB(255,255,0));
			}
			if(hand_vertexs[i]->isFinger)
			{
				cvCircle(Globals::screen,cvPoint(hand_vertexs[i]->data.x, hand_vertexs[i]->data.y),20,CV_RGB(255,255,255));
				cvLine(Globals::screen,
					cvPoint( centroid.x , centroid.y) ,
					cvPoint( hand_vertexs[i]->data.x ,hand_vertexs[i]->data.y ),
					CV_RGB(255,255,255));
			}
			if(hand_vertexs[i]->isValley)
			{
				cvCircle(Globals::screen,cvPoint(hand_vertexs[i]->data.x, hand_vertexs[i]->data.y),20,CV_RGB(0,0,255));
				cvLine(Globals::screen,
					cvPoint( centroid.x , centroid.y) ,
					cvPoint( hand_vertexs[i]->data.x ,hand_vertexs[i]->data.y ),
					CV_RGB(0,0,255));
			}
		}
		for (int i = 0; i < (int)hull_vertexs.size(); i++)
		{
			if( i == 0)
			{
				cvLine(Globals::screen,
					cvPoint( hull_vertexs[hull_vertexs.size()-1]->data.x ,hull_vertexs[hull_vertexs.size()-1]->data.y ) ,
					cvPoint( hull_vertexs[i]->data.x ,hull_vertexs[i]->data.y ),
					CV_RGB(255,0,0));
			}
			else
			{
				cvLine(Globals::screen,
					cvPoint( hull_vertexs[i-1]->data.x ,hull_vertexs[i-1]->data.y ) ,
					cvPoint( hull_vertexs[i]->data.x ,hull_vertexs[i]->data.y ),
					CV_RGB(255,0,0));
			}
		}
	}
    /*ofNoFill();
    ofBeginShape();
    for (int i = 0; i < (int)hand_vertexs.size(); i++)
    {
        ofVertex(x + hand_vertexs[i]->data.x, y + hand_vertexs[i]->data.y);
        ofFill();
        if(hand_vertexs[i]->isFinger)
        {
            ofSetColor(0xffffff);
            ofCircle(x + hand_vertexs[i]->data.x, y + hand_vertexs[i]->data.y, 2);
            ofLine(x + centroid.x, y + centroid.y,x + hand_vertexs[i]->data.x, y + hand_vertexs[i]->data.y );
        }
    }
    ofSetColor(0xffff00);
    ofNoFill();
    ofEndShape(true);

    ofSetColor(0xff0000);
    ofNoFill();
    ofBeginShape();
    for (unsigned int i = 0; i < hull_vertexs.size(); i++)
    {
        ofVertex(x + hull_vertexs[i]->data.x, y + hull_vertexs[i]->data.y);
    }
    ofEndShape(true);

    ofFill();
    ofSetColor(0xff0000);
    ofCircle(x + centroid_hand.x, y + centroid_hand.y, 5);

    ofFill();
    ofSetColor(0x00ff00);
    ofCircle(x + from.x, y + from.y, 5);

    ofFill();
    ofSetColor(0x00ff00);
    ofCircle(x + to.x, y + to.y, 5);

    ofLine(x + from.x, y + from.y,x + centroid.x, y + centroid.y);

    ofSetColor(0xff00ff);
    ofCircle(x + centroid.x, y + centroid.y, 5);

    ofSetColor(0x0000ff);
    char reportStr[10];
	sprintf(reportStr, "%i", (int)sessionID);
    ofDrawBitmapString(reportStr, x + centroid.x, y + centroid.y);*/
}
