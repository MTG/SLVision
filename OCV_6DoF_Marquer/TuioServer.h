#pragma once

#include "osc/OscOutboundPacketStream.h"
#include "ip/NetworkingUtils.h"
#include "ip/UdpSocket.h"
#include "FrameProcessor.h"

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
private:
	TuioServer(/*const char* address, int port*/);
	void AddFrameMessage();
	int AddAliveMessage();
	long getCurrentTime();
};

