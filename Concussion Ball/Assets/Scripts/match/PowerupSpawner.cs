using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class PowerupSpawner : NetworkComponent
{
    private bool hasPowerup = false;
    float spawnInterval = 30.0f;
    float timeLeftUntilSpawn = 0.0f;
    public override void Start()
    {

    }

    public override void Update()
    {
        if (isOwner)
        {
            if (!hasPowerup)
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
        hasPowerup = false;
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        if(initialState)
        {
            writer.Put(timeLeftUntilSpawn);
            writer.Put(hasPowerup);
            return true;
        }
        return false;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        if (initialState)
        {
            timeLeftUntilSpawn = reader.GetFloat();
            hasPowerup = reader.GetBool();
        }
    }

    public void SpawnPowerup()
    {
        if (isOwner)
        {
            if (!hasPowerup)
            {
                GameObject spawnedPowerup = MatchSystem.instance.PowerupManager.InstantiatePowerup(transform);
                spawnedPowerup.GetComponent<Powerup>().spawner = this;
                timeLeftUntilSpawn = spawnInterval;
                hasPowerup = true;
            }
            else
                Debug.Log("Powerup already spawned");
        }

    }

    public override void OnDrawGizmos()
    {
        BoundingSphere b = new BoundingSphere(Vector3.Zero, 0.25f);
        Gizmos.SetColor(Color.Orange);
        Gizmos.SetMatrix(transform.world);
        Gizmos.DrawBoundingSphere(ref b);
    }
}
