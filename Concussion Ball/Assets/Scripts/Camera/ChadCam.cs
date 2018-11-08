using System;
using System.ComponentModel;
using ThomasEngine;

public class ChadCam : ScriptComponent
{
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public ChadControls Chad
    {
        get
        {
            return MatchSystem.instance?.LocalChad;
        }
    }

    Camera _camera;
    Camera Camera
    {
        get
        {
            if (!_camera)
                _camera = gameObject.GetComponent<Camera>();
            return _camera;
        }
    }

    public float CameraSensitivity_x { get; set; } = 10;
    public float CameraSensitivity_y { get; set; } = 20;

    public float CameraMaxVertDegrees { get; set; } = 60;
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }

    private float TotalYStep = 0;
    private float TotalXStep = 0;

    public float CameraOffset { get; set; } = 3;
    public Vector3 ThrowingOffset { get; set; } = new Vector3(1.2f, 0.5f, 1.2f);
    private Vector3 ChadHead { get { if (Chad) return Chad.rBody.Position + new Vector3(0, 1.8f, 0); else return new Vector3(0, 0, 0); } }

    private float velocity { get { if (Chad?.rBody) return Chad.rBody.LinearVelocity.z; else return 0; } }
    private float xStep { get { return Input.GetMouseX() * Time.ActualDeltaTime; } }
    private float yStep { get { return Input.GetMouseY() * Time.ActualDeltaTime; } }

    public float MaxFov { get; set; } = 110;
    private float MinFov;

    public override void Start()
    {
        if (Camera)
            MinFov = Camera.fieldOfView;
    }

    public override void Update()
    {
        if (Chad)
        {
            switch (Chad.State)
            {
                case ChadControls.STATE.CHADING:
                case ChadControls.STATE.DIVING:
                    if (!Input.GetKey(Input.Keys.LeftShift))
                        FondleCamera();
                    else if (Input.GetKeyDown(Input.Keys.LeftShift))
                        InitFreeLookCamera();
                    else
                        FreeLookCamera();
                    break;
                case ChadControls.STATE.THROWING:
                    ThrowingCamera();
                    break;
                case ChadControls.STATE.RAGDOLL:
                    break;
            }
            //Camera.fieldOfView = MinFov + velocity;
            //Camera.fieldOfView = Math.Min(Camera.fieldOfView, MaxFov);
        }
    }

    public void FondleCamera()
    {
        TotalXStep += MathHelper.ToRadians(-xStep * CameraSensitivity_x);
        //Chad.transform.RotateByAxis(Vector3.Up, yaw);
        Quaternion newRot = Quaternion.CreateFromAxisAngle(Vector3.Up, TotalXStep);
        //Chad.rBody.SetRotation(newRot, true);
        TotalYStep -= MathHelper.ToRadians(yStep * CameraSensitivity_y);
        TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);

        transform.position = ChadHead;
        transform.rotation = Chad.rBody.Rotation;
        transform.RotateByAxis(transform.right, TotalYStep);
        transform.position = ChadHead + CameraOffset * -transform.forward;
    }

    public void FreeLookCamera()
    {
        TotalXStep -= MathHelper.ToRadians(xStep * CameraSensitivity_x);
        TotalYStep -= MathHelper.ToRadians(yStep * CameraSensitivity_y);
        TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);

        transform.position = ChadHead;
        transform.rotation = Chad.transform.rotation;
        transform.RotateByAxis(Vector3.Up, TotalXStep + MathHelper.Pi);
        transform.RotateByAxis(transform.right, TotalYStep);
        transform.position = ChadHead + CameraOffset * -transform.forward;
    }

    public void ThrowingCamera()
    {
        float yaw = MathHelper.ToRadians(-xStep * CameraSensitivity_x);

        Chad.transform.RotateByAxis(Vector3.Up, yaw);

        TotalYStep -= MathHelper.ToRadians(yStep * CameraSensitivity_y);
        TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);

        transform.position = ChadHead;
        transform.rotation = Chad.transform.rotation;
        transform.RotateByAxis(transform.right, TotalYStep);
        transform.position = ChadHead + ThrowingOffset.z * -transform.forward  + ThrowingOffset.x * transform.right + ThrowingOffset.y * transform.up;
    }

    public void InitFreeLookCamera()
    {
        TotalXStep = 0;
        TotalYStep = 0;
    }

    public void ResetCamera()
    {
        if(Chad)
            transform.position = CameraOffset * -Chad.transform.forward;
        transform.LookAt(ChadHead);

        transform.localEulerAngles = new Vector3(0, 0, 0);
        TotalXStep = 0;
        TotalYStep = 0;
    }

    private float ClampCameraRadians(float angle, float min, float max)
    {
        if (angle < -2 * ThomasEngine.MathHelper.Pi)
            angle += 2 * ThomasEngine.MathHelper.Pi;
        if (angle > 2 * ThomasEngine.MathHelper.Pi)
            angle -= 2 * ThomasEngine.MathHelper.Pi;
        return Math.Min(Math.Max(angle, min), max);
    }
}
