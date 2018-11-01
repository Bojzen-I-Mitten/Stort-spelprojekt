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

    public GameObject InstantiatePowerup(Transform trans)
    {
        //GameObject ExistingPowerup = MatchSystem.instance.Scene.FindNetworkObject(NextPowerupID)?.gameObject;
        //if (!ExistingPowerup)
        //{
        //    int seed = (int)(MatchSystem.instance.MatchStartTime + NextPowerupID);
        //    System.Random random = new System.Random(seed);
        //    int powerupIndex = random.Next(0, Powerups.Count);
        //    Debug.Log(powerupIndex);
        //    GameObject prefab = Powerups[powerupIndex];
        //    ExistingPowerup = MatchSystem.instance.NetworkInstantiate(prefab, trans.position, trans.rotation, false, NextPowerupID);
        //}
        //NextPowerupID++;
        //return ExistingPowerup;

        int seed = (int)(MatchSystem.instance.MatchStartTime + NextPowerupID);
        System.Random random = new System.Random(seed);
        int powerupIndex = random.Next(0, Powerups.Count);
        Debug.Log(powerupIndex);
        GameObject powerup = GetAvailablePowerup(powerupIndex);
        if(powerup)
        {
            powerup.activeSelf = true;
            powerup.transform.position = trans.position;
            powerup.transform.rotation = trans.rotation;
            powerup.GetComponent<NetworkIdentity>().WriteInitialData();
        }
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
            activePowerups[i].Remove();
        }
        Object.GetObjectsOfType<Powerup>();

        spawnPoints.ForEach(point => point.Free()); //Safety free
        spawnPoints.ForEach(point => point.SpawnPowerup());
    }
}
