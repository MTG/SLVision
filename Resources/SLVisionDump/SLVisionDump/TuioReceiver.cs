/*
 * Daniel Gallardo Grassot
 *   Universitat Pompeu Fabra, Barcelona    <daniel.gallardo@upf.edu>
 *   Microsoft Research, Cambridge          <t-danieg@microsoft.com>
 * Cambridge 2010
 * 
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Collections;

namespace SLVisionDump
{
    public delegate void NewOSCPacket(OscPacket packet);

    /// <summary>
    /// This is the class that receive data from external apps
    /// It opens a UDP socket, and crates a thread to listen the data that it receicves
    /// All messages that it receives must be OSC messages.
    /// It fills a OSCMessage queue to be processed in a future.
    /// </summary>
    class TuioReceiver
    {
        /// <summary>
        /// New OSC Packet received event, it is launched for each received 
        /// message when the paquet queue is being processed.
        /// </summary>
        public event NewOSCPacket newOscPaquet;
        /// <summary>
        /// the message queue
        /// </summary>
        private Queue<OscPacket> paquets_queue;
        /// <summary>
        /// port to listen; connected flag; osc receiver and the instance 
        /// of the tread to launch.
        /// </summary>
        int port;
        bool connected;
        private OSCReceiver receiver;
        private Thread thread;
        /// <summary>
        /// In order to make this class accessible and create only one 
        /// instance of it, it is declared as a singleton class.
        /// </summary>
        #region Constructor and singleton methods
        private static volatile TuioReceiver pinstance = null;
        private static object syncRoot = new Object();

        private TuioReceiver()
        {
        }

        public static TuioReceiver Instance
        {
            get
            {
                if (pinstance == null)
                {
                    lock (syncRoot)
                    {
                        pinstance = new TuioReceiver();
                    }
                }
                return pinstance;
            }
        }
        #endregion

        public int Port
        {
            get { return port; }
        }

        public bool IsConnected
        {
            get { return connected; }
        }

        /// <summary>
        /// Connects the socket and starts the listenning thread
        /// </summary>
        /// <param name="_port">port where the socket will be listenning</param>
        public void connect(int _port)
        {
            port = _port;
            connected = false;
            paquets_queue = new Queue<OscPacket>();

            try
            {
                receiver = new OSCReceiver(port);
                thread = new Thread(new ThreadStart(listen));
                connected = true;
                thread.Start();
            }
            catch (Exception e)
            {
                Console.WriteLine("failed to connect to port " + port);
                Console.WriteLine(e.Message);
            }
        }

        /// <summary>
        /// Disconnects the socket
        /// </summary>
        public void disconnect()
        {
            connected = false;
            if (receiver != null)
            {
                receiver.Close();
            }
            receiver = null;
        }

        /// <summary>
        /// Main loop, it receives oscPackets and add them into the packet queue.
        /// </summary>
        private void listen()
        {
            while (connected)
            {
                try
                {
                    OscPacket packet = receiver.Receive();
                    if (packet != null)
                    {
                        paquets_queue.Enqueue(packet);
                    }
                    else Console.WriteLine("Received null OSC packet.");
                }
                catch (Exception e)
                {
                    Console.WriteLine("Error geting data from TUIO Socket.");
                    Console.WriteLine(e.Message);
                }
            }
        }

        /// <summary>
        /// This method is called when we need to process each received packet.
        /// It is recomendable to call it when we are not writing or reading input data
        /// </summary>
        public void ProcessMessages()
        {
            int end_at_this_point = paquets_queue.Count;
            for (int i = 0; i < end_at_this_point; i++)
            {
                if (newOscPaquet != null) newOscPaquet(paquets_queue.Dequeue());
            }
            paquets_queue.Clear();
        }
    }
}
