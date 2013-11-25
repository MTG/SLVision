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

#include "Hand.h"
#include "Fiducial.h"
#include "Globals.h"

	Hand::Hand(void)
	{
		Reset();
		centroid = cv::Point(-1,-1);
	}

	Hand::Hand(unsigned long _sessionID, const cv::Point & _centroid)
	{
		Reset();
		sessionID = _sessionID;
		centroid = cv::Point(_centroid);
		center_hand = cv::Point(_centroid);
	}

	bool Hand::IsItTheSame( cv::Point &point )
	{
		if(fabsf(insqdist(centroid.x, centroid.y, point.x, point.y)) <= HAND_CENTROID_SIMILARITY)
			return true;
		return false;
	}

	void Hand::Reset()
	{
		sessionID = 0;
		startarm = cv::Point(0,0);
		center_hand = cv::Point(0,0);
		fingers[0] = cv::Point(0,0);
		fingers[1] = cv::Point(0,0);
		fingers[2] = cv::Point(0,0);
		fingers[3] = cv::Point(0,0);
		fingers[4] = cv::Point(0,0);
		is_open = false;
		is_confirmed = false;
		is_on_the_surface = false;
		is_updated = false;
		hull.clear();
		blobPath.clear();
		defects.clear();
	}

	void Hand::UpdateData( cv::Point &point, cv::vector<cv::Point> &path  )
	{
		is_updated = true;
		this->center_hand = cv::Point(point);
		this->blobPath = cv::vector<cv::Point>(path);
		//******************************************************
		//* Find convex Hull
		//*******************************************************
		hull.clear();
		cv::convexHull( cv::Mat(blobPath), hull, false );
		//******************************************************
		//* Find defects (Hull valleys)
		//*******************************************************
		defects.clear();
		cv::convexityDefects(cv::Mat(blobPath), hull, defects);
		float x, y;
		x = 0; y = 0;
		int num_vert = 0;
		for(int i = 0; i < defects.size(); i++)
		{
			x += blobPath[defects[i][2]].x;
			y += blobPath[defects[i][2]].y;
			num_vert++;
		}
		//******************************************************
		//* Find centerhand
		//*******************************************************
		x = x / num_vert;
		y = y / num_vert;
		center_hand = cv::Point(x,y);
		//******************************************************
		//* Find fingers and startarm
		//*******************************************************
		float min = 99;
		int edge_index = -1;
		cv::Point edge_point = cv::Point(0,0);
		for(int i = 0; i < defects.size(); i++)
		{
			float tmp = IsNearEdge( blobPath[defects[i][1]] );
			if( tmp < min)
			{
				min = tmp;
				edge_index = defects[i][1];
			}
		}

		if(edge_index == -1) //not a hand (misssed arm)
		{
			is_confirmed = false;
			return;
		}

		//fingers???

		/*for(int i = 0; i < defects.size(); i++)
		{
			if(defects[i][1]  == edge_index)
			{
				cv::line(Globals::CameraFrame,blobPath[defects[i][1]],cv::Point(x,y),cv::Scalar(255,0,0,255),1,CV_AA);
			}
			else
			{

				//float insqdist(float x, float y, float a, float b)
				//if (fabs( nsqdist2(blobPath[defects[i][1]], blobPath[defects[i][2]]) )>50)
				{
					//cv::line(Globals::CameraFrame,blobPath[defects[i][1]],cv::Point(x,y),cv::Scalar(255,255,255,255),1,CV_AA);
					cv::line(Globals::CameraFrame,blobPath[defects[i][1]],blobPath[defects[i][2]],cv::Scalar(255,255,255,255),1,CV_AA);
				}


				
			}
		}*/
	}

	float Hand::IsNearEdge( cv::Point & p )
	{
		float shortest = 9000;
		if(p.x  < shortest) shortest = p.x;
		if(p.y  < shortest) shortest = p.y;
		if(p.x - Globals::CamSize.width >= 0 &&  p.x - Globals::CamSize.width < shortest) shortest = p.x - Globals::CamSize.width;
		if(p.y - Globals::CamSize.height >= 0 &&  p.y - Globals::CamSize.height < shortest) shortest = p.y - Globals::CamSize.height;

		if ( p.x > 10 && p.y > 10 && p.x <= Globals::CamSize.width-10 && p.y <= Globals::CamSize.height-10)
			return 99;
		return shortest;
	}

	void Hand::Draw(bool force)
	{
		if(!force && !is_confirmed) return;
		for(int i = 0; i < blobPath.size(); i++)
		{
			if(i+1 != blobPath.size())
				cv::line(Globals::CameraFrame,blobPath[i],blobPath[i+1],cv::Scalar(0,255,255,255),1,CV_AA);
			else
				cv::line(Globals::CameraFrame,blobPath[i],blobPath[0],cv::Scalar(0,255,255,255),1,CV_AA);
		}
		cv::circle(Globals::CameraFrame,centroid,10,cv::Scalar(50,255,50),5);
		for(int i = 0; i < hull.size(); i++)
		{
			if(i+1 != hull.size())
				cv::line(Globals::CameraFrame,blobPath[hull[i]],blobPath[hull[i+1]],cv::Scalar(0,0,255,255),1,CV_AA);
			else
				cv::line(Globals::CameraFrame,blobPath[hull[i]],blobPath[hull[0]],cv::Scalar(0,0,255,255),1,CV_AA);
		}
		for(int i = 0; i < defects.size(); i++)
		{
			if(i+1 != defects.size())
				cv::line(Globals::CameraFrame,blobPath[defects[i][2]],blobPath[defects[i+1][2]],cv::Scalar(255,0,255,255),1,CV_AA);
			else
				cv::line(Globals::CameraFrame,blobPath[defects[i][2]],blobPath[defects[0][2]],cv::Scalar(255,0,255,255),1,CV_AA);
		}

		cv::circle(Globals::CameraFrame,center_hand,10,cv::Scalar(255,0,0),5);
	}

	unsigned long Hand::GetSID()
	{
		return sessionID;
	}

	bool Hand::IsValid()
	{
		if(centroid.x == -1 && centroid.y == -1)
			return false;
		return true;
	}
