using ThomasEngine;
using ThomasEngine.Network;
public class Ball : ScriptComponent
{

    Rigidbody rb;
    public float force { get; set; } = 5.0f;
    GameObject playerThatHasBall;
    public float currentForce = 0;
    private ThrowStrengthVisualizer visualizer;
    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
        //visualizer = Object.GetObjectsOfType<ThrowStrengthVisualizer>()[0];
    }

    public override void Update()
    {
        if (true)
        {
            if (Input.GetKey(Input.Keys.LeftControl) && !rb.enabled)
            {
                currentForce += force * Time.DeltaTime;
                // visualizer.SetStrength(currentForce);
            }

            if (Input.GetKeyUp(Input.Keys.LeftControl) && !rb.enabled)
            {
                transform.parent = null;
                rb.enabled = true;
                rb.AddForce(playerThatHasBall.transform.forward * currentForce, Rigidbody.ForceMode.Impulse);
                playerThatHasBall = null;
                currentForce = 0.0f;
                // visualizer.SetStrength(0);
            }
        }

    }


    public override void OnCollisionEnter(Collider collider)
    {
        if (rb.enabled)
        {
            playerThatHasBall = collider.gameObject;
            rb.enabled = false;
            transform.parent = collider.transform;
            transform.localPosition = new Vector3(0, 2, 0);

            foreach (var player in NetworkManager.instance.players)
                if (player.Value == playerThatHasBall)
                {
                    if (player.Key.EndPoint != null) //is self
                    {
                        TransferOwnerEvent transEvent = new TransferOwnerEvent
                        {
                            netID = gameObject.GetComponent<NetworkID>().ID
                        };
                        NetworkManager.instance.SendEventToPeer(transEvent, LiteNetLib.DeliveryMethod.ReliableOrdered, player.Key);
                    }
                    break;
                }
        }

    }
}
