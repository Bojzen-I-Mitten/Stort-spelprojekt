using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class PowerupSpawner : NetworkComponent
{
    private GameObject spawnedPowerup = null;
    float spawnInterval = 30.0f;
    float timeLeftUntilSpawn = 0.0f;
    public override void Start()
    {

    }

    public override void Update()
    {
        if (isOwner)
        {
            if (spawnedPowerup == null)
            {
                timeLeftUntilSpawn -= Time.DeltaTime;
                if (timeLeftUntilSpawn < 0.0f)
                    SpawnPowerup();
            }
        }

    }

    public void Free()
    {
        SendRPC("RPCFree");
        RPCFree();
    }

    public void RPCFree()
    {
        timeLeftUntilSpawn = spawnInterval;
        spawnedPowerup = null;
        
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        if(initialState)
        {
            writer.Put(timeLeftUntilSpawn);
            return true;
        }
        return false;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        if (initialState)
        {
            timeLeftUntilSpawn = reader.GetFloat();
        }
    }

    public void SpawnPowerup()
    {
        if (spawnedPowerup == null)
        {
            spawnedPowerup = MatchSystem.instance.PowerupManager.InstantiatePowerup(transform);
            spawnedPowerup.GetComponent<Powerup>().spawner = this;
            timeLeftUntilSpawn = spawnInterval;
        }
        else
            Debug.Log("Powerup already spawned");
    }

    public override void OnDrawGizmos()
    {
        BoundingSphere b = new BoundingSphere(Vector3.Zero, 0.25f);
        GUI.SetColor(Color.Orange);
        GUI.SetMatrix(transform.world);
        GUI.DrawBoundingSphere(ref b);
    }
}
