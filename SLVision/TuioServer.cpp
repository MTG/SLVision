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

#include "TuioServer.h"
#include <time.h>
#include "Globals.h"
#include "GlobalConfig.h"
#include <windows.h>

TuioServer*	TuioServer::pinstance = NULL;

TuioServer &TuioServer::Instance()
{
	if(pinstance == NULL) pinstance = new TuioServer();
	return *pinstance;
}

TuioServer::TuioServer(/*const char* address, int port*/)
{
	long unsigned int ip	= GetHostByName( (datasaver::GlobalConfig::getRef<std::string>("OSC:ADDRESS", "127.0.0.1")).c_str() );
	transmitSocket			= new UdpTransmitSocket(IpEndpointName(ip, datasaver::GlobalConfig::getRef("OSC:PORT", 3333) ));
	
	buffer					= new char[IP_MTU_SIZE];
	packet_stream			= new osc::OutboundPacketStream(buffer,IP_MTU_SIZE);
	messages				= 0;

	bundle_started			= false;
	frame_seq				= 0;
}

TuioServer::~TuioServer(void)
{
	delete packet_stream;
	delete []buffer;
	delete transmitSocket;
}

void TuioServer::RegisterProcessor(FrameProcessor * processor)
{
	processors.push_back(processor);
}

void TuioServer::AddFrameMessage()
{
	(*packet_stream) << osc::BeginMessage( "/tuio2/frm" );
	(*packet_stream) << (int)(frame_seq++);
	(*packet_stream) << getCurrentTime();
	(*packet_stream) << APP_NAME;
	(*packet_stream) << Globals::dim.c_str();
	(*packet_stream) << osc::EndMessage;
}

int TuioServer::AddAliveMessage()	
{
	alive.clear();
	if(!bundle_started)StartBundle();
	int i = 0;
	//processors
	(*packet_stream) << osc::BeginMessage( "/tuio2/alv" );
	for(Processors::iterator it = processors.begin(); it != processors.end(); it++)
	{
		AliveList tmp = (*it)->GetAlive();
		for(AliveList::iterator it2 = tmp.begin(); it2 != tmp.end(); it2++)
		{
			(*packet_stream) << (int)(*it2);
			alive.push_back((int)(*it2));
			i++;
		}
	}
	(*packet_stream) << osc::EndMessage;

	return i;
}

void TuioServer::StartBundle()
{
	if(bundle_started) SendBundle();
	(*packet_stream) << osc::BeginBundleImmediate;
	AddFrameMessage();
	messages = 0;
	bundle_started = true;
}

void TuioServer::Add3DObjectMessage(unsigned int sid, unsigned int uid, unsigned int fid, float x, float y, float z, float yaw, float pitch, float roll)
{
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/t3d" ) << (int)sid << (int)uid << (int)fid << x << y << z << yaw << pitch << roll << osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
}

void TuioServer::Add3DObjectMessage(unsigned int sid, unsigned int uid, unsigned int fid, float x, float y, float z, float yaw, float pitch, float roll,
		float r11, float r12, float r13, float r21, float r22, float r23, float r31, float r32, float r33)
{
	if(!bundle_started)StartBundle();
		(*packet_stream) << osc::BeginMessage( "/tuio2/t3d" ) << (int)sid << (int)uid << (int)fid << x << y << z << yaw << pitch << roll <<
			r11 << r12 << r13 << r21 << r22 << r23 << r31 << r32 << r33 << osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
}

void TuioServer::AddPointerMessage(unsigned int sid, unsigned int uid, unsigned int cid, float x, float y, float width, float press, int is_on_the_air, int handid)
{
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/ptr" ) << (int)sid << (int)uid << (int)cid << x << y << width << press << is_on_the_air << handid << osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
}


void TuioServer::AddHand(unsigned int sid, 
				 float centroidx, float centroidy, 
				 float area,
				 float start_armx, float start_army,
				 float end_armx, float end_army,
				 float handx, float handy, float hand_influence,
				 float pinchx, float pinchy, float pinch_influence,
				 int numfingers)
{
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/hand" ) << 
		(int)sid << centroidx << centroidy << area << 
		start_armx << start_army << end_armx << end_army <<
		handx << handy << hand_influence <<
		pinchx << pinchy <<pinch_influence <<
		(int)numfingers <<osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
}

void TuioServer::AddHandPath(unsigned int sid,std::vector<cv::Point> &path)
{
	
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/hand/path" ) << (int)sid ;
	for(std::vector<cv::Point>::iterator it = path.begin(); it != path.end(); it++)
	{
		(*packet_stream) << (float)it->x << (float)it->y;
	}
	(*packet_stream) <<osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
}

void TuioServer::SendBundle()	
{
	if(messages > 0 && bundle_started)
	{
		AddAliveMessage();
		(*packet_stream) << osc::EndBundle;
		transmitSocket->Send( packet_stream->Data(), packet_stream->Size() );
	}
	else
	{
		int q = alive.size();
		if ( q != AddAliveMessage() )
		{
			(*packet_stream) << osc::EndBundle;
			transmitSocket->Send( packet_stream->Data(), packet_stream->Size() );
		}
	}

	int i = AddAliveMessage();
	packet_stream->Clear();
	bundle_started = false;
	if( i == 0) SendEmptyBundle();
	packet_stream->Clear();
	bundle_started = false;
}

void TuioServer::SendEmptyBundle()
{
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/alv" ) << osc::EndMessage;
	(*packet_stream) << osc::EndBundle;
	transmitSocket->Send( packet_stream->Data(), packet_stream->Size() );
	packet_stream->Clear();
	bundle_started = false;
}

long TuioServer::getCurrentTime() {

	#ifdef WIN32
		long timestamp = GetTickCount();
	#else
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv,&tz);
		long timestamp = (tv.tv_sec*1000)+(tv.tv_usec/1000);
	#endif
	
	return timestamp;
}
