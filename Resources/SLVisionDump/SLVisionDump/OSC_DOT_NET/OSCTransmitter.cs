using System;
using System.Net;
using System.Net.Sockets;

namespace SLVisionDump
{
	/// <summary>
	/// OSCTransmitter
	/// </summary>
	public class OSCTransmitter
	{
		protected UdpClient udpClient;
		protected string remoteHost;
		protected int remotePort;

		public OSCTransmitter(string remoteHost, int remotePort)
		{
			this.remoteHost = remoteHost;
			this.remotePort = remotePort;
			Connect();
		}

		public void Connect()
		{
			if(this.udpClient != null) Close();
			this.udpClient = new UdpClient(this.remoteHost, this.remotePort);
		}

		public void Close()
		{
			this.udpClient.Close();
			this.udpClient = null;
		}

		public int Send(OscPacket packet)
		{
			int byteNum = 0;
			byte[] data = packet.BinaryData;
			try 
			{
				byteNum = this.udpClient.Send(data, data.Length);
			}
			catch (Exception e)
			{
                Console.WriteLine(e.Message);
                Console.WriteLine(e.StackTrace);
			}

			return byteNum;
		}

	}
}
