////#include "StdAfx.h"
#include "TuioServer.h"
#include <time.h>
#include "GlobalConfig.h"

TuioServer*	TuioServer::pinstance = NULL;

TuioServer &TuioServer::Instance()
{
	if(pinstance == NULL) pinstance = new TuioServer();
	return *pinstance;
}

TuioServer::TuioServer(/*const char* address, int port*/)
{
	//InitializeNetworking();
	long unsigned int ip	= GetHostByName( (datasaver::GlobalConfig::getRef<std::string>("OSC:ADDRESS", "127.0.0.1")).c_str() );
	transmitSocket			= new UdpTransmitSocket(IpEndpointName(ip, datasaver::GlobalConfig::getRef("OSC:PORT", 3333) ));
	
	buffer					= new char[IP_MTU_SIZE];
	packet_stream			= new osc::OutboundPacketStream(buffer,IP_MTU_SIZE);
	messages				= 0;

	//(*packet_stream) << osc::BeginBundleImmediate;
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
	(*packet_stream) << Globals::dim;
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

void TuioServer::AddPointerMessage(unsigned int sid, unsigned int uid, unsigned int cid, float x, float y, float width, float press)
{
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/ptr" ) << (int)sid << (int)uid << (int)cid << x << y << width << press << osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
}

void TuioServer::AddHand(unsigned int sid, int confirmed, int open, float x, float y, float area)
{
	
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/hand" ) << (int)sid << (int)confirmed << (int)open << x << y << area <<osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
}

void TuioServer::AddHandPath(unsigned int sid,std::vector<Hand_Vertex> &path)
{
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/hand/path" ) << (int)sid /*<< (int) path.size()*/;
	for(std::vector<Hand_Vertex>::iterator it = path.begin(); it != path.end(); it++)
	{
		(*packet_stream) << (float)it->GetDistortionatedX() << (float)it->GetDistortionatedY() << (int)it->GetDescription() ;
	}
	(*packet_stream) <<osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
}

void TuioServer::AddHandPinch(unsigned int sid, std::vector<Hand_Vertex> &path)
{
	if(!bundle_started)StartBundle();
	(*packet_stream) << osc::BeginMessage( "/tuio2/hand/pinch" ) << (int)sid /*<< (int) path.size()*/;
	for(std::vector<Hand_Vertex>::iterator it = path.begin(); it != path.end(); it++)
	{
		(*packet_stream) << (float)it->GetDistortionatedX() << (float)it->GetDistortionatedY();
	}
	(*packet_stream) <<osc::EndMessage;
	messages++;
	if(messages > 5) SendBundle();
	//if(packet_stream->Size
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