using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;
namespace ThomasEngine.Network
{
    [HideInInspector]
    public class NetworkComponent : ScriptComponent
    {


        virtual public void Read(NetPacketReader reader)
        {

        }

        virtual public void Write(NetDataWriter writer)
        {

        }
    }


}
