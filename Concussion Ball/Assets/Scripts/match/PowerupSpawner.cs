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
        spawnedPowerup = null;
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {

        writer.Put(timeLeftUntilSpawn);
        writer.Put(hasPowerup);

        return false;
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {
  
        timeLeftUntilSpawn = reader.GetFloat();
        hasPowerup = reader.GetBool();
        
    }

    public void SpawnPowerup()
    {
        if (isOwner)
        {
            if (!hasPowerup)
            {
                spawnedPowerup = MatchSystem.instance.PowerupManager.InstantiatePowerup();
                if (spawnedPowerup)
                {
                    spawnedPowerup.activeSelf = true;
                    Powerup powerup = spawnedPowerup.GetComponent<Powerup>();
                    spawnedPowerup.GetComponent<NetworkIdentity>().Owner = true;
                    powerup.Reset();
                    MatchSystem.instance.SendRPC(powerup.ID, "Reset");

                    powerup.m_rigidBody.Position = transform.position;
                    powerup.m_rigidBody.Rotation = transform.rotation;
                    powerup.transform.position = transform.position;
                    powerup.transform.rotation = transform.rotation;

                    powerup.spawner = this;
                    timeLeftUntilSpawn = spawnInterval;
                    hasPowerup = true;
                    spawnedPowerup.GetComponent<NetworkIdentity>().WriteInitialData();

                }
                else
                {
                    Debug.Log("No available powerup to spawn");
                }
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
