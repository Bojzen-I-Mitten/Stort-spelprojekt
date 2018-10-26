using ThomasEngine;
using ThomasEngine.Network;
public class RpcTest : NetworkComponent
{
    public override void Start()
    {
        
    }

    public override void Update()
    {
        if (isOwner)
            if (Input.GetKeyDown(Input.Keys.Space))
                SendRPC("RPCHelloWorld", "testString", 5, true);
    }

    public void RPCHelloWorld(string message, int number, bool sendMessage)
    {
        if (sendMessage)
            Debug.Log("Hello! " + message + " number: " + number);
        else
            Debug.Log("sendmessage was false");
    }
}
