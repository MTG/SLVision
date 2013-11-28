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
	void SendBundle();
	void SendEmptyBundle();

	void Add3DObjectMessage(unsigned int sid, unsigned int uid, unsigned int fid, 
							float x, float y, float z, 
							float yaw, float pitch, float roll);
	
	void Add3DObjectMessage(unsigned int sid, unsigned int uid, unsigned int fid, 
							float x, float y, float z, 
							float yaw, float pitch, float roll,
							float r11, float r12, float r13, 
							float r21, float r22, float r23, 
							float r31, float r32, float r33);
	
	void AddPointerMessage(	unsigned int sid, unsigned int uid, unsigned int cid, 
							float x, float y, float width, float press);
	
	void AddHand(unsigned int sid, 
				 float centroidx, float centroidy, 
				 float area,
				 float start_armx, float start_army,
				 float end_armx, float end_army,
				 float handx, float handy, float hand_influence,
				 float pinchx, float pincy, float pinc_influence,
				 int numfingers);

	void AddHandPath(unsigned int sid, std::vector<cv::Point> &path);
//	void AddHandPinch(unsigned int sid, std::vector<Hand_Vertex> &path);
	/*void AddHandFingers(unsigned int sid, float x1, float y1, 
		float x2, float y2, float x3, float y3, float x4, float y4, float x5, float y5);*/
private:
	TuioServer(/*const char* address, int port*/);
	void AddFrameMessage();
	int AddAliveMessage();
	long getCurrentTime();
};

