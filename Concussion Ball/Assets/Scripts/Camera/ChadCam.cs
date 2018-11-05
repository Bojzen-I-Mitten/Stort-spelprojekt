using System;
using ThomasEngine;

class ChadCam : ScriptComponent
{
    public ChadControls Chad { get { return MatchSystem.instance.LocalChad; } }
    public Camera Camera { get; set; }

    public float CameraSensitivity_x { get; set; } = 10;
    public float CameraSensitivity_y { get; set; } = 20;

    public float CameraMaxVertDegrees { get; set; } = 60;
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }

    public float TotalYStep { get; private set; } = 0;
    public float TotalXStep { get; private set; } = 0;
    
    private Vector3 CameraOffset = new Vector3(0, 1.8f, 1.5f);
    private Vector3 CameraOffsetThrowing = new Vector3(0.8f, 1.6f, 0.8f);

    private float velocity { get { return Chad.rBody.LinearVelocity.Length(); } }
    private float xStep { get { return Input.GetMouseX() * Time.ActualDeltaTime; } }
    private float yStep { get { return Input.GetMouseY() * Time.ActualDeltaTime; } }
    
    public override void Start()
    {
        if (Chad == null)
            Debug.Log("Camera could not find local Chad");
    }

    public override void Update()
    {
        if (Chad != null)
        {
            switch (Chad.State)
            {
                case ChadControls.STATE.CHADING:
                case ChadControls.STATE.DIVING:
                case ChadControls.STATE.RAGDOLL://TODO: maybe change to what it was in chadControls
                    if (!Input.GetKey(Input.Keys.LeftShift))
                        FondleCamera();
                    else
                        FreeLookCamera();
                    break;
                case ChadControls.STATE.THROWING:
                    ThrowingCamera(); 
                    break;
            }
        }
    }

    public void FondleCamera()
    {
        if (Camera && Chad.State != ChadControls.STATE.RAGDOLL)
        {
            Camera.transform.localPosition = Vector3.Zero;
            float yaw = ThomasEngine.MathHelper.ToRadians(-xStep * CameraSensitivity_x);
            if (velocity != 0)
                yaw = ClampCameraRadians(yaw, -1 / velocity, 1 / velocity);

            transform.RotateByAxis(transform.up, yaw);

            TotalXStep -= yaw; //for of freelook
            TotalYStep -= ThomasEngine.MathHelper.ToRadians(yStep * CameraSensitivity_y);
            TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);
            Camera.transform.localRotation = Quaternion.CreateFromAxisAngle(Vector3.Right, TotalYStep);
            Camera.transform.localPosition = Vector3.Transform(CameraOffset, Camera.transform.localRotation) + Chad.transform.position;

        }
    }

    public void FreeLookCamera()
    {
        if (Camera)
        {
            TotalXStep -= ThomasEngine.MathHelper.ToRadians(xStep * CameraSensitivity_x);
            TotalYStep -= ThomasEngine.MathHelper.ToRadians(yStep * CameraSensitivity_y);
            TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);

            Quaternion rot = Quaternion.CreateFromYawPitchRoll(TotalXStep, TotalYStep, 0);
            Camera.transform.localRotation = rot;

            Camera.transform.localPosition = Vector3.Transform(CameraOffset, Camera.transform.localRotation) + Chad.transform.position;
        }
    }

    public void ThrowingCamera()
    {
        if (Camera)
        {
            float yaw = ThomasEngine.MathHelper.ToRadians(-xStep * CameraSensitivity_x);
            if (velocity != 0)
                yaw = ClampCameraRadians(yaw, -1 / velocity, 1 / velocity);
            transform.RotateByAxis(transform.up, yaw);

            TotalXStep -= yaw; //for of freelook
            TotalYStep -= ThomasEngine.MathHelper.ToRadians(yStep * CameraSensitivity_y);
            TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);
            Camera.transform.localRotation = Quaternion.CreateFromAxisAngle(Vector3.Right, TotalYStep);
            Camera.transform.localPosition = CameraOffsetThrowing + Chad.transform.position;
        }
    }

    public void InitFreeLookCamera()
    {
        Camera.transform.localPosition = new Vector3(CameraOffset.x, CameraOffset.y, -CameraOffset.z);
        Camera.transform.LookAt(transform.position + new Vector3(0, CameraOffset.y, 0));

        Camera.transform.localEulerAngles = new Vector3(0, 180, 0);
        TotalXStep = ThomasEngine.MathHelper.Pi;
        TotalYStep = 0;
    }

    public void ResetCamera()
    {
        if (Camera)
        {
            Camera.transform.parent = gameObject.transform;
            Camera.transform.localPosition = CameraOffset;
            Camera.transform.LookAt(transform.position + new Vector3(0, CameraOffset.y, 0));

            Camera.transform.localEulerAngles = new Vector3(0, 0, 0);
            TotalXStep = 0;
            TotalYStep = 0;
        }
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
