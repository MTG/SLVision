#pragma once

#include "osc/OscOutboundPacketStream.h"
#include "ip/NetworkingUtils.h"
#include "ip/UdpSocket.h"
#include "FrameProcessor.h"
#include <vector>
#include "Hand_Vertex.h"

#define IP_MTU_SIZE 4096 //2048//1536
typedef std::vector<FrameProcessor*> Processors;

class TuioServer
{
	UdpTransmitSocket *transmitSocket;

	osc::OutboundPacketStream*		packet_stream;
	char*							buffer;
	int								messages;
	unsigned int					frame_seq;

	bool							bundle_started;
	
	static TuioServer*				pinstance;

	Processors						processors;
	std::vector<unsigned int>		alive;
public:
	static TuioServer &Instance();
	~TuioServer(void);
	void RegisterProcessor(FrameProcessor * processor);
	void StartBundle();
	void Add3DObjectMessage(unsigned int sid, unsigned int uid, unsigned int fid, float x, float y, float z, float yaw, float pitch, float roll);
	void Add3DObjectMessage(unsigned int sid, unsigned int uid, unsigned int fid, float x, float y, float z, float yaw, float pitch, float roll,
		float r11, float r12, float r13, float r21, float r22, float r23, float r31, float r32, float r33);
	void AddPointerMessage(unsigned int sid, unsigned int uid, unsigned int cid, float x, float y, float width, float press);
	void SendBundle();
	void SendEmptyBundle();
	
	void AddHand(unsigned int sid, int confirmed, int open, float x, float y, float area);
	void AddHandPath(unsigned int sid, std::vector<Hand_Vertex> &path);
	void AddHandPinch(unsigned int sid, std::vector<Hand_Vertex> &path);
	/*void AddHandFingers(unsigned int sid, float x1, float y1, 
		float x2, float y2, float x3, float y3, float x4, float y4, float x5, float y5);*/
private:
	TuioServer(/*const char* address, int port*/);
	void AddFrameMessage();
	int AddAliveMessage();
	long getCurrentTime();
};