/*#include "Globals.h"
#include "Fiducial.h"
#define MIN_DIST_EDGES 10
#define SMOOTH_LWP_CENTER 4
#define FINGER_THRESHOLD 20

Hand::~Hand(void)
{
}

Hand::Hand()
{
	InitVars();
}

Hand::Hand(unsigned long _sessionID, const CvPoint & _centroid)
{
	InitVars();
    sessionID   = _sessionID;
    centroid    = _centroid;
}

void Hand::InitVars()
{
	centroidLwP = cvPoint(-1,-1);
    centroid = cvPoint(-1,-1);
    sessionID = 0;
    area = 0.0f;
    length = 0.0f;
    first_update = true;
    edge = -1;
	is_open = false;
	confirmed_hand = false;
	updated = true;
	is_pinching = false;
	sendEndPinching = false;
	lp_centerhandx = new LowPass(SMOOTH_LWP_CENTER);
	lp_centerhandy = new LowPass(SMOOTH_LWP_CENTER);
	fingers.clear();
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
	updated = true;
}

void Hand::Clear()
{
    vertexs.clear();
    hull_vertexs.clear();
    hand_vertexs.clear();
}

void Hand::AddVertex(int x, int y)
{
    vertexs.push_back(Hand_Vertex(x,y,vertexs.size()));
}

void Hand::AddVertexConvex(int x, int y)
{
    Hand_Vertex* V= GetNearest(x,y);
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

Hand_Vertex* Hand::GetNearest(int x, int y)
{
    float tmpdist;
    float mindist = 99;
    Hand_Vertex* candidate = NULL;
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

Hand_Vertex* Hand::GetNext(Hand_Vertex* v)
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
        Hand_Vertex* from = hull_vertexs[i];
        Hand_Vertex* to;
        if( i != hull_vertexs.size()-1)
            to = hull_vertexs[i+1];
        else
            to = hull_vertexs[0];
        if( from->compute || to->compute)
        {
            Hand_Vertex* iterator = GetNext(from);
            Hand_Vertex* candidate = iterator;
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
	
	///////////////////////////////////////////////////////////
	//calculate hand centroid lowpass
	float cnt = 0;
	centroidLwP.x = 0;
	centroidLwP.y = 0;
	for (int i = 0; i < (int)hand_vertexs.size(); i++)
	{
		if(hand_vertexs[i]->isValley)
		{
			float value = fabsf(insqdist(this->to.x, this->to.y, hand_vertexs[i]->data.x, hand_vertexs[i]->data.y));
			//if(value < 300)
			{
				value = 1-((float)value/300.0f);
				cnt += value;
				centroidLwP.x += hand_vertexs[i]->data.x*value;
				centroidLwP.y += hand_vertexs[i]->data.y*value;
			}
		}
	}
	if(cnt != 0)
	{
		centroidLwP.x /= cnt;
		centroidLwP.y /= cnt;
		centroidLwP.x = lp_centerhandx->addvalue(centroidLwP.x);
		centroidLwP.y = lp_centerhandy->addvalue(centroidLwP.y);
	}
	//////////////////////////////////////////////////////////

}

float Hand::GetArea()
{
    return area;
}

bool Hand::FindHandFrom(int indexplus)
{
    int k = 0;
	int finger_number=0;
	fingers.clear();

    if(vertexs[indexplus].extreme != 0 && indexplus > 0 && indexplus < (int)vertexs.size())
    {
        hand_vertexs.push_back(&vertexs[indexplus]);
        k++;
        if(vertexs[indexplus].isHull && vertexs[indexplus].valley_distance > FINGER_THRESHOLD)
		{
			vertexs[indexplus].isFinger = true;
			fingers.push_back(CvPoint(vertexs[indexplus].data));
			finger_number ++;
		}
        GetNextIndexVertex(indexplus);
        while(vertexs[indexplus].extreme == 0 )
        {
            hand_vertexs.push_back(&vertexs[indexplus]);
            k++;
            if(vertexs[indexplus].isHull && vertexs[indexplus].valley_distance > FINGER_THRESHOLD)
			{
				vertexs[indexplus].isFinger = true;
				fingers.push_back(CvPoint(vertexs[indexplus].data));
				finger_number ++;
			}
            GetNextIndexVertex(indexplus);
        }
        hand_vertexs.push_back(&vertexs[indexplus]);
        k++;
        if(vertexs[indexplus].isHull && vertexs[indexplus].valley_distance > FINGER_THRESHOLD)
		{
			vertexs[indexplus].isFinger = true;
			fingers.push_back(CvPoint(vertexs[indexplus].data));
			finger_number ++;
		}

		///Check Open / Closed Hand
		if(finger_number == 5)
		{
			is_open = true;
			confirmed_hand = true;
		}
		else 
			is_open = false;

        return true;
    }
	is_open = false;
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
		//Draw from - to points and lines
		cvLine(Globals::screen,
			this->from ,
			this->to,
			CV_RGB(255,255,0));
		cvCircle(Globals::screen,
			this->from,
			20,
			CV_RGB(255,0,0));
		cvCircle(Globals::screen,
			this->to,
			20,
			CV_RGB(255,0,255));
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
				cvCircle(Globals::screen,
					cvPoint( hand_vertexs[i]->data.x, hand_vertexs[i]->data.y ),
					20,
					CV_RGB(0,0,255));
				cvLine(Globals::screen,
					cvPoint( centroid.x , centroid.y) ,
					cvPoint( hand_vertexs[i]->data.x ,hand_vertexs[i]->data.y ),
					CV_RGB(0,0,255));
			}
		}
	


		//LWP centroid
		if(is_open)
			cvCircle(Globals::screen,cvPoint(centroidLwP.x, centroidLwP.y),20,CV_RGB(255,0,0),4);
		else
			cvCircle(Globals::screen,cvPoint(centroidLwP.x, centroidLwP.y),20,CV_RGB(255,0,0),1);

		///bad centroid
		if(is_open)
			cvCircle(Globals::screen,cvPoint(centroid.x, centroid.y),20,CV_RGB(0,255,0),4);
		else
			cvCircle(Globals::screen,cvPoint(centroid.x, centroid.y),20,CV_RGB(0,255,0),1);

		//sessionID
		char buffer[100];
		sprintf(buffer,"ID: %d",sessionID);
		if(!confirmed_hand) Globals::Font::Write(Globals::screen,buffer,centroid,FONT_AXIS,255,0,0);
		else Globals::Font::Write(Globals::screen,buffer,centroid,FONT_AXIS,0,255,0);
		
	}
}

bool Hand::IsUpdated()
{
	if(updated)
	{
		updated = false;
		return true;
	}
	return false;
}

int Hand::IsConfirmedAsHand()
{
	if(confirmed_hand)
		return 1;
	return 0;
}

int Hand::IsOpened()
{
	if(is_open)
		return 1;
	return 0;
}

CvPoint Hand::GetCentroid()
{
	return centroid; 
}

float Hand::TCentroidX()
{
	return (float)centroid.x/(float)Globals::width;
}

float Hand::TCentroidY()
{
	return (float)centroid.y/(float)Globals::height;
}

void Hand::SetPinch(CvSeq* seq)
{
	int q = hand_hole.size();
	hand_hole.clear();
	if (seq == NULL)
	{
		is_pinching = false;
		if( q != 0) sendEndPinching = true;
	}
	else
	{
		is_pinching = true;
		CvPoint           pt;
		CvSeqReader       reader;
		cvStartReadSeq( seq, &reader, 0 );
    	for( int j=0; j < seq->total; j++ )
    	{
			CV_READ_SEQ_ELEM( pt, reader );
			hand_hole.push_back(Hand_Vertex(pt.x,pt.y,0));
		}
	}
}

bool Hand::IsPinching()
{
	return is_pinching;
}

bool Hand::IsPinchingEnd()
{
	bool toreturn = sendEndPinching;
	sendEndPinching = false;
	return toreturn;
}

//#include <iostream>
bool Hand::IsFinger(float x, float y)
{
	for(std::vector <Hand_Vertex*>::iterator it = hull_vertexs.begin(); it!= hull_vertexs.end(); it++)
	{
		Hand_Vertex * vert = *it;
		float dist = fabs(sqrt(vert->Distance(x,y)));
		////!!!!!
		//std::cout << dist << std::endl;
		if(dist < 5 ) return true;
	}
	return false;
}*/
