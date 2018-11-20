using System;
using ThomasEngine;
using LiteNetLib;
using LiteNetLib.Utils;


namespace ThomasEngine.Network
{
    public class RpcUtils
    {

        static public void WriteRPCParameters(object[] parameters, NetDataWriter writer)
        {
            for (int i = 0; i < parameters.Length; i++)
            {
                object parameter = parameters[i];
                Type parameterType = parameters[i].GetType();
                if (parameterType == typeof(int))
                    writer.Put((int)parameter);
                else if (parameterType == typeof(float))
                    writer.Put((float)parameter);
                else if (parameterType == typeof(bool))
                    writer.Put((bool)parameter);
                else if (parameterType == typeof(string))
                    writer.Put((string)parameter);
                else if (parameterType == typeof(Quaternion))
                    writer.Put((Quaternion)parameter);
                else if (parameterType == typeof(Vector2))
                    writer.Put((Vector2)parameter);
                else if (parameterType == typeof(Vector3))
                    writer.Put((Vector3)parameter);
                else if (parameterType == typeof(Vector4))
                    writer.Put((Vector4)parameter);
                else if (parameterType == typeof(Color))
                    writer.Put((Color)parameter);
                else
                {
                    Debug.LogError("RPC error: unsupported type.");
                    return;
                }
            }
        }

        static public object[] ReadRPCParameters(System.Reflection.MethodInfo methodInfo, NetDataReader reader)
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
                else if (parameterType == typeof(Color))
                    parameter = reader.GetColor();
                else
                {
                    Debug.LogError("RPC error: unsupported type.");
                    return null;
                }


                if (parameter != null)
                    parameters[i] = parameter;
            }
            return parameters;
        }

    }
}
