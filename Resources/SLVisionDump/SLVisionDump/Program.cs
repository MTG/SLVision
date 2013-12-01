using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace SLVisionDump
{
    class Program
    {
        static int port;
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                port = 3333;
                Console.WriteLine("usage: SLVisionDump port");
                Console.WriteLine("\nUsing default port 3333");
                Console.WriteLine();
            }
            else
            {
                port = int.Parse(args[0]);
            }
            Console.WriteLine("#######################################################");
            Console.WriteLine("Second Light vision System OSC Receiver");
            Console.WriteLine("");
            Console.WriteLine("Daniel Gallardo Grassot me@danielgallardo.eu");
            Console.WriteLine("Barcelona 2013");
            Console.WriteLine("#######################################################");

            //Connect receiver
            TuioReceiver tuior = new TuioReceiver();
            tuior.newOscPaquet += new NewOSCPacket(process_packet);
            tuior.connect(port);

            
        }

        static void process_packet(OscPacket packet)
        {
            if (packet.IsBundle())
            {
                ArrayList messages = packet.Values;
                for (int i = 0; i < messages.Count; i++)
                {
                    OSCMessage msg = (OSCMessage)messages[i];
                    if (msg.Address.Contains("t3d"))
                    {
                        ArrayList data = msg.Values;
                        Console.WriteLine("6DoF tag:");
                        Console.WriteLine("\tSID: " + (int)data[0] + "\tuid: " + (int)data[1] + "\tfid: " + (int)data[2]);
                        Console.WriteLine("\tX: " + (float)data[3] + "\tY: " + (float)data[4] + "\tZ: " + (float)data[5]);
                        Console.WriteLine("\tyaw: " + (float)data[6] + "\tpitch: " + (float)data[7] + "\troll: " + (float)data[8]);
                    }
                    else if (msg.Address.Contains("ptr"))
                    {
                        //(int)sid << (int)uid << (int)cid << x << y << width << press <<
                        ArrayList data = msg.Values;
                        Console.WriteLine("Finger:");
                        Console.WriteLine("\tSID: " + (int)data[0] + "\tuid: " + (int)data[1] + "\tcid: " + (int)data[2]);
                        Console.WriteLine("\tX: " + (float)data[3] + "\tY: " + (float)data[4]);
                        Console.WriteLine("\twidth: " + (float)data[5] + "\tpress: " + (float)data[6]);
                    }
                    else if (msg.Address.Contains("hand/path"))
                    {
                        Console.WriteLine("HandPath data");
                    }
                    else if (msg.Address.Contains("hand"))
                    {
                        ArrayList data = msg.Values;
                        Console.WriteLine("Hand:");
                        Console.WriteLine("\tSID: " + (int)data[0]);
                        Console.WriteLine("\tcentroid: (" + (float)data[1] + ",\t " + (float)data[2] + ")\tarea: " + (float)data[3]);
                        Console.WriteLine("\tstartpoint: (" + (float)data[4] + ",\t " + (float)data[5] + ")\tendpoint: (" + (float)data[6] + ",\t " + (float)data[7] + ")");
                        Console.WriteLine("\tHandX: " + (float)data[8] + "\tHandY: " + (float)data[9] + "\thand_influence: " + (float)data[10]);
                        Console.WriteLine("\tPinchX: " + (float)data[11] + "\tPinchY: " + (float)data[12] + "\tpinch_influence: " + (float)data[13]);
                        Console.WriteLine("\tnumfingers: " + (int)data[14]);
                    }
                    else //if (msg.Address.Contains(""))
                    {
                    //    Console.WriteLine(msg.Address);
                    }
                }
            }
        }
    }
}
