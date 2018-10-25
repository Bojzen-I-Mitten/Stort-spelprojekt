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
        if(spawnedPowerup == null)
        {
            
            timeLeftUntilSpawn -= Time.DeltaTime;
            if (timeLeftUntilSpawn < 0.0f)
                SpawnPowerup();
        }
    }

    public void Free()
    {
        spawnedPowerup = null;
    }

    public void SpawnPowerup()
    {
        if(spawnedPowerup == null)
        {
            spawnedPowerup = MatchSystem.instance.PowerupManager.InstantiatePowerup(transform);
            spawnedPowerup.GetComponent<Powerup>().spawner = this;
            timeLeftUntilSpawn = 30.0f;
        }
    }

    public override void OnDrawGizmos()
    {
        BoundingSphere b = new BoundingSphere(Vector3.Zero, 0.25f);
        GUI.SetColor(Color.Orange);
        GUI.SetMatrix(transform.world);
        GUI.DrawBoundingSphere(ref b);
    }
}
