using System;
using System.ComponentModel;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    [HideInInspector]
    public class NetworkComponent : ScriptComponent
    {
        protected bool isDirty = false;
        private int prefabID;
        private NetworkIdentity networkIdentity;
        protected float SmoothingFactor = 1.0f / 3;


        protected int ID
        {
            get { return Identity.ID; }
        }

        protected NetworkIdentity Identity
        {
            get
            {
                if (networkIdentity == null)
                {
                    networkIdentity = gameObject.GetComponent<NetworkIdentity>();
                    if (networkIdentity == null)
                    {
                        Debug.Log("There is no networkIdentity on this object.");
                    }
                }
                return networkIdentity;
            }
        }

        protected float SendInterval
        {
            get{return 1.0f/NetworkManager.instance.TICK_RATE;}
        }

        [Browsable(false)]
        public bool isOwner
        {
            get { return Identity ? Identity.Owner : false; } 
        }

        virtual public void OnRead(NetPacketReader reader, bool initialState)
        {
            if (!initialState)
                reader.GetInt();
        }

        virtual public bool OnWrite(NetDataWriter writer, bool initialState)
        {
            if (!initialState)
                writer.Put(0);
            return false;
        }

        virtual public void OnGotOwnership() { }
        virtual public void OnLostOwnership() { }
        
        protected void TakeOwnership(GameObject obj)
        {
            NetworkIdentity networkIdentity = obj.GetComponent<NetworkIdentity>();
            if(networkIdentity != null)
            {
                networkIdentity.Owner = true;
            }
        }

        protected void SendRPC(string methodName, params object[] parameters)
        {
            Identity.SendRPC(methodName, parameters);
        }

        internal void ReadRPC(System.Reflection.MethodInfo methodInfo, NetPacketReader reader)
        {
            System.Reflection.ParameterInfo[] parameterInfos = methodInfo.GetParameters();
            object[] parameters = new object[parameterInfos.Length];
            for (int i = 0; i < parameterInfos.Length; i++)
            {
                Type parameterType = parameterInfos[i].ParameterType;
                object parameter = null;
                if (parameterType == typeof(int))
                    parameter = reader.GetInt();
                else if (parameterType == typeof(float))
                    parameter = reader.GetFloat();
                else if (parameterType == typeof(bool))
                    parameter = reader.GetBool();
                else if (parameterType == typeof(string))
                    parameter = reader.GetString();
                else if (parameterType == typeof(Quaternion))
                    parameter = reader.GetQuaternion();
                else if (parameterType == typeof(Vector2))
                    parameter = reader.GetVector2();
                else if (parameterType == typeof(Vector3))
                    parameter = reader.GetVector3();
                else if (parameterType == typeof(Vector4))
                    parameter = reader.GetVector4();
                else
                {
                    Debug.LogError("RPC error: unsupported type.");
                    return;
                }
                    

                if (parameter != null)
                    parameters[i] = parameter;
            }
            methodInfo.Invoke(this, parameters);
        }
    }


}
