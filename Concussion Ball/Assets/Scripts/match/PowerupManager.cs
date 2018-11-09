using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;
using ThomasEngine.Script;

public class PowerupManager : ScriptComponent
{
    
    public List<GameObject> Powerups { get; set; } = new List<GameObject>();

    private List<List<GameObject>> powerupPool = new List<List<GameObject>>();
    public int NextPowerupID = 1000;
    public int PoolSize { get; set; } = 5;
    public override void OnAwake()
    {
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
        foreach(Powerup p in ScriptUtility.GetComponentsOfType<Powerup>())
        {
            if(p.isOwner)
                p.Remove();
        }
        foreach (PowerupSpawner ps in ScriptUtility.GetComponentsOfType<PowerupSpawner>())
         {
            if (ps.isOwner)
                ps.Free();
            ps.SpawnPowerup();
        }
    }
}
