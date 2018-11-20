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
    public bool CanBeTackled = true;
    

    #region Throwing stuff
    [Category("Throwing")]
    private float ThrowForce;
    [Category("Throwing")]
    public Transform hand { get; set; }
    [Category("Throwing")]
    public float ChargeTime { get; private set; }

    private float BaseThrowForce = 10.0f;
    private float MaxThrowForce = 18.0f;

    private uint ChargeAnimIndex = 0;
    private uint ThrowAnimIndex = 1;
    private bool HasThrown = false;
    #endregion

    #region Movement stuff
    public Vector3 Direction; //Right, roll, forward

    public Vector2 CurrentVelocity = new Vector2(0, 0); //Right and forward
    public float Acceleration { get; set; } = 2.0f; //2 m/s^2
    public float BaseSpeed { get; private set; }  = 5.0f;
    public float MaxSpeed { get; private set; } = 10.0f;

    public float DiveTimer { get; private set; } = 0f;
    private bool Landed = false;
    public Quaternion DivingRotation = Quaternion.Identity;
    private float MinimumRagdollTimer = 2.0f;
    #endregion

    [Browsable(false)]
    public Rigidbody rBody { get; private set; }
    Chadimations Animations;
    public Ragdoll Ragdoll;
    ChadCam _camera;
    ChadCam Camera
    {
        get
        {
            if(!_camera)
                _camera = GetObjectsOfType<ChadCam>().FirstOrDefault();
            return _camera;
        }
    }

    [Browsable(false)]
    public NetworkPlayer NetPlayer { get; private set; }

    public float ImpactFactor { get; set; } = 2000;
    public float TackleThreshold { get; set; } = 7;
    private float DivingTimer = 0.0f;
    private float JumpingTimer = 0.0f;
    private bool Jumping = false;

    IEnumerator Ragdolling = null;
    IEnumerator Throwing = null;
    IEnumerator Diving = null;

    public PickupableObject PickedUpObject;
    private float xStep { get { return Input.GetMouseX() * Time.ActualDeltaTime; } }

    public override void Start()
    {
        State = STATE.CHADING;

        if (isOwner)
            MatchSystem.instance.LocalChad = this;
        ThrowForce = BaseThrowForce;
        rBody = gameObject.GetComponent<Rigidbody>();
        NetPlayer = gameObject.GetComponent<NetworkPlayer>();
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
        if (isOwner)
        {
            DivingTimer += Time.DeltaTime;
            JumpingTimer += Time.DeltaTime;

            Direction = new Vector3(0, 0, 0);
            if (State != STATE.RAGDOLL)
            {
                HandleKeyboardInput();
                HandleMouseInput();
                AirHandling();

                //Accelerate fake gravity as it felt too low, playtest
                if (!OnGround() && rBody.LinearVelocity.y < 0 && rBody.LinearVelocity.y > -5.9f && JumpingTimer > 1)
                    rBody.LinearVelocity = rBody.LinearVelocity = Vector3.Transform(new Vector3(rBody.LinearVelocity.x, rBody.LinearVelocity.y-2, rBody.LinearVelocity.z), rBody.Rotation);
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
            ActivateRagdoll(MinimumRagdollTimer, (-transform.forward + transform.up * 0.5f) * 2000);
        }
        if (Input.GetKeyDown(Input.Keys.K))
            NetPlayer.Reset();

        rBody.Friction = 0.5f;
        if (!OnGround())
            rBody.Friction = 0.0f;
    }

    #region Ragdoll handling
    private void EnableRagdoll()
    {
        // reset aim stuff 
        if (Throwing != null)
        {
            StopCoroutine(Throwing);
            Throwing = null;
        }
        if (Diving != null)
        {
            StopCoroutine(Diving);
            Diving = null;
        }

        ResetThrow();

        rBody.enabled = false;
        CanBeTackled = true;//false;
        Ragdoll.EnableRagdoll();
    }

    private void DisableRagdoll()
    {
        gameObject.transform.position = Ragdoll.GetHips().transform.position;
        gameObject.transform.eulerAngles = new Vector3(0, Ragdoll.GetHips().transform.localEulerAngles.y, 0);
        Ragdoll.DisableRagdoll();
        gameObject.GetComponent<Rigidbody>().enabled = true;

        // call coroutine function that sets canragdoll true
        RagdollRecovery();
    }

    public void LocalActivateRagdoll(float duration, Vector3 force, bool diveTackle)
    {
        SendRPC("RPCStartRagdoll", duration, force);
        RPCStartRagdoll(duration, force, diveTackle);
    }

    public void RPCLocalActivateRagdoll(float duration, Vector3 force, bool diveTackle)
    {
        if(MatchSystem.instance.LocalChad)
        {
            MatchSystem.instance.LocalChad.LocalActivateRagdoll(duration, force, diveTackle);
        }
    }

    public void ActivateRagdoll(float duration, Vector3 force)
    {
        if(!isOwner)
        {
            NetPeer peerThatOwnsThisPlayer = MatchSystem.instance.Scene.Players.FirstOrDefault(player => player.Value == Identity).Key;
            if(peerThatOwnsThisPlayer != null)
            {
                MatchSystem.instance.SendRPC(peerThatOwnsThisPlayer, -1, "RPCLocalActivateRagdoll", duration, force);
            }
            RPCStartRagdoll(duration, force, State == STATE.DIVING);
        }
        else
        {
            LocalActivateRagdoll(duration, force, State == STATE.DIVING);
        }
        Ragdoll.Smack();
    }

    public void RPCStartRagdoll(float duration, Vector3 force, bool diveTackle)
    {
        if (isOwner && PickedUpObject && PickedUpObject.DropOnRagdoll)
        {
            if (typeof(Powerup).IsAssignableFrom(PickedUpObject.GetType()))
            {
                Debug.Log("remove");
                (PickedUpObject as Powerup).Remove();
            }
            else
            {
                PickedUpObject.Drop();
                Debug.Log("drop");
            }

        }

        if (State != STATE.RAGDOLL)
        {
            Ragdolling = StartRagdoll(duration, force, diveTackle);
            State = STATE.RAGDOLL;
            StartCoroutine(Ragdolling);
            Camera.InitFreeLookCamera();
        }
    }
    #endregion
    public bool OnGround()
    {
        // Shoots ray down and checks if Chads feet are in air or not
        Vector3 footPosition = new Vector3(gameObject.transform.position.x, gameObject.transform.position.y + 0.1f/*- 0.001f*/, gameObject.transform.position.z);
        RaycastHit hit;
        Physics.Raycast(footPosition, Vector3.Down, out hit);

        if (hit.distance >= 0.2f || hit.distance == 0)
        {
            return false;
        }
        if(JumpingTimer > 0.1f)
            Landed = true;
        return true;
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

    #region Input handling
    private void HandleKeyboardInput()
    {
        if (Input.GetKeyUp(Input.Keys.Escape))
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
        }

        if (Locked)
            return;

        if (Input.GetKey(Input.Keys.W))
        {
            Direction.z = 1 + (CurrentVelocity.y / (MaxSpeed*0.5f));
        }
            
        if (Input.GetKey(Input.Keys.S))
            Direction.z -= 1;
        if (Input.GetKey(Input.Keys.D))
            Direction.x -= 1;
        if (Input.GetKey(Input.Keys.A))
            Direction.x += 1;

        if (Input.GetKeyDown(Input.Keys.LeftShift) && DivingTimer > 5.0f)
        {
            State = STATE.DIVING;
            CurrentVelocity.y += 2.0f;
            Diving = DivingCoroutine();
            StartCoroutine(Diving);
            DivingTimer = 0.0f;
            DivingRotation = this.gameObject.transform.rotation;
        }
        else if (Input.GetKeyDown(Input.Keys.Space))
        {
            if(OnGround())
            {
                JumpingTimer = 0.0f;
                Jumping = true;
                Landed = false;
                DivingRotation = this.gameObject.transform.rotation;
                CurrentVelocity.y = BaseSpeed;
                rBody.LinearVelocity = Vector3.Transform(new Vector3(rBody.LinearVelocity.x, 0, rBody.LinearVelocity.z), rBody.Rotation);
                rBody.AddForce(new Vector3(0, 450, 0), Rigidbody.ForceMode.Impulse);
            }
            else if(Jumping || (!OnGround() && JumpingTimer > 3.0f))
            {
                JumpingTimer = 0.0f;
                Jumping = false;
                Landed = false;
                DivingRotation = this.gameObject.transform.rotation;
                CurrentVelocity.y = BaseSpeed;
                rBody.LinearVelocity = Vector3.Transform(new Vector3(rBody.LinearVelocity.x, 0, rBody.LinearVelocity.z), rBody.Rotation);
                rBody.AddForce(new Vector3(0, 350, 0), Rigidbody.ForceMode.Impulse);
            }
        }
    }

    private void HandleMouseInput()
    {
        //Focus stuff
        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
        }


        if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
        {
            //Throw stuff
            if (HasThrowableObject())
            {
                if (State != STATE.DIVING)
                {
                    if (Input.GetMouseButtonDown(Input.MouseButtons.RIGHT) && !HasThrown)
                    {
                        State = STATE.THROWING;
                        ChadHud.Instance.ActivateAimHUD();

                        Animations.SetAnimationWeight(ChargeAnimIndex, 1);

                    }
                    else if (Input.GetMouseButton(Input.MouseButtons.RIGHT) && !HasThrown && State == STATE.THROWING)
                    {
                        ChargeObject();
                        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                        {
                            HasThrown = true;

                            Throwing = PlayThrowAnim();
                            StartCoroutine(Throwing);
                        }
                    }
                    else if (Input.GetMouseButtonUp(Input.MouseButtons.RIGHT) && State == STATE.THROWING && Throwing == null)
                    {
                        State = STATE.CHADING;
                        ResetThrow();
                    }
                }
                else if (Input.GetKeyDown(Input.Keys.LeftShift) && Input.GetMouseButton(Input.MouseButtons.RIGHT))
                {
                    ResetThrow();
                }
            }
            else if (PickedUpObject) // player is holding object that is not throwable
            {
                if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                    PickedUpObject.OnActivate();
            }
        }
    }
    #endregion

    #region Reset Throw
    public void RPCResetThrow()
    {
        HasThrown = false;
        ChadHud.Instance.DeactivateAimHUD();
        Animations.SetAnimationWeight(ChargeAnimIndex, 0);
        Animations.SetAnimationWeight(ThrowAnimIndex, 0);
        ChargeTime = 0;
        
        if (PickedUpObject)
        {
            PickedUpObject.StopEmitting();
            PickedUpObject.Cleanup();
        }

    }

    private void ResetThrow()
    {
        SendRPC("RPCResetThrow");
        RPCResetThrow();
    }
    #endregion

    private void AirHandling()
    {
        //if (Landed && !OnGround())
        //{
        //    Landed = false;
        //    DivingRotation = this.gameObject.transform.rotation;
        //}
        //else if (!Landed && OnGround() && JumpingTimer > 0.1f)
        //    Landed = true;

        //No random in-air y-directional velocity
        if (!OnGround() && rBody.LinearVelocity.y > 0 && Landed)
            rBody.LinearVelocity = new Vector3(rBody.LinearVelocity.x, 0.0f, rBody.LinearVelocity.z);
    }

    private void StateMachine()
    {
        float modifiedBaseSpeed = PickedUpObject ? PickedUpObject.MovementSpeedModifier * BaseSpeed : BaseSpeed;
        float modifiedMaxSpeed = PickedUpObject ? PickedUpObject.MovementSpeedModifier * MaxSpeed : MaxSpeed;

        float diagonalModifier = 1.0f;

        switch (State)
        {

            case STATE.CHADING:
                // Moving forward
                if (Direction.z > 0)
                {
                    //Direction.x = 0; // comment back if we don't want to allow forward diagonal movement
                    if(Direction.x != 0)
                    {
                        diagonalModifier = 0.5f; // fix more accurately later
                    }
                    Direction.y = 0;

                    // Started to move
                    if (CurrentVelocity.y == 0)
                        CurrentVelocity.y = modifiedBaseSpeed;
                }
                // Walking backwards
                else if (Direction.z < 0) 
                    CurrentVelocity.y = -modifiedBaseSpeed * 0.75f * diagonalModifier;

                if(OnGround() && Direction.z >= 0) // as to not allow acceleration mid air
                    CurrentVelocity.y += Direction.z * Acceleration * diagonalModifier * Time.DeltaTime;

                if (Direction.z == 0)
                    CurrentVelocity.y = 0;

                CurrentVelocity.x = Direction.x * modifiedBaseSpeed * diagonalModifier;
                CurrentVelocity.y = MathHelper.Clamp(CurrentVelocity.y, -modifiedBaseSpeed, modifiedMaxSpeed);
               // CurrentVelocity.y -= Math.Abs(xStep / (MaxSpeed / CurrentVelocity.y)); //TODO:Fix this when diagonal running is added
                break;
            case STATE.THROWING:
                CurrentVelocity.y = Slope(Direction.z, 1) * modifiedBaseSpeed;
                CurrentVelocity.x = Slope(Direction.x, 1) * modifiedBaseSpeed;
                
                break;
            case STATE.DIVING:
                Direction = Vector3.Zero; 
                CurrentVelocity.x = 0;
                break;
            case STATE.RAGDOLL:
                //Camera.transform.rotation = Quaternion.Identity;
                //Camera.transform.position = Ragdoll.GetHips().transform.position + new Vector3(0, 1, 3);
                //Camera.transform.LookAt(Ragdoll.GetHips().transform);
                break;
        }

        if (State != STATE.DIVING && Landed)
            rBody.LinearVelocity = Vector3.Transform(new Vector3(-CurrentVelocity.x, rBody.LinearVelocity.y, -CurrentVelocity.y), rBody.Rotation);
        else
            rBody.LinearVelocity = Vector3.Transform(new Vector3(-CurrentVelocity.x, rBody.LinearVelocity.y, -CurrentVelocity.y), DivingRotation);

        //Temporary animation fix, without this no animation will play when moving diagonally
        if(Direction.z != 0)
            Direction.x = 0;

        //rBody.IgnoreNextTransformUpdate();
    }

    private int Slope(float delta, int absLimit)
    {
        if (delta >= absLimit)
            return absLimit;
        else if (delta <= -absLimit)
            return -absLimit;
        else return 0;
    }

    public void Reset()
    {
        State = STATE.CHADING;
        if (Diving != null)
        {
            StopCoroutine(Diving);
            Diving = null;
        }
        DivingTimer = 5;
        if(Ragdolling != null)
        {
            StopCoroutine(Ragdolling);
            Ragdolling = null;
        }
        
        CurrentVelocity = Vector2.Zero;
        if (PickedUpObject)
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
        Animations.RPCResetTimer(0);
        float timer = 1.5f;
        while (timer > 0.0f)
        {
            timer -= Time.DeltaTime;
            yield return null;
        }
        CurrentVelocity.y = BaseSpeed;
        State = STATE.CHADING;
    }

    IEnumerator StartRagdoll(float duration, Vector3 force, bool diveTackle)
    {
        State = STATE.RAGDOLL;
        
        EnableRagdoll();
        Ragdoll.AddForce(force, diveTackle);

        yield return new WaitForSeconds(duration);
        float timer = 0;
        while (Ragdoll.DistanceToWorld() >= 0.5f && timer < 15)
        {
            timer += Time.DeltaTime;
            yield return null;
        }
        yield return new WaitForSeconds(1);
        DisableRagdoll();
        State = STATE.CHADING;
    }

    IEnumerator RagdollRecovery()
    {
        yield return new WaitForSeconds(2);
        CanBeTackled = true;

        // Recovery particles
    }

    public void RPCSetAnimWeight(int index, float weight)
    {
        Animations.SetAnimationWeight((uint)index, weight);
    }

    public void RPCStartThrow()
    {
        
        Animations.SetAnimationWeight(ChargeAnimIndex, 0);
        Animations.SetAnimationWeight(ThrowAnimIndex, 1);
    }

    IEnumerator PlayThrowAnim()
    {
        ChadHud.Instance.DeactivateAimHUD();
        RPCStartThrow();
        SendRPC("RPCStartThrow");
        Vector3 chosenDirection = Camera.transform.forward;
        chosenDirection.y *= 1.2f;
        chosenDirection *= ThrowForce;// new Vector3(Camera.transform.forward.x, Camera.transform.forward.y, Camera.transform.forward.z) * ThrowForce;
        Vector3 ballCamPos = Camera.transform.position;
        
        //yield return new WaitForSeconds(0.50f); // animation bound, langa lite _magic_ numbers

        ThrowObject(ballCamPos, chosenDirection);
        HasThrown = false;

        yield return new WaitForSeconds(1.0f);

        ResetThrow();
        if (State != STATE.RAGDOLL)
        {
            State = STATE.CHADING;
            Animations.SetAnimationWeight(ThrowAnimIndex, 0);
            SendRPC("RPCSetAnimWeight", (int)ThrowAnimIndex, 0);
        }

        Throwing = null;
    }

    #endregion

    #region Throwing stuff
    private void ChargeObject()
    {
        ChargeTime += Time.DeltaTime;
        ChargeTime = MathHelper.Clamp(ChargeTime, 0, PickedUpObject.chargeTimeMax);
        
        PickedUpObject.SetChargeTime(ChargeTime);
        float tets = PickedUpObject.GetChargeTime();
        

        ThrowForce = MathHelper.Lerp(BaseThrowForce, MaxThrowForce, ChargeTime / PickedUpObject.chargeTimeMax);
        ChadHud.Instance.ChargeChargeBar(ChargeTime / PickedUpObject.chargeTimeMax);
    }

    private void ThrowObject(Vector3 camPos, Vector3 direction)
    {
        PickedUpObject.SetChargeTime(0.0f);
        PickedUpObject.Throw(camPos, direction);
    }

    public void RPCPickup(int id)
    {
        if (State != STATE.RAGDOLL)
        {
            if (PickedUpObject)
            {
                if (PickedUpObject.ID == id)
                    return;
                else
                    PickedUpObject.Drop();
            }

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
    #endregion

    public override void OnRead(NetDataReader reader, bool initialState)
    {

        int pickedUpObject = reader.GetInt();
        if (pickedUpObject >= 0)
        {
            RPCPickup(pickedUpObject);
        }    
        else if(PickedUpObject)
        {
            PickedUpObject.Drop();
        }

        if (isOwner)
        {
            reader.GetInt();
            reader.GetVector3();
            reader.GetVector2();
            reader.GetBool();
            reader.GetBool();
            return;
        }
        State = (STATE)reader.GetInt();
        Direction = reader.GetVector3();
        CurrentVelocity = reader.GetVector2();
        HasThrown = reader.GetBool();
        CanBeTackled = reader.GetBool();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        writer.Put(PickedUpObject ? PickedUpObject.ID : -1);
        writer.Put((int)State);
        writer.Put(Direction);
        writer.Put(CurrentVelocity);
        writer.Put(HasThrown);
        writer.Put(CanBeTackled);
        return true;
    }

    public override void OnTriggerEnter(Collider collider)
    {
        if (isOwner && State != STATE.RAGDOLL && !Locked)
        {
            PickupableObject pickupablea = collider.transform.parent?.gameObject.GetComponent<PickupableObject>();
            if (pickupablea)
            {
                Debug.LogError("Why Denny!?");
            }

            PickupableObject pickupable = collider.transform.parent?.gameObject.GetComponent<PickupableObject>();
            if (pickupable && PickedUpObject == null)
            {
                if (pickupable.transform.parent == null)
                {
                    TakeOwnership(pickupable.gameObject);
                    SendRPC("RPCPickup", pickupable.ID);
                    RPCPickup(pickupable.ID);
                }
            }
        }
    }


    public override void OnCollisionEnter(Collider collider)
    {

        if (isOwner && State != STATE.RAGDOLL && !Locked)
        {
            ChadControls otherChad = collider.gameObject.GetComponent<ChadControls>();

            if (otherChad)
            {
                float TheirVelocity = otherChad.CurrentVelocity.Length();
                Debug.Log(TheirVelocity);
                Debug.Log(CurrentVelocity.Length());
                if (MatchSystem.instance.GetPlayerTeam(collider.gameObject) == MatchSystem.instance.GetPlayerTeam(this.gameObject))
                {
                    Debug.Log("Trying to tackle player on same team, you baka.");
                }
                else if (otherChad.CanBeTackled && (CurrentVelocity.Length() > TackleThreshold && CurrentVelocity.Length() >= TheirVelocity))
                {
                    //toggle ragdoll
                    Vector3 force = (transform.forward + Vector3.Up * 0.5f) * ImpactFactor * CurrentVelocity.Length();
                    otherChad.ActivateRagdoll(MinimumRagdollTimer, force);
                }

            }
        
        }
    }
}
