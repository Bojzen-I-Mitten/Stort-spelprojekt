using LiteNetLib;
using LiteNetLib.Utils;
using System;
using ThomasEngine;
using ThomasEngine.Network;
public class PowerupSpawner : NetworkComponent
{
    GameObject spawnedPowerup;
    private bool hasPowerup = false;
    float spawnInterval = 30.0f;
    float timeLeftUntilSpawn = 0.0f;
    Vector3 pos;
    public override void Start()
    {
        pos = transform.position;
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


        if (hasPowerup)
        {
            float test = (float)Math.Sin(Time.ElapsedTime);
            float test2 = (float)Math.Cos(Time.DeltaTime);

            spawnedPowerup.transform.position = pos + new Vector3(0, test, 0) / 10;
            spawnedPowerup.transform.localEulerAngles += new Vector3(0, MathHelper.ToDegrees(test2), 0) / 20;
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
                spawnedPowerup = MatchSystem.instance.PowerupManager.InstantiatePowerup(transform);
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
