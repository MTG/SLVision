using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SLVisionDump
{
    class Program
    {
        static int port;
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                port = 1234;
                Console.WriteLine("usage: SLVisionDump port");
                Console.WriteLine("\nUsing default port 1234");
                Console.WriteLine();
            }
            else
            {
                port = int.Parse(args[0]);
            }
            Console.WriteLine("#######################################################");
            Console.WriteLine("Second Light vision System OSC Receiver");
            Console.WriteLine("");
            Console.WriteLine("Daniel Gallardo Grassot bestsheep1@gmail.com");
            Console.WriteLine("Barcelona 2013");
            Console.WriteLine("#######################################################");

            //Connect receiver
            TuioReceiver tuior = TuioReceiver.Instance;
            tuior.connect(port);
            

        }
    }
}
