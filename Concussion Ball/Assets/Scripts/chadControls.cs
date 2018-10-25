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

    #region Camera Stuff
    [Category("Camera Stuff")]
    public Camera Camera { get; set; }
    [Category("Camera Stuff")]
    public float CameraSensitivity_x { get; set; } = 10;
    [Category("Camera Stuff")]
    public float CameraSensitivity_y { get; set; } = 20;
    [Category("Camera Stuff")]
    public Vector3 CameraOffset { get; set; } = new Vector3(0, 2, 2);
    [Category("Camera Stuff")]
    public float CameraMaxVertDegrees { get; set; } = 60;
    [Category("Camera Stuff")]
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }
    [Category("Camera Stuff")]
    public Vector3 CameraOffsetThrowing { get; set; } = new Vector3(-1.5f, 1.5f, 1.5f);
    [Category("Camera Stuff")]
    public float TotalYStep { get; private set; } = 0;
    [Category("Camera Stuff")]
    public float TotalXStep { get; private set; } = 0;
    [Category("Camera Stuff")]
    public Texture2D BallArrow { get; set; }
    [Browsable(false)]
    private String BallArrowID { get; } = "BallArrow";
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
    Ragdoll Ragdoll;

    public string PlayerPrefabName { get; set; } = "Chad";
    public float ImpactFactor { get; set; } = 100;
    public float TackleThreshold { get; set; } = 5;
    private float DivingTimer = 0.0f;
    IEnumerator Ragdolling = null;

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
        Camera.renderGUI = true;
        Vector3 Arrowpos = Camera.WorldToViewport(Vector3.Zero, Ball.transform.world);
        Camera.AddImage(BallArrowID, BallArrow, new Vector2(Arrowpos.x, Arrowpos.y), false);
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
        Ragdoll = gameObject.GetComponent<Ragdoll>();
        NetworkTransform ragdollSync = gameObject.AddComponent<NetworkTransform>();
        ragdollSync.target = Ragdoll.GetHips().transform;
        ragdollSync.SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;

        Identity.RefreshCache();
    }

    public override void Update()
    {
        if (isOwner)
        {
            DivingTimer += Time.DeltaTime;
            Direction = new Vector3(0, 0, 0);
            HandleKeyboardInput();
            HandleMouseInput();
            StateMachine();
        }

        if (State == STATE.RAGDOLL && !Ragdoll.RagdollEnabled)
            EnableRagdoll();
        else if (State != STATE.RAGDOLL && Ragdoll.RagdollEnabled)
        {
            DisableRagdoll();
        }

        if (Input.GetKeyDown(Input.Keys.L))
        {
            Ragdolling = StartRagdoll(5.0f, (-transform.forward + transform.up * 0.5f) * 2000);
            StartCoroutine(Ragdolling);
        }
        if (Input.GetKeyDown(Input.Keys.K))
            gameObject.GetComponent<NetworkPlayer>().Reset();

        BallIndicator();
    }

    public void EnableRagdoll()
    {

        rBody.enabled = false;
        Ragdoll.EnableRagdoll();
    }

    public void DisableRagdoll()
    {
        gameObject.transform.position = Ragdoll.GetHips().transform.position;
        gameObject.transform.eulerAngles = new Vector3(0, Ragdoll.GetHips().transform.localEulerAngles.y, 0);
        Ragdoll.DisableRagdoll();
        gameObject.GetComponent<Rigidbody>().enabled = true;
    }

    public void RPCStartRagdoll(float duration, Vector3 force)
    {
        if (State != STATE.RAGDOLL)
        {
            Ragdolling = StartRagdoll(duration, force);
            StartCoroutine(Ragdolling);
        }
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

        if (Input.GetKey(Input.Keys.Space) && DivingTimer > 5.0f)
        {
            State = STATE.DIVING;
            StartCoroutine(DivingCoroutine());
            DivingTimer = 0.0f;
        }
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
                    ResetCharge();
                    ResetCamera();
                }
                else if (Input.GetMouseButtonDown(Input.MouseButtons.LEFT))
                {

                }
                else if (Input.GetMouseButton(Input.MouseButtons.LEFT) && State == STATE.THROWING)
                {
                    ChargeBall();
                }
                else if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT) && State == STATE.THROWING)
                {
                    State = STATE.CHADING;
                    ThrowBall();
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

    #region Camera Functions
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

    private void BallIndicator()
    {
        Vector3 screenPos = Camera.WorldToViewport(Ball.transform.localPosition, Ball.transform.world);

        if (!(screenPos.z > 0 && screenPos.z < 1 &&
            screenPos.x > 0 && screenPos.x < Camera.viewport.x &&
            screenPos.y > 0 && screenPos.y < Camera.viewport.y)) //Offscreen check
        {
            //Adjust for center of screen
            Vector3 screenCenter = new Vector3(Camera.viewport, 0) / 2;
            screenPos -= screenCenter;

            //flip coordinates if more than 90 deg away
            if (screenPos.z > 1)
                screenPos *= -1;
            
            float angle = (float)Math.Atan2(-screenPos.y, screenPos.x);
            angle -= MathHelper.PiOver2;

            float cos = (float)Math.Cos(angle);
            float sin = -(float)Math.Sin(angle);

            //screenPos = screenCenter + new Vector3(sin * )

            float m = screenPos.y / screenPos.x;
            //Debug.Log(screenPos.ToString() + '\t' + m);

            Vector3 screenBounds = screenCenter; //473.5, 253.5, 0

            if (screenPos.y > 0)
                screenPos = new Vector3(screenBounds.y / m, screenBounds.y, 0);
            else
                screenPos = new Vector3(-screenBounds.y / m, -screenBounds.y, 0);

            if (screenPos.x > screenBounds.x)
                screenPos = new Vector3(screenBounds.x, screenBounds.x * m, 0);
            else if (screenPos.x < -screenBounds.x)
                screenPos = new Vector3(-screenBounds.x, -screenBounds.x * m, 0);

            //Re-adjust for screen center
            screenPos += screenCenter;
            screenPos -= new Vector3(BallArrow.width, BallArrow.height, 0) / 2;
            //Make sure the arrow is drawn on-screen
            screenPos = Vector3.Clamp(screenPos, Vector3.Zero, new Vector3(Camera.viewport.x - BallArrow.width, Camera.viewport.y - BallArrow.height, 0));
            
            Camera.SetImagePosition(BallArrowID, new Vector2(screenPos.x, screenPos.y));
            Camera.SetImageRotation(BallArrowID, -angle);
        }
        else
        {
            Camera.SetImagePosition(BallArrowID, new Vector2(-1000, -1000));
        }
        /*//Get the ball position in view port coordinates
        Vector3 screenPos = Camera.WorldToViewport(Ball.transform.localPosition, Ball.transform.world);
        //Correct for size of texture
        screenPos -= new Vector3(BallArrow.width, BallArrow.width, 0) /2;
        //flip coordinates if behind us
        if(screenPos.z > 1)
            screenPos *= -1;

        //Clamp to screen edges, with image offset
        screenPos = Vector3.Clamp(screenPos, Vector3.Zero, new Vector3(Camera.viewport - new Vector2(BallArrow.width, BallArrow.width), 0));

        //Draw the arrow at that position
        Camera.SetImagePosition(BallArrowID, new Vector2(screenPos.x, screenPos.y));
        //Rotate ball indicator arrow to point outward from the center of the screen
        Vector2 screenCenter = Camera.viewport / 2;
        //TODO: Rotation
        //Vector2 UnitVectorFromCenter = new Vector2(screenPos.x, screenPos.y);
        //UnitVectorFromCenter.Normalize();
        //Camera.SetImageRotation(BallArrowID, (float)Math.Acos(UnitVectorFromCenter.x));*/
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
                Camera.transform.position = Ragdoll.GetHips().transform.position + new Vector3(0, 1, 3);
                Camera.transform.LookAt(Ragdoll.GetHips().transform);

                break;
        }
    }

    public void Reset()
    {
        State = STATE.CHADING;
        StopCoroutine(DivingCoroutine());
        DivingTimer = 5;
        StopCoroutine(Ragdolling);
        CurrentVelocity = Vector2.Zero;
        ResetCamera();
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

    IEnumerator StartRagdoll(float duration, Vector3 force)
    {
        State = STATE.RAGDOLL;
        Camera.transform.parent = null;
        EnableRagdoll();
        Ragdoll.AddForce(force);
        yield return new WaitForSeconds(duration);
        DisableRagdoll();
        State = STATE.CHADING;
        ResetCamera();
    }

    #endregion

    private void ChargeBall()
    {
        ChargeTime += Time.DeltaTime;
        ChargeTime = MathHelper.Clamp(ChargeTime, 0, 4);
        Ball.chargeTimeCurrent = ChargeTime;
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
            reader.GetVector2();
            return;
        }
        State = (STATE)reader.GetInt();
        Direction = reader.GetVector3();
        CurrentVelocity = reader.GetVector2();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        if (initialState)
        {
            writer.Put(HasBall());
        }
        writer.Put((int)State);
        writer.Put(Direction);
        writer.Put(CurrentVelocity);

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
                Debug.Log(TheirVelocity);
                Debug.Log(CurrentVelocity.Length());
                if (TheirVelocity > TackleThreshold && TheirVelocity > CurrentVelocity.Length())
                {
                    //toggle ragdoll
                    RPCStartRagdoll(5.0f, (collider.gameObject.transform.forward + Vector3.Up * 0.5f) * 2000);
                    SendRPC("RPCStartRagdoll", 5.0f, (collider.gameObject.transform.forward + Vector3.Up * 0.5f) * 2000);
                }
            }
        }
    }
}
