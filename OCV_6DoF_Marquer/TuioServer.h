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
public:
	static TuioServer &Instance();
	~TuioServer(void);
	void RegisterProcessor(FrameProcessor * processor);
	void StartBundle();
	void Add3DObjectMessage(unsigned int sid, unsigned int uid, unsigned int fid, float x, float y, float z, float yaw, float pitch, float roll);
	void AddPointerMessage(unsigned int sid, unsigned int uid, unsigned int cid, float x, float y, float width, float press);
	void SendBundle();
private:
	TuioServer(/*const char* address, int port*/);
	void AddFrameMessage();
	void AddAliveMessage();
};

