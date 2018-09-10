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

        public override void Start()
        {
            server.Start();
            client.Start();
            server.SendData("You are Connected To the server");

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
