using System;
using System.Net;
using System.Net.Sockets;

namespace SLVisionDump
{
	/// <summary>
	/// OSCReceiver
	/// </summary>
	public class OSCReceiver
	{
		protected UdpClient udpClient;
		protected int localPort;
        

		public OSCReceiver(int localPort)
		{
            this.udpClient = null;
			this.localPort = localPort;
			Connect();
		}

		public void Connect()
		{
			if(this.udpClient != null) Close();
			this.udpClient = new UdpClient(this.localPort);
		}

		public void Close()
		{
			if (this.udpClient!=null) this.udpClient.Close();
			this.udpClient = null;
		}

		public OscPacket Receive()
		{
            if (udpClient != null)
            {
                try
                {
                    IPEndPoint RemoteIpEndPoint = null;
                    byte[] bytes = this.udpClient.Receive(ref RemoteIpEndPoint);
                    if (bytes != null && bytes.Length > 0)
                        return OscPacket.Unpack(bytes);

                }
                catch (Exception e)
                {
                    Console.WriteLine("Error receiving OSC packet from UDP client" + "\n" + e.Message);
                    return null;
                }
            }
			return null;
		}
	}
}
