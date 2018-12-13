using System;
using System.ComponentModel;
using ThomasEngine;

public class ChadCam : ScriptComponent
{
    private enum STATE
    {
        FONDLE,
        FREE,
        REVERSE
    }

    private STATE _state;
    private STATE state
    {
        get
        {
            return _state;
        }
        set
        {
            switch (value)
            {
                case STATE.FONDLE:
                    ResetCamera();
                    break;
                case STATE.FREE:
                    PrevXStep = TotalXStep;
                    PrevYStep = TotalYStep;
                    break;
                case STATE.REVERSE:
                    PrevXStep = TotalXStep;
                    PrevYStep = TotalYStep;
                    TotalXStep += MathHelper.Pi;
                    break;
            }
            _state = value;
        }
    }

    public static ChadCam instance;
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
    [Newtonsoft.Json.JsonIgnore]
    public float CameraSensitivity_x
    {
        get
        {
            return CameraSensitivity_y * 16f / 11f;
        }
    }
    [Newtonsoft.Json.JsonIgnore]
    public float CameraSensitivity_y
    {
        get
        {
            if (Chad.State == ChadControls.STATE.THROWING)
                return GUIOptionsMenu.instance.getAim();
            else
                return GUIOptionsMenu.instance.getMovement();
        }
    }

