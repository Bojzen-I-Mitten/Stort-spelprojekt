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
    public bool Locked = false;

    #region Throwing stuff
    [Category("Throwing")]
    public float BaseThrowForce { get; set; } = 5.0f;
    [Category("Throwing")]
    public float MaxThrowForce { get; set; } = 20.0f;
    [Category("Throwing")]
    public float maxChargeTime { get; set; } = 4.0f;
    [Category("Throwing")]
    private float ThrowForce;
    [Category("Throwing")]
    public Transform hand { get; set; }
    [Category("Throwing")]
    public float ChargeTime { get; private set; }

    private uint ChargeAnimIndex = 0;
    private uint ThrowAnimIndex = 1;
    private bool HasThrown = false;
    #endregion

    #region Camera Settings etc.
    [Category("Camera Settings")]
    public Camera Camera { get; set; }
    [Category("Camera Settings")]
    public float CameraSensitivity_x { get; set; } = 10;
    [Category("Camera Settings")]
    public float CameraSensitivity_y { get; set; } = 20;
    [Category("Camera Settings")]
    public float CameraMaxVertDegrees { get; set; } = 60;
    [Category("Camera Settings")]
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }
    [Category("Camera Settings")]
    public float TotalYStep { get; private set; } = 0;
    [Category("Camera Settings")]
    public float TotalXStep { get; private set; } = 0;

    // Camera positions
    private Vector3 CameraOffset = new Vector3(0, 1.8f, 1.5f);
    private Vector3 CameraOffsetThrowing = new Vector3(0.8f, 1.6f, 0.8f);
    #endregion

    #region Movement stuff
    public Vector3 Direction; //Right, roll, forward

    public Vector2 CurrentVelocity = new Vector2(0, 0); //Right and forward
    public float Acceleration { get; set; } = 2.0f; //2 m/s^2
    private float BaseSpeed = 3.0f;
    private float MaxSpeed = 10.0f;
    private float DiveSpeed = 12.0f;

    public float DiveTimer { get; private set; } = 0f;
    #endregion

    Rigidbody rBody;
    Chadimations Animations;
    Ragdoll Ragdoll;

    public string PlayerPrefabName { get; set; } = "Chad";
    public float ImpactFactor { get; set; } = 100;
    public float TackleThreshold { get; set; } = 5;
    private float DivingTimer = 0.0f;
    IEnumerator Ragdolling = null;
    IEnumerator Throwing = null;

    public PickupableObject PickedUpObject;

    public override void Start()
    {
        

        State = STATE.CHADING;

        if (isOwner)
            MatchSystem.instance.LocalChad = this;

        if (Camera)
            Camera.gameObject.activeSelf = false;
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
        rBody.Friction = 0.99f;
        Animations = gameObject.GetComponent<Chadimations>();
        Ragdoll = gameObject.GetComponent<Ragdoll>();
        NetworkTransform ragdollSync = gameObject.AddComponent<NetworkTransform>();
        ragdollSync.target = Ragdoll.GetHips().transform;
        ragdollSync.SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;

        Identity.RefreshCache();
    }

    public void DeactivateCamera()
    {
        if (isOwner)
        {
            Camera.gameObject.activeSelf = false;
            Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
        }
            
    }

    public void ActivateCamera()
    {
        if (isOwner)
        {
            Camera.gameObject.activeSelf = true;
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
        }
            
    }

    public override void Update()
    {
        // Debug.Log(State);
        if (isOwner)
        {
            DivingTimer += Time.DeltaTime;
            Direction = new Vector3(0, 0, 0);
            if(State != STATE.RAGDOLL)
            {
                HandleKeyboardInput();
                HandleMouseInput();
            }
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
            State = STATE.RAGDOLL;
            StartCoroutine(Ragdolling);
        }
        if (Input.GetKeyDown(Input.Keys.K))
            gameObject.GetComponent<NetworkPlayer>().Reset();
    }

    private void EnableRagdoll()
    {
        // reset aim stuff 
        ResetCharge();
        Animations.SetAnimationWeight(ChargeAnimIndex, 0);
        ChadHud.Instance.DeactivateCrosshair();
        ChadHud.Instance.DeactivateChargeBar();

        rBody.enabled = false;
        Ragdoll.EnableRagdoll();
    }

    private void DisableRagdoll()
    {
        gameObject.transform.position = Ragdoll.GetHips().transform.position;
        gameObject.transform.eulerAngles = new Vector3(0, Ragdoll.GetHips().transform.localEulerAngles.y, 0);
        Ragdoll.DisableRagdoll();
        gameObject.GetComponent<Rigidbody>().enabled = true;
    }

    public void RPCStartRagdoll(float duration, Vector3 force)
    {
        if(State != STATE.RAGDOLL)
        {
            Ragdolling = StartRagdoll(duration, force);
            State = STATE.RAGDOLL;
            StartCoroutine(Ragdolling);
        }
    }

    public void OnDisconnect()
    {
        if (PickedUpObject)
        {
            if (typeof(Powerup).IsAssignableFrom(PickedUpObject.GetType()))
                (PickedUpObject as Powerup).Remove();
            else
                PickedUpObject.Drop();
        }
            
    }


    public void PublicStartRagdoll(float duration, Vector3 force)
    {
        RPCStartRagdoll(duration, force);
        SendRPC("RPCStartRagdoll", duration, force);
    }

    #region Input handling
    private void HandleKeyboardInput()
    {
        if (Input.GetKeyUp(Input.Keys.Escape))
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
            //Escape = true;
        }

        if (Locked)
            return;

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
            CurrentVelocity.y = DiveSpeed;
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
            if (HasThrowableObject())
            {
                if (Input.GetMouseButtonDown(Input.MouseButtons.RIGHT))
                {
                    State = STATE.THROWING;
                    ChadHud.Instance.ActivateCrosshair();
                    ChadHud.Instance.ActivateChargeBar();

                    Animations.SetAnimationWeight(ChargeAnimIndex, 1);
                }
                else if (Input.GetMouseButton(Input.MouseButtons.RIGHT) && !HasThrown)
                {
                    ChargeObject();
                    if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT) && State == STATE.THROWING)
                    {
                        HasThrown = true;
                        ChadHud.Instance.DeactivateCrosshair();
                        ChadHud.Instance.DeactivateChargeBar();
                        Animations.SetAnimationWeight(ChargeAnimIndex, 0);
                        Throwing = PlayThrowAnim();
                        StartCoroutine(Throwing);
                    }
                }
                else if (Input.GetMouseButtonUp(Input.MouseButtons.RIGHT) && State == STATE.THROWING && Throwing == null)
                {
                    State = STATE.CHADING;
                    ResetCharge();
                    ResetCamera();
                    ChadHud.Instance.DeactivateCrosshair();
                    ChadHud.Instance.DeactivateChargeBar();
                    Animations.SetAnimationWeight(ChargeAnimIndex, 0);
                }
                else if (Input.GetKeyDown(Input.Keys.Space) && Input.GetMouseButton(Input.MouseButtons.RIGHT) && DivingTimer > 5.0f)
                {
                    State = STATE.DIVING;
                    ResetCharge();
                    ResetCamera();
                    Animations.SetAnimationWeight(ChargeAnimIndex, 0);
                }
            }
            else if (PickedUpObject) // player is holding object that is not throwable
            {
                if(Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                    PickedUpObject.OnActivate();
            }

            float xStep = Input.GetMouseX() * Time.ActualDeltaTime;
            float yStep = Input.GetMouseY() * Time.ActualDeltaTime;

            Direction.y = xStep;
            if ((!Input.GetKey(Input.Keys.LeftShift) && State != STATE.THROWING) || State == STATE.DIVING || State == STATE.CHADING)
            {
                FondleCamera(CurrentVelocity.Length(), xStep, yStep);
            }
            else if (State == STATE.THROWING)
            {
                ThrowingCamera(CurrentVelocity.Length(), xStep, yStep);
            }
            else if (Input.GetKeyDown(Input.Keys.LeftShift))
            {
                InitFreeLookCamera();
            }
            else
            {
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
        PickedUpObject.StopEmitting();
        PickedUpObject.Cleanup();

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
        float modifiedBaseSpeed = PickedUpObject ? PickedUpObject.MovementSpeedModifier * BaseSpeed : BaseSpeed;
        float modifiedMaxSpeed = PickedUpObject ? PickedUpObject.MovementSpeedModifier * MaxSpeed : MaxSpeed;
        switch (State)
        {

            case STATE.CHADING:
                if (Direction.z > 0) // if moving forward
                {
                    Direction.x = 0; // don't allow for strafing
                    Direction.y = 0;
                    if (CurrentVelocity.y == 0)
                        CurrentVelocity.y = modifiedBaseSpeed;
                }
                else if (Direction.z < 0) //if walking backwards
                    CurrentVelocity.y = -modifiedBaseSpeed / 2.0f;

                CurrentVelocity.y += Direction.z * Acceleration * Time.DeltaTime;
                if (Direction.z == 0)
                    CurrentVelocity.y = 0;
                CurrentVelocity.x = Direction.x * modifiedBaseSpeed;

                CurrentVelocity.y = MathHelper.Clamp(CurrentVelocity.y, -modifiedBaseSpeed, modifiedMaxSpeed);

                //rBody.Position = Vector3.Transform(new Vector3(CurrentVelocity.x, 0, CurrentVelocity.y) * Time.DeltaTime, transform.rotation);
                //rBody.LinearVelocity = -Vector3.Transform(new Vector3(CurrentVelocity.x, 0, CurrentVelocity.y) * Time.DeltaTime, transform.rotation);
                break;
            case STATE.THROWING:
                CurrentVelocity.y = Direction.z * modifiedBaseSpeed;
                CurrentVelocity.x = Direction.x * modifiedBaseSpeed;

                //rBody.LinearVelocity = -Vector3.Transform(new Vector3(CurrentVelocity.x, 0, CurrentVelocity.y) * Time.DeltaTime, transform.rotation);
                break;
            case STATE.DIVING:
                Direction = Vector3.Zero;
                CurrentVelocity.x = 0;
                CurrentVelocity.y = modifiedMaxSpeed;
                //rBody.LinearVelocity = - Vector3.Transform(new Vector3(CurrentVelocity.x, 0, CurrentVelocity.y) * Time.DeltaTime, transform.rotation);
                break;
            case STATE.RAGDOLL:
                Camera.transform.rotation = Quaternion.Identity;
                Camera.transform.position = Ragdoll.GetHips().transform.position + new Vector3(0, 1, 3);
                Camera.transform.LookAt(Ragdoll.GetHips().transform);

                break;
        }

        rBody.LinearVelocity = Vector3.Transform(new Vector3(-CurrentVelocity.x, rBody.LinearVelocity.y, -CurrentVelocity.y), transform.rotation);
    }

    public void Reset()
    {
        State = STATE.CHADING;
        StopCoroutine(DivingCoroutine());
        DivingTimer = 5;
        StopCoroutine(Ragdolling);
        CurrentVelocity = Vector2.Zero;
        ResetCamera();
        if(PickedUpObject)
        {
            PickedUpObject.Drop();
            PickedUpObject = null;
        }

        if (isOwner)
            MatchSystem.instance.LocalChad = this;
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
        Camera.transform.SetParent(null, true);
        EnableRagdoll();
        Ragdoll.AddForce(force);
        //yield return new WaitForSeconds(duration);
        yield return new WaitForSeconds(2);
        float timer = 0;
        while(Ragdoll.DistanceToWorld()>=0.02 && timer <15)
        {
            //Debug.Log(Ragdoll.DistanceToWorld());
            timer += Time.DeltaTime;
            yield return null;
        }
        yield return new WaitForSeconds(1);
        DisableRagdoll();
        State = STATE.CHADING;
        ResetCamera();
    }

    IEnumerator PlayThrowAnim()
    {
        Animations.SetAnimationWeight(ThrowAnimIndex, 1);
        Vector3 chosenDirection = Camera.transform.forward * ThrowForce;// new Vector3(Camera.transform.forward.x, Camera.transform.forward.y, Camera.transform.forward.z) * ThrowForce;
        Vector3 ballCamPos = Camera.transform.position;

        if (Camera)
            Camera.transform.localPosition = new Vector3(0.0f, 1.5f, 3.0f); // m a g i c

        

        yield return new WaitForSeconds(0.70f); // animation bound, langa lite _magic_ numbers
        ResetCharge();
        ThrowObject(ballCamPos, chosenDirection);
        HasThrown = false;

        yield return new WaitForSeconds(1.0f);
        if(State != STATE.RAGDOLL)
        {
            State = STATE.CHADING;
            ResetCamera();
            Animations.SetAnimationWeight(ThrowAnimIndex, 0);
        }
        
        Throwing = null;
    }

    #endregion

    private void ChargeObject()
    {
        ChargeTime += Time.DeltaTime;
        ChargeTime = MathHelper.Clamp(ChargeTime, 0, maxChargeTime);

        PickedUpObject.chargeTimeCurrent = ChargeTime;
        PickedUpObject.ChargeEffect();

        ThrowForce = MathHelper.Lerp(BaseThrowForce, MaxThrowForce, ChargeTime/maxChargeTime);
        ChadHud.Instance.ChargeChargeBar(ChargeTime / maxChargeTime);
    }

    private void ThrowObject(Vector3 camPos, Vector3 direction)
    {
        PickedUpObject.Throw(camPos, direction);
        ChadHud.Instance.HideHeldObjectText();
    }

    public void RPCPickup(int id)
    {
        if(State != STATE.RAGDOLL)
        {
            GameObject pickupableObject = NetworkManager.instance.Scene.FindNetworkObject(id)?.gameObject;
            PickupableObject pickupable = pickupableObject.GetComponent<PickupableObject>();
            pickupable.Pickup(this, hand ? hand : transform);
        }
    }

    public bool HasThrowableObject()
    {
        if (PickedUpObject)
            if (PickedUpObject.m_throwable)
                return true;
            else
                return false;
        else
            return false;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        if (initialState)
        {
            int pickedUpObject = reader.GetInt();
            if (pickedUpObject >= 0)
                RPCPickup(pickedUpObject);
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
            writer.Put(PickedUpObject ? PickedUpObject.ID : -1);
        writer.Put((int)State);
        writer.Put(Direction);
        writer.Put(CurrentVelocity);

        return true;
    }


    public override void OnCollisionEnter(Collider collider)
    {
        if (isOwner && State != STATE.RAGDOLL && !Locked)
        {
            PickupableObject pickupable = collider.gameObject.GetComponent<PickupableObject>();
            if (pickupable && PickedUpObject == null)
            {
                if (pickupable.transform.parent == null)
                {
                    TakeOwnership(pickupable.gameObject);
                    SendRPC("RPCPickup", pickupable.ID);
                    RPCPickup(pickupable.ID);
                    
                    ChadHud.Instance.ShowHeldObjectText(pickupable.gameObject.Name);
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

                    if (PickedUpObject && PickedUpObject.DropOnRagdoll)
                    {
                        PickedUpObject.Drop();
                        PickedUpObject = null;
                    }
                        
                }
            }
        }
    }
}
