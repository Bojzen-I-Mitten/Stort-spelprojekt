using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;

public class PowerupManager : ScriptComponent
{
    
    public List<GameObject> Powerups { get; set; } = new List<GameObject>();
    private List<PowerupSpawner> spawnPoints;

    private List<List<GameObject>> powerupPool = new List<List<GameObject>>();
    public int NextPowerupID = 1000;
    public int PoolSize { get; set; } = 5;
    public override void Awake()
    {
        spawnPoints = Object.GetObjectsOfType<PowerupSpawner>();
        initPowerupPool();
        //MatchSystem.instance.SpawnablePrefabs.AddRange(Powerups);
    }

    void initPowerupPool()
    {
        foreach(GameObject prefab in Powerups)
        {
            List<GameObject> pool = new List<GameObject>(PoolSize);
            for(int i=0; i < PoolSize; i++)
            {
                GameObject powerup = GameObject.Instantiate(prefab);
                powerup.activeSelf = false;
                pool.Add(powerup);
            }
            powerupPool.Add(pool);
        }
    }

    public override void Update()
    {

    }

    public GameObject InstantiatePowerup()
    {
        int seed = (int)(MatchSystem.instance.MatchStartTime + NextPowerupID);
        System.Random random = new System.Random(seed);
        int powerupIndex = random.Next(0, Powerups.Count);
        GameObject powerup = GetAvailablePowerup(powerupIndex);
        NextPowerupID++;
        return powerup;
    }

    private GameObject GetAvailablePowerup(int powerupIndex)
    {
        foreach(GameObject powerup in powerupPool[powerupIndex])
        {
            if (!powerup.activeSelf)
                return powerup;
        }
        return null;
    }

    public void RecyclePowerup(Powerup powerup)
    {
        powerup.gameObject.activeSelf = false;

    }

    public void ResetPowerups()
    {
        List<Powerup> activePowerups = Object.GetObjectsOfType<Powerup>();
        for (int i=0; i < activePowerups.Count; i++)
        {
            if(activePowerups[i].isOwner)
                activePowerups[i].Remove();
        }
        Object.GetObjectsOfType<Powerup>();

        spawnPoints.ForEach(point => { if (point.isOwner) point.Free(); }); //Safety free
        spawnPoints.ForEach(point => point.SpawnPowerup());
    }
}
