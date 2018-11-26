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
    private Vector3 ThrowingOffset = new Vector3(1.2f, -0.5f, 1.8f);
    //public Vector3 ThrowingOffset { get; set; } = new Vector3(1.2f, 0.5f, 1.2f);
    private Vector3 ChadHead { get { if (Chad) return Chad.rBody.Position + new Vector3(0, 1.8f, 0); else return new Vector3(0, 0, 0); } }

    private float velocity { get { if (Chad?.rBody) return Chad.rBody.LinearVelocity.z; else return 0; } }
    private float xStep { get { return Input.GetMouseX(); } }
    private float yStep { get { return Input.GetMouseY(); } }

    public float MaxFov { get; set; } = 110;
    private float MinFov;

    public override void Start()
    {
        if (Camera)
            MinFov = Camera.fieldOfView;

        CameraSensitivity_x = 0.5f;
        CameraSensitivity_y = 1.0f;
    }

    public override void Update()
    {
        if (Chad)
        {
            switch (Chad.State)
            {
                case ChadControls.STATE.CHADING:
                case ChadControls.STATE.DIVING:
                    if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
                    {
                        if (!Input.GetKey(Input.Keys.LeftAlt) && !Input.GetKey(Input.Keys.R))
                            FondleCamera();
                        //else if (Input.GetKeyDown(Input.Keys.LeftAlt))
                        //    InitFreeLookCamera();
                        else if (Input.GetKey(Input.Keys.LeftAlt))
                            FreeLookCamera(false);
                        else
                            ReverseCamera();
                    }
                    else
                    {
                        ResetCamera();
                        transform.rotation = Chad.transform.rotation;
                        transform.position = ChadHead + CameraOffset * -transform.forward;
                    }
                    break;
                case ChadControls.STATE.THROWING:
                    if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
                        ThrowingCamera();
                    else
                    {
                        ResetCamera();
                        transform.rotation = Chad.transform.rotation;
                        transform.position = ChadHead + CameraOffset * -transform.forward;
                    }
                    break;
                case ChadControls.STATE.RAGDOLL:
                    {
                        RagdollCamera();
                    }
                    break;
            }
            //Camera.fieldOfView = MinFov + velocity;
            //Camera.fieldOfView = Math.Min(Camera.fieldOfView, MaxFov);
        }
    }

    private void FondleCamera()
    {
        FreeLookCamera(false);
        transform.position = ChadHead + CameraOffset * -transform.forward;
        Chad.rBody.Rotation = Quaternion.CreateFromRotationMatrix(Matrix.CreateLookAt(Vector3.Zero, new Vector3(-transform.forward.x, 0, transform.forward.z), Vector3.Up));
    }

    private void RagdollCamera()
    {
        FreeLookCamera(false);
        transform.position = Chad.Ragdoll.GetHips().transform.position + new Vector3(0, 0.8f, 0) + CameraOffset * -transform.forward; //magic number
    }

    private void ReverseCamera()
    {
        FreeLookCamera(true);
        Chad.rBody.Rotation = Quaternion.CreateFromRotationMatrix(Matrix.CreateLookAt(Vector3.Zero, new Vector3(transform.forward.x, 0, -transform.forward.z), Vector3.Up));
    }

    private void FreeLookCamera(bool reverse)
    {
        TotalXStep -= MathHelper.ToRadians(xStep * CameraSensitivity_x);
        TotalYStep -= MathHelper.ToRadians(yStep * CameraSensitivity_y);
        TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);
        
        if(reverse)
            transform.rotation = Quaternion.CreateFromYawPitchRoll(TotalXStep + MathHelper.Pi, TotalYStep, 0);
        else
            transform.rotation = Quaternion.CreateFromYawPitchRoll(TotalXStep, TotalYStep, 0);

        transform.position = ChadHead + CameraOffset * -transform.forward;
    }

    public void ThrowingCamera()
    {
        float yaw = MathHelper.ToRadians(-xStep * CameraSensitivity_x);
        Chad.rBody.Rotation = Chad.rBody.Rotation * Quaternion.CreateFromAxisAngle(Vector3.Up, yaw);

        TotalYStep -= MathHelper.ToRadians(yStep * CameraSensitivity_y);
        TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);

        transform.position = ChadHead;
        transform.rotation = Chad.rBody.Rotation;
        transform.RotateByAxis(transform.right, TotalYStep);
        transform.position = ChadHead + ThrowingOffset.z * -transform.forward + ThrowingOffset.x * transform.right + ThrowingOffset.y * transform.up;
    }

    public void ResetCamera()
    {
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
