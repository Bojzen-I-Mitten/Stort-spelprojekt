using System;
using System.ComponentModel;
using ThomasEngine;

public class ChadCam : ScriptComponent
{
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

    public float CameraSensitivity_x { get; set; } = 10;
    public float CameraSensitivity_y { get; set; } = 20;

    public float CameraMaxVertDegrees { get; set; } = 60;
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }
    
    private float TotalYStep { get; set; } = 0;
    private float TotalXStep { get; set; } = 0;

    public float CameraOffset { get; set; } = 3;
    private Vector3 ThrowingOffsetDirection = new Vector3(0.64f, 0.0f, 0.77f);
    public float ThrowingOffset { get; set; } = 1.56f;
    //public Vector3 ThrowingOffset { get; set; } = new Vector3(1.2f, 0.5f, 1.2f);
    private Vector3 ChadHead { get { if (Chad) return Chad.transform.position + new Vector3(0, 1.8f, 0); else return new Vector3(0, 0, 0); } }

    private Vector2 velocity { get { if (Chad) return Chad.CurrentVelocity; else return new Vector2(0); } }
    private float xStep { get { return Input.GetMouseX() * Time.ActualDeltaTime; } }
    private float yStep { get { return Input.GetMouseY() * Time.ActualDeltaTime; } }

    [Newtonsoft.Json.JsonIgnore]
    public float MaxFov { get; set; } = 77;
    private float MinFov = 70;

    public override void OnAwake()
    {
        instance = this;
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
        if (Input.GetKeyDown(Input.Keys.X))
        { 
            Debug.Log(CameraSensitivity_x);
            Debug.Log(CameraSensitivity_y);
        }
        if (Chad && !MatchSystem.instance.ReplaySystem.Replaying)
        {
            CameraSensitivity_x = GUIOptionsMenu.instance.getMovement();
            CameraSensitivity_y = GUIOptionsMenu.instance.getMovement();
            switch (Chad.State)
            {
                case ChadControls.STATE.CHADING:
                case ChadControls.STATE.DANCING:
                case ChadControls.STATE.DIVING:
                    if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
                    {
                        if (!Input.GetKey(Input.Keys.LeftAlt) && !Input.GetKey(Input.Keys.R))
                            FondleCamera();
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
                    {
                        CameraSensitivity_x = GUIOptionsMenu.instance.getAim();
                        CameraSensitivity_y = GUIOptionsMenu.instance.getAim();
                        ThrowingCamera();
                        
                    }
                    else
                    {
                        CameraSensitivity_x = GUIOptionsMenu.instance.getMovement();
                        CameraSensitivity_y = GUIOptionsMenu.instance.getMovement();
                        ResetCamera();
                        transform.rotation = Chad.transform.rotation;
                        transform.position = ChadHead + CameraOffset * -transform.forward;
                    }
                    break;
                case ChadControls.STATE.RAGDOLL:
                    if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
                    {
                        RagdollCamera();
                    }
                    else
                    {
                        ResetCamera();
                        transform.rotation = Chad.transform.rotation;
                        transform.position = ChadHead + CameraOffset * -transform.forward;
                    }
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
        FreeLookCamera(false);
        Chad.rBody.Rotation = Quaternion.CreateFromRotationMatrix(Matrix.CreateLookAt(Vector3.Zero, new Vector3(-transform.forward.x, 0, transform.forward.z), Vector3.Up));
        if (Chad.CurrentVelocity.y > 0)
            Chad.CurrentVelocity.y -= Math.Abs(xStep * CameraSensitivity_x / 50);
        Chad.CurrentVelocity.y = Math.Max(Chad.CurrentVelocity.y, 0);
        Chad.rBody.IgnoreNextTransformUpdate();
    }

    private void RagdollCamera()
    {
        FreeLookCamera(false);
    }

    private void ReverseCamera()
    {
        FreeLookCamera(true);
        Chad.rBody.Rotation = Quaternion.CreateFromRotationMatrix(Matrix.CreateLookAt(Vector3.Zero, new Vector3(transform.forward.x, 0, -transform.forward.z), Vector3.Up));
        if(Chad.CurrentVelocity.y > 0)
            Chad.CurrentVelocity.y -= Math.Abs(xStep * CameraSensitivity_x / 50);
        Chad.CurrentVelocity.y = Math.Max(Chad.CurrentVelocity.y, 0);
        Chad.rBody.IgnoreNextTransformUpdate();
    }

    private void FreeLookCamera(bool reverse)
    {
        TotalXStep -= MathHelper.ToRadians(xStep * CameraSensitivity_x);
        TotalYStep -= MathHelper.ToRadians(yStep * CameraSensitivity_y);
        TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);
        

        if (reverse)
        {
            transform.rotation = Quaternion.CreateFromYawPitchRoll(TotalXStep + MathHelper.Pi, TotalYStep, 0);
        }
        else
        {
            transform.rotation = Quaternion.CreateFromYawPitchRoll(TotalXStep, TotalYStep, 0);
        }
    }


    public void ThrowingCamera()
    {
        FreeLookCamera(false);
        Chad.rBody.Rotation = Quaternion.CreateFromRotationMatrix(Matrix.CreateLookAt(Vector3.Zero, new Vector3(-transform.forward.x, 0, transform.forward.z), Vector3.Up));
    }

    public override void FixedUpdate()
    {
        if (Chad && !MatchSystem.instance.ReplaySystem.Replaying)
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
            if(Physics.Raycast(ray, out rayInfo, actualOffset, collisionMask))
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
