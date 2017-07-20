/*
 * jesssoft
 */
using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

class Program
{
	static void Main(string[] args)
	{
		Net net = new Net();

		if (!net.Connect("127.0.0.1", 4000))
			return;

		//
		// login test
		//
		{
			Msg msg = new Msg(32);
			msg.AddUInt32(2);

			net.SendMsg(msg);

			Msg rdMsg = null;
			while ((rdMsg = net.GetMsg()) == null 
			    && net.IsConnected() == true)
				Thread.Sleep(100);

			if (net.IsConnected() == true && rdMsg.GetId() == 2) {
				int ret;
				ret = rdMsg.GetInt32();

				if (ret == 0)
					Console.WriteLine("login success");
				else
					Console.WriteLine("login failed");
			}
		}

		Console.WriteLine("press any key");
		Console.ReadLine();

		net.Shutdown();
		while (net.IsConnected()) {
			Console.WriteLine("wait");
			Thread.Sleep(100);
		}

		net.Close();
	}
}

