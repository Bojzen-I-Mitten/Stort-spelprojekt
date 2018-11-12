using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;

public class PowerupManager : ScriptComponent
{
    
    public List<GameObject> Powerups { get; set; } = new List<GameObject>();
    private List<PowerupSpawner> spawnPoints;

    public int NextPowerupID = 1000;

    public override void Start()
    {
        spawnPoints = Object.GetObjectsOfType<PowerupSpawner>();
        
        MatchSystem.instance.SpawnablePrefabs.AddRange(Powerups);
    }

    public override void Update()
    {

    }

    public GameObject InstantiatePowerup(Transform trans)
    {
        GameObject ExistingPowerup = MatchSystem.instance.Scene.FindNetworkObject(NextPowerupID)?.gameObject;
        if (!ExistingPowerup)
        {
            int seed = (int)(MatchSystem.instance.MatchStartTime + NextPowerupID);
            System.Random random = new System.Random(seed);
            int powerupIndex = random.Next(0, Powerups.Count);
            Debug.Log(powerupIndex);
            GameObject prefab = Powerups[powerupIndex];
            ExistingPowerup = MatchSystem.instance.NetworkInstantiate(prefab, trans.position, trans.rotation, false, NextPowerupID);
        }
        NextPowerupID++;
        return ExistingPowerup;
    }

    public void ResetPowerups()
    {
        List<Powerup> activePowerups = Object.GetObjectsOfType<Powerup>();
        for (int i=0; i < activePowerups.Count; i++)
        {
            activePowerups[i].Remove();
        }
        Object.GetObjectsOfType<Powerup>();

        spawnPoints.ForEach(point => point.Free()); //Safety free
        spawnPoints.ForEach(point => point.SpawnPowerup());
    }
}
