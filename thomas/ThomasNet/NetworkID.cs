using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    public class NetworkID : ScriptComponent
    {

        public bool Owner { set; get; } = false;
        public int ID { set; get; }

        NetDataWriter m_dataWriter = new NetDataWriter();
        float timeLeftUntilUpdate = 0;

        NetworkManager Manager
        {
            get{return NetworkManager.instance;}
        }

        List<NetworkComponent> networkComponentsCache;
        public override void OnEnable()
        {
            networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
        }
        
        public override void Update()
        {
            if(Owner && Manager.InternalManager.IsRunning)
            {
                timeLeftUntilUpdate -= Time.ActualDeltaTime;
                if(timeLeftUntilUpdate <= 0)
                {
                    timeLeftUntilUpdate = Manager.InternalManager.UpdateTime;
                    WriteFrameData();
                }
            }
        }

        public void WriteFrameData()
        {
            m_dataWriter.Reset();
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnWrite(m_dataWriter, false);
            }
            Manager.InternalManager.SendToAll(m_dataWriter, DeliveryMethod.Sequenced);
        }

        public void WriteAllVars(NetDataWriter writer)
        {
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnWrite(writer, true);
            }
        }

        public void OnUpdateVars(NetPacketReader reader, bool initialState)
        {
            if(initialState && networkComponentsCache == null)
            {
                networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
            }
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnRead(reader, initialState);
            }
        }
    }
}