    public float CameraMaxVertDegrees { get; set; } = 60;
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }

    private float TotalYStep { get; set; } = 0;
    private float PrevYStep { get; set; } = 0;
    private float TotalXStep { get; set; } = 0;
    private float PrevXStep { get; set; } = 0;

    public float CameraOffset { get; set; } = 3;
    private Vector3 ThrowingOffsetDirection = new Vector3(0.64f, 0.0f, 0.77f);
    public float ThrowingOffset { get; set; } = 1.56f;
    //public Vector3 ThrowingOffset { get; set; } = new Vector3(1.2f, 0.5f, 1.2f);
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 ChadHead { get { if (Chad) return Chad.rBody.Position + new Vector3(0, 0.8f, 0); else return new Vector3(0, 0, 0); } }

    private Vector2 velocity { get { if (Chad) return Chad.CurrentVelocity; else return new Vector2(0); } }
    private float xStep { get { return Input.GetMouseX() * Time.ActualDeltaTime; } }
    private float yStep { get { return Input.GetMouseY() * Time.ActualDeltaTime; } }

    [Newtonsoft.Json.JsonIgnore]
    public float MaxFov { get; set; } = 77;
    private float MinFov = 70;

    public override void OnAwake()
    {
        instance = this;
        _state = STATE.FONDLE;
    }

    public override void Start()
    {
        //if (Camera)
        //{
        //    MinFov = 70.0f;//Camera.fieldOfView;
        //    MaxFov = 75.0f;
        //}
        //CameraSensitivity_x = 0.5f;
        //CameraSensitivity_y = 1.0f;
    }

    public override void Update()
    {
        if (Chad && !MatchSystem.instance.ReplaySystem.Replaying && CameraMaster.instance.GetState() == CAM_STATE.GAME)
        {
            switch (Chad.State)
            {
                case ChadControls.STATE.CHADING:
                case ChadControls.STATE.DANCING:
                case ChadControls.STATE.DIVING:
                    if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
                    {
                        if (Input.GetKeyUp(Input.Keys.LeftAlt))
                            state = STATE.FONDLE;
                        else if (Input.GetKeyDown(Input.Keys.LeftAlt))
                            state = STATE.FREE;
                        else if (Input.GetKeyUp(Input.Keys.R))
                            state = STATE.FONDLE;
                        else if (Input.GetKeyDown(Input.Keys.R))
                            state = STATE.REVERSE;

                        switch (state)
                        {
                            case STATE.FONDLE:
                                FondleCamera();
                                break;
                            case STATE.FREE:
                                FreeLookCamera();
                                break;
                            case STATE.REVERSE:
                                ReverseCamera();
                                break;
                        }
                    }
                    break;
                case ChadControls.STATE.THROWING:
                    if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
                        ThrowingCamera();
                    break;
                case ChadControls.STATE.RAGDOLL:
                    if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
                        RagdollCamera();
                    break;
            }
            if (velocity.y > 8.5f)
                Camera.fieldOfView = Math.Min(MathHelper.Lerp(Camera.fieldOfView, Camera.fieldOfView + 1.150f * velocity.y, Time.DeltaTime), MaxFov);
            else
                Camera.fieldOfView = Math.Max(MathHelper.Lerp(Camera.fieldOfView, Camera.fieldOfView - 93, Time.DeltaTime), MinFov);
        }
    }

    private void FondleCamera()
    {
        RotateCamera(false);
        Chad.rBody.Rotation = Quaternion.CreateFromRotationMatrix(Matrix.CreateLookAt(Vector3.Zero, new Vector3(-transform.forward.x, 0, transform.forward.z), Vector3.Up));
        if (Chad.CurrentVelocity.y > 0 && xStep * CameraSensitivity_x > MathHelper.Pi / 8)
            Chad.CurrentVelocity.y -= Math.Abs(xStep * CameraSensitivity_x / 50);
        Chad.CurrentVelocity.y = Math.Max(Chad.CurrentVelocity.y, 0);
        Chad.rBody.IgnoreNextTransformUpdate();
    }

    private void RagdollCamera()
    {
        RotateCamera(false);
    }

    private void ReverseCamera()
    {
        RotateCamera(true);
        Chad.rBody.Rotation = Quaternion.CreateFromRotationMatrix(Matrix.CreateLookAt(Vector3.Zero, new Vector3(transform.forward.x, 0, -transform.forward.z), Vector3.Up));
        if (Chad.CurrentVelocity.y > 0 && xStep * CameraSensitivity_x > MathHelper.Pi / 8)
            Chad.CurrentVelocity.y -= Math.Abs(xStep * CameraSensitivity_x / 50);
        Chad.CurrentVelocity.y = Math.Max(Chad.CurrentVelocity.y, 0);
        Chad.rBody.IgnoreNextTransformUpdate();
    }

    private void RotateCamera(bool reverse)
    {
        TotalXStep -= MathHelper.ToRadians(xStep * CameraSensitivity_x);
        TotalYStep -= MathHelper.ToRadians(yStep * CameraSensitivity_y);
        TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);
        transform.rotation = Quaternion.CreateFromYawPitchRoll(TotalXStep, TotalYStep, 0);
    }

    private void FreeLookCamera()
    {
        RotateCamera(false);
    }

    public void ThrowingCamera()
    {
        RotateCamera(false);
        Chad.rBody.Rotation = Quaternion.CreateFromRotationMatrix(Matrix.CreateLookAt(Vector3.Zero, new Vector3(-transform.forward.x, 0, transform.forward.z), Vector3.Up));
    }

    public override void FixedUpdate()
    {
        if (Chad && !MatchSystem.instance.ReplaySystem.Replaying && CameraMaster.instance.GetState() == CAM_STATE.GAME)
        {
            float actualOffset = 0;
            if (Chad.State != ChadControls.STATE.THROWING)
                actualOffset = CameraOffset;
            else
                actualOffset = ThrowingOffset;

            Vector3 chadToCam = Vector3.Normalize(Chad.transform.position - transform.position);
            Ray ray = new Ray(ChadHead, -transform.forward);
            RaycastHit rayInfo;
            int collisionMask = ~Physics.GetCollisionGroupBit("Chad");
            if (Physics.Raycast(ray, out rayInfo, actualOffset, collisionMask))
            {
                actualOffset = rayInfo.distance - 0.1f;
            }

            switch (Chad.State)
            {
                case ChadControls.STATE.THROWING:
                    transform.position = ChadHead + (ThrowingOffsetDirection.z * -transform.forward + ThrowingOffsetDirection.x * transform.right + ThrowingOffsetDirection.y * transform.up) * actualOffset;
                    break;
                case ChadControls.STATE.RAGDOLL:
                    transform.position = Chad.Ragdoll.GetHips().transform.position + new Vector3(0, 0.8f, 0) + actualOffset * -transform.forward; //magic number
                    break;
                default:
                    transform.position = ChadHead - transform.forward * actualOffset;
                    break;
            }
        }
    }

    public void ResetCamera()
    {
        TotalXStep = PrevXStep;
        TotalYStep = PrevYStep;
        PrevXStep = 0;
        PrevYStep = 0;
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
