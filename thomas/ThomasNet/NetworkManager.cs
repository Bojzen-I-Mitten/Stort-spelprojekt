using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ThomasEngine.Network
{
    public class NetworkManager : ScriptComponent
    {

        Server server = new Server();
        Client client = new Client();
        public string IP { get; set; } = "localhost";
        public bool Server { get; set; } = false;
//       public int port { get; set; } = 9050;

        public override void Start()
        { 
            if(!Server)
            { 
                client.SetIp(IP);
                client.Start();
                client.ExecuteEvent();
            //   client.SendData("client send data");
            }
            else
            { 
                server.Start();
                server.SendData("You are Connected To the server");
                server.ExecuteEvent();
            }
        }
        public override void Update()
        {
            server.Update();
            client.Update();
        }
        public override void Destroy()
        {
            server.Stop();
            client.Stop();
            base.Destroy();
        }
 


    }
}
