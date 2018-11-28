using ThomasEngine;

public class SpectatorCam : ScriptComponent
{
    public float Speed { get; set; } = 5;
    private Vector3 Direction = Vector3.Zero;
    private float xStep { get { return Input.GetMouseX() * Time.ActualDeltaTime; } }
    private float yStep { get { return Input.GetMouseY() * Time.ActualDeltaTime; } }
    public float CameraSensitivity_x { get; set; } = 10;
    public float CameraSensitivity_y { get; set; } = 20;
    float pitch = 0;
    float yaw = 0;

    public override void Start()
    {

    }

    public override void Update()
    {
        if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
        {
            Direction = Vector3.Zero;
            if (Input.GetKey(Input.Keys.W))
                Direction.z += 1;
            if (Input.GetKey(Input.Keys.S))
                Direction.z -= 1;
            if (Input.GetKey(Input.Keys.A))
                Direction.x -= 1;
            if (Input.GetKey(Input.Keys.D))
                Direction.x += 1;
            if (Input.GetKey(Input.Keys.Space))
                Direction.y += 1;
            if (Input.GetKey(Input.Keys.LeftControl))
                Direction.y -= 1;

            float speed = Speed * Time.DeltaTime;
            if (Input.GetKey(Input.Keys.LeftShift))
                speed *= 2;

            transform.position += transform.forward * speed * Direction.z;
            transform.position += transform.right * speed * Direction.x;
            transform.position += Vector3.Up * speed * Direction.y;

            yaw -= xStep * CameraSensitivity_x;
            pitch -= yStep * CameraSensitivity_y;
            pitch = MathHelper.Clamp(pitch, -85, 85);
            transform.rotation = Quaternion.CreateFromYawPitchRoll(MathHelper.ToRadians(yaw), MathHelper.ToRadians(pitch), 0);
        }
    }
}
