using System;
using ThomasEngine;

public class HoverScript : ScriptComponent
{
    public float height { get; set; } = 1.0f;
    public float heightSpeed { get; set; } = 1.0f;
    public float rotateSpeed { get; set; } = 1.0f;
    

    public override void Start()
    {
        
    }

    public override void Update()
    {
        gameObject.transform.position = new Vector3(gameObject.transform.position.x, gameObject.transform.position.y + height * (float)Math.Sin(Time.ElapsedTime * heightSpeed), gameObject.transform.position.z);
        gameObject.transform.Rotate(0.0f, 0.0f, Time.DeltaTime * rotateSpeed);

    }
}

