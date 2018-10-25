using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;

public class PowerupManager : ScriptComponent
{
    
    public List<GameObject> Powerups { get; set; } = new List<GameObject>();
    private List<PowerupSpawner> spawnPoints;
    private System.Random random;

    public int NextPowerupID = 1000;

    public override void Start()
    {
        random = new System.Random();
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
            GameObject prefab = Powerups[random.Next(0, Powerups.Count - 1)];
            ExistingPowerup = MatchSystem.instance.NetworkInstantiate(prefab, trans.position, trans.rotation, false, NextPowerupID);
        }
        NextPowerupID++;
        return ExistingPowerup;
    }

    public void ResetPowerups()
    {
        List<Powerup> activePowerups = Object.GetObjectsOfType<Powerup>();
        Debug.Log(activePowerups.Count);
        for (int i=0; i < activePowerups.Count; i++)
        {
            activePowerups[i].Remove();
        }
        Object.GetObjectsOfType<Powerup>();

        spawnPoints.ForEach(point => point.SpawnPowerup());
    }
}
