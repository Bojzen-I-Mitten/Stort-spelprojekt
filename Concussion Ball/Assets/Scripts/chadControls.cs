using System.Linq;
using ThomasEngine;
using System;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;
using System.ComponentModel;
using System.Collections;

public class ChadControls : NetworkComponent
{
    public enum STATE
    {
        CHADING,   // Idle/moving
        THROWING,   // player throws ball / power-up, not all power-ups activate this state
        DIVING,    // user got tackled / hit by a power-up
        RAGDOLL,    // user pressed Space to jump tackle

        NUM_STATES
    };
    public STATE State { get; private set; }

    //public int Speed { get; set; } = 5;
    //public float Force { get; set; } = 5;

    #region Throwing stuff
    [Category("Throwing")]
    public float BaseThrowForce { get; set; } = 5.0f;
    [Category("Throwing")]
    public float MaxThrowForce { get; set; } = 20.0f;
    [Category("Throwing")]
    public float ChargeRate { get; set; } = 5.0f;
    [Category("Throwing")]
    private float ThrowForce;
    [Category("Throwing")]
    public Transform hand { get; set; }
    [Category("Throwing")]
    public float ChargeTime { get; private set; }
    #endregion

    #region Camera Settings etc.
    [Category("Camera Settings")]
    public Camera Camera { get; set; }
    [Category("Camera Settings")]
    public float CameraSensitivity_x { get; set; } = 10;
    [Category("Camera Settings")]
    public float CameraSensitivity_y { get; set; } = 20;
    [Category("Camera Settings")]
    public Vector3 CameraOffset { get; set; } = new Vector3(0, 2, 2);
    [Category("Camera Settings")]
    public float CameraMaxVertDegrees { get; set; } = 60;
    [Category("Camera Settings")]
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }
    [Category("Camera Settings")]
    public Vector3 CameraOffsetThrowing { get; set; } = new Vector3(-1.5f, 1.5f, 1.5f);
    [Category("Camera Settings")]
    public float TotalYStep { get; private set; } = 0;
    [Category("Camera Settings")]
    public float TotalXStep { get; private set; } = 0;
    #endregion

    #region Movement stuff
    public Vector3 Direction; //Right, roll, forward

    public Vector2 CurrentVelocity = new Vector2(0, 0); //Right and forward
    public float Acceleration { get; set; } = 2.0f; //2 m/s^2
    private float BaseSpeed = 2.0f;
    private float MaxSpeed = 10.0f;

    public float DiveTimer { get; private set; } = 0f;
    #endregion

    private Quaternion FreeLookDirection;
    Rigidbody rBody;
    Chadimations Animations;

    public string PlayerPrefabName { get; set; } = "Chad";
    public float ImpactFactor { get; set; } = 10;
    public float TackleThreshold { get; set; } = 5;

    //Camera test;
    private Ball _Ball;
    private Ball Ball
    {
        get
        {
            if (!_Ball)
                _Ball = GetObjectsOfType<Ball>().FirstOrDefault();
            return _Ball;
        }
    }
    // private bool canPickupBall = true;

    public override void Start()
    {
        Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);

        State = STATE.CHADING;

        if (!isOwner && Camera)
            Camera.enabled = false;
        if (isOwner && !Camera)
            Debug.LogWarning("Camera not set for player");
        if (Camera)
        {
            Camera.transform.localPosition = CameraOffset;
        }
            


        ThrowForce = BaseThrowForce;

        rBody = gameObject.GetComponent<Rigidbody>();
        if (rBody != null)
            rBody.IsKinematic = !isOwner;
        Animations = gameObject.GetComponent<Chadimations>();
    }

    public override void Update()
    {
        if (isOwner)
        {
            Direction = new Vector3(0, 0, 0);
            HandleKeyboardInput();
            HandleMouseInput();
            StateMachine();
        }

        
    }

    public void RPCStartRagdoll()
    {
        rBody.enabled = false;
        gameObject.GetComponent<Ragdoll>().EnableRagdoll();
    }

    public void RPCStopRagdoll()
    {
        gameObject.transform.position = gameObject.GetComponent<Ragdoll>().GetHips().transform.position;
        gameObject.transform.eulerAngles = new Vector3(0, gameObject.GetComponent<Ragdoll>().GetHips().transform.localEulerAngles.y, 0);
        gameObject.GetComponent<Ragdoll>().DisableRagdoll();
        gameObject.GetComponent<Rigidbody>().enabled = true;
    }

    IEnumerator StartRagdoll(float duration, Vector3 force)
    {
        State = STATE.RAGDOLL;
        Camera.transform.parent = gameObject.GetComponent<Ragdoll>().GetHips().transform;
        RPCStartRagdoll();
        SendRPC("RPCStartRagdoll");
        gameObject.GetComponent<Ragdoll>().AddForce(force);
        yield return new WaitForSeconds(duration);
        RPCStopRagdoll();
        SendRPC("RPCStopRagdoll");
        State = STATE.CHADING;
        ResetCamera();
    }

    #region Input handling
    private void HandleKeyboardInput()
    {
        if (Input.GetKeyUp(Input.Keys.Escape))
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
            //Escape = true;
        }

        if (Input.GetKey(Input.Keys.W))
            Direction.z += 1;
        if (Input.GetKey(Input.Keys.S))
            Direction.z -= 1;

        if (Input.GetKey(Input.Keys.D))
            Direction.x -= 1;
        if (Input.GetKey(Input.Keys.A))
            Direction.x += 1;

        //if (Input.GetKey(Input.Keys.Space))
        //{
        //    State = STATE.DIVING;
        //    StartCoroutine(DivingCoroutine());
        //}
    }

    private void HandleMouseInput()
    {
        //Focus stuff
        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT) /*&& Escape*/)
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            //Escape = false;
        }

        if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
        {
            //Throw stuff
            if (HasBall())
            {
                if (Input.GetMouseButtonDown(Input.MouseButtons.RIGHT))
                {
                    State = STATE.THROWING;
                }
                else if (Input.GetMouseButtonUp(Input.MouseButtons.RIGHT) && State == STATE.THROWING)
                {
                    State = STATE.CHADING;
                    StopCoroutine(ChargingCoroutine());
                    ResetCharge();
                    ResetCamera();
                }
                else if(Input.GetMouseButtonDown(Input.MouseButtons.LEFT))
                {
                    StartCoroutine(ChargingCoroutine());
                }
                else if (Input.GetMouseButton(Input.MouseButtons.LEFT) && State == STATE.THROWING)
                {
                    ChargeBall();
                }
                else if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT) && State == STATE.THROWING)
                {
                    State = STATE.CHADING;
                    ThrowBall();
                    StopCoroutine(ChargingCoroutine());
                    ResetCharge();
                    ResetCamera();
                }
            }

            float xStep = Input.GetMouseX() * Time.ActualDeltaTime;
            float yStep = Input.GetMouseY() * Time.ActualDeltaTime;

            Direction.y = xStep;
            //ChadSM.Direction.Normalize();

            if (!Input.GetKey(Input.Keys.LeftShift) && !Input.GetMouseButton(Input.MouseButtons.RIGHT))
            {
                //Regular cam
                FondleCamera(CurrentVelocity.Length(), xStep, yStep);
            }
            else if (Input.GetMouseButton(Input.MouseButtons.RIGHT) && HasBall())
            {
                //Throwing cam
                ThrowingCamera(CurrentVelocity.Length(), xStep, yStep);
            }
            else if (Input.GetKeyDown(Input.Keys.LeftShift))
            {
                //Free look
                InitFreeLookCamera();
            }
            else
            {
                //Free look
                FreeLookCamera(CurrentVelocity.Length(), xStep, yStep);
            }

            if (Input.GetKeyUp(Input.Keys.LeftShift) && !Input.GetMouseButton(Input.MouseButtons.RIGHT)) //released shift while not throwing
            {
                ResetCamera();
            }
        }
    }

    private void ResetCharge()
    {
        ChargeTime = 0;
        Ball.chargeTimeCurrent = 0;
        Ball.StopEmitting();
    }
    #endregion

    #region Camera controls
    public void FondleCamera(float velocity, float xStep, float yStep)
    {
        if (Camera && State != STATE.RAGDOLL)
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
            Camera.transform.localPosition = Vector3.Transform(CameraOffset, Camera.transform.localRotation);

        }
    }

    public void FreeLookCamera(float velocity, float xStep, float yStep)
    {
        if (Camera)
        {
            TotalXStep -= ThomasEngine.MathHelper.ToRadians(xStep * CameraSensitivity_x);
            TotalYStep -= ThomasEngine.MathHelper.ToRadians(yStep * CameraSensitivity_y);
            TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);

            Quaternion rot = Quaternion.CreateFromYawPitchRoll(TotalXStep, TotalYStep, 0);
            Camera.transform.localRotation = rot;

            Camera.transform.localPosition = Vector3.Transform(CameraOffset, Camera.transform.localRotation);
        }
    }

    public void ThrowingCamera(float velocity, float xStep, float yStep)
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
            Camera.transform.localPosition = CameraOffsetThrowing;
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
    #endregion

    private void StateMachine()
    {
        switch (State)
        {
            case STATE.CHADING:
                if (Direction.z > 0)
                {
                    Direction.x = 0;
                    Direction.y = 0;
                }

                CurrentVelocity.y += Direction.z * Acceleration * Time.DeltaTime;
                if (Direction.z == 0)
                    CurrentVelocity.y = 0;
                CurrentVelocity.x = Direction.x * BaseSpeed;

                CurrentVelocity.y = MathHelper.Clamp(CurrentVelocity.y, -BaseSpeed, MaxSpeed);
                transform.position -= Vector3.Transform(new Vector3(CurrentVelocity.x, 0, CurrentVelocity.y) * Time.DeltaTime, transform.rotation);

                break;
            case STATE.THROWING:
                CurrentVelocity.y = Direction.z * BaseSpeed;
                CurrentVelocity.x = Direction.x * BaseSpeed;

                transform.position -= Vector3.Transform(new Vector3(CurrentVelocity.x, 0, CurrentVelocity.y) * Time.DeltaTime, transform.rotation);
                break;
            case STATE.DIVING:
                Direction = Vector3.Zero;
                CurrentVelocity.x = 0;
                CurrentVelocity.y = MaxSpeed;
                transform.position -= Vector3.Transform(new Vector3(CurrentVelocity.x, 0, CurrentVelocity.y) * Time.DeltaTime, transform.rotation);
                break;
            case STATE.RAGDOLL:
                Camera.transform.rotation = Quaternion.Identity;
                Camera.transform.LookAt(Camera.transform.parent);
                break;
        }
    }

    #region Coroutines
    IEnumerator DivingCoroutine()
    {
        float timer = 1.0f;
        while (timer > 0.0f)
        {
            timer -= Time.DeltaTime;
            yield return null;
        }
        CurrentVelocity.y = BaseSpeed;
        State = STATE.CHADING;
    }

    IEnumerator ChargingCoroutine()
    {
        float timer = 4;
        while (timer > 0)
        {
            timer = MathHelper.Clamp(timer - Time.DeltaTime, 0, 4);
            ChargeTime = 4 - timer;
            Ball.chargeTimeCurrent = ChargeTime;
            yield return null;
        }
    }

    #endregion

    private void ChargeBall()
    {
        Ball.ChargeColor();
        ThrowForce = ChargeRate * ChargeTime;
    }

    private void ThrowBall()
    {
        if (HasBall())
        {
            Ball.Throw(Camera.transform.forward * ThrowForce);
        }
    }

    public void RPCPickupBall()
    {
        if (Ball)
            Ball.Pickup(gameObject, hand ? hand : transform);

    }

    public bool HasBall()
    {
        if (Ball)
            return Ball.isOwner && Ball.PickedUp;
        else
            return false;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        if (initialState)
        {
            bool hasBall = reader.GetBool();
            if (hasBall)
                RPCPickupBall();
        }

        if (isOwner)
        {
            reader.GetInt();
            reader.GetVector3();
            return;
        }
        State = (STATE)reader.GetInt();
        Direction = reader.GetVector3();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        if (initialState)
        {
            writer.Put(HasBall());
        }
        writer.Put((int)State);
        writer.Put(Direction);

        return true;
    }

    public override void OnCollisionEnter(Collider collider)
    {
        if (isOwner)
        {
            if (Ball)
            {
                if (collider.gameObject == Ball.gameObject)
                {
                    if (Ball.transform.parent == null)
                    {
                        TakeOwnership(Ball.gameObject);
                        SendRPC("RPCPickupBall");
                        RPCPickupBall();
                    }

                }
            }
            if (collider.gameObject.Name == PlayerPrefabName)
            {
                float TheirVelocity = collider.gameObject.GetComponent<ChadControls>().CurrentVelocity.Length();
                if (TheirVelocity > TackleThreshold)
                {
                    //toggle ragdoll
                    StartCoroutine(StartRagdoll(5.0f, -collider.gameObject.transform.forward * ImpactFactor));
                }
            }
        }
    }
}
