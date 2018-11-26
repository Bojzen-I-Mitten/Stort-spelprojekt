﻿#define PRINT_CONSOLE_DEBUG
#define L_FOR_RAGDOLL

using System.Linq;
using ThomasEngine;
using System;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;
using System.ComponentModel;
using System.Collections;
using ThomasEngine.Script;


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

    // Counter tracking frames before enabling ragdoll
    private readonly static int FRAME_TICK_WAIT_RAGDOLL = 2;
    private static int frameRagdollDisableTick = FRAME_TICK_WAIT_RAGDOLL;

    #region GUI
    //private Canvas Canvas;
    //public Font PickupFont { get; set; }
    //public Font PickupDescFont { get; set; }
    //private Text PowerupPickupText;
    //private Text PowerupPickupDescText;
    #endregion

    #region Throwing stuff
    [Category("Throwing")]
    public Transform hand { get; set; }
    [Category("Throwing")]
    public float ChargeTime { get; private set; }

    

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

    public Quaternion DivingRotation = Quaternion.Identity;
    private float MinimumRagdollTimer = 2.0f;

    public float ImpactFactor = 25.0f;//{ get; set; } = 100;
    public float TackleThreshold { get; set; } = 7;
    private float DivingTimer = 0.0f;
    private float JumpingTimer = 0.0f;
    private bool Jumping = false;
    private bool Landed = false;
    #endregion

    [Browsable(false)]
    public Rigidbody rBody { get; private set; }
    private NetworkTransform ragdollSync;
    Chadimations Animations;
    public Ragdoll Ragdoll;
    ChadCam _camera;
    ChadCam Camera
    {
        get
        {
            if (!_camera)
                _camera = ScriptUtility.FindComponent<ChadCam>();
            return _camera;
        }
    }

    [Browsable(false)]
    public NetworkPlayer NetPlayer { get; private set; }

    IEnumerator RagdollRecoverer = null;
    IEnumerator Ragdolling = null;
    IEnumerator Throwing = null;
    IEnumerator Diving = null;
    IEnumerator FadeText = null;

    public PickupableObject PickedUpObject;
    private float xStep { get { return Input.GetMouseX() * Time.ActualDeltaTime; } }

    public override void OnAwake()
    {
        ragdollSync = gameObject.AddComponent<NetworkTransform>();
        NetworkIdentity c = gameObject.GetComponent<NetworkIdentity>();
        gameObject.SetComponentIndex(c, 0xfffffff);  // Ensure network writer is last
    }

    public override void Start()
    {
        //Canvas = ChadHud.Instance.Canvas;

        // Init pick-up text and description
        //PowerupPickupText = Canvas.Add("");
        //PowerupPickupText.position = new Vector2(0.4975f, 0.5f);
        //PowerupPickupText.color = Color.Yellow; // Need black outline for better visual effect
        //PowerupPickupText.origin = new Vector2(0.5f, 0.0f);
        //PowerupPickupText.font = PickupFont;

        //PowerupPickupDescText = Canvas.Add("");
        //PowerupPickupDescText.position = new Vector2(0.4975f, 0.56f);
        //PowerupPickupDescText.color = Color.Black;
        //PowerupPickupDescText.origin = new Vector2(0.5f, 0.0f);
        //PowerupPickupDescText.font = PickupDescFont;

        State = STATE.CHADING;

        // Access rigidbody and apply
        rBody = gameObject.GetComponent<Rigidbody>();
        rBody.IsKinematic = false;

        NetPlayer = gameObject.GetComponent<NetworkPlayer>();
        rBody.Friction = 0.99f;
        Animations = gameObject.GetComponent<Chadimations>();
        Ragdoll = gameObject.GetComponent<Ragdoll>();
        ragdollSync.target = Ragdoll.GetHips().transform;
        ragdollSync.SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;

        Identity.RefreshCache();
    }
    public override void OnGotOwnership()
    {
        // Called when NetworkScene::SpawnPlayer is called
        base.OnGotOwnership();
        gameObject.SetActive(true);
        MatchSystem.instance.LocalChad = this;
    }
    public override void OnLostOwnership()
    {
        base.OnLostOwnership();
        if (this == MatchSystem.instance.LocalChad) // Currently chad is lost control of
            MatchSystem.instance.LocalChad = null;
    }


    #region camera state
    public void DeactivateCamera()
    {
        if (isOwner)
        {
            Camera.gameObject.SetActive(false);
            Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
        }

    }

    public void ActivateCamera()
    {
        if (isOwner)
        {
            Camera.gameObject.SetActive(true);
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
        }

    }
    #endregion

    public override void Update()
    {
        // Stop the fading routine 
        // Note: Doesn't properly work, the couroutine doesn't stop when the alpha has reached zero
        //if (FadeText != null)
        //{
        //    if(PowerupPickupText.color.a == 0 && PowerupPickupDescText.color.a == 0)
        //    {
        //        Debug.Log("Should stop");
        //        StopCoroutine(FadeText);
        //        FadeText = null;
        //    }
        //}
        if (CameraMaster.instance.State != CAM_STATE.EXIT_MENU)
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
                    rBody.LinearVelocity = rBody.LinearVelocity = Vector3.Transform(new Vector3(rBody.LinearVelocity.x, rBody.LinearVelocity.y - 2, rBody.LinearVelocity.z), rBody.Rotation);
            }
            StateMachine();
        }

        /* Enter leave ragdoll state
         */
        if (State == STATE.RAGDOLL && !Ragdoll.RagdollEnabled)
            EnableRagdoll();
        else if (State != STATE.RAGDOLL && Ragdoll.RagdollEnabled)
        {
            // Wait N frames before disabling ragdoll
            if (isOwner || frameRagdollDisableTick-- == 0)
            {
                DisableRagdoll();
                frameRagdollDisableTick = FRAME_TICK_WAIT_RAGDOLL;
            }
        }
#if (L_FOR_RAGDOLL)
        if (Input.GetKeyDown(Input.Keys.L))
        {
            Ragdoll.ImpactParams param = new Ragdoll.ImpactParams(gameObject.transform.position, (-transform.forward + transform.up * 0.5f) * 2000, 1);
            ActivateRagdoll(MinimumRagdollTimer, param);
        }
#endif
            if (Input.GetKeyDown(Input.Keys.K))
                NetPlayer.Reset();

        rBody.Friction = 0.5f;
        if (!OnGround())
            rBody.Friction = 0.0f;


        if (State != STATE.DIVING)
            Animations.ResetTimer(STATE.DIVING, 0);
    }

    #region Ragdoll handling
    private void EnableRagdoll()
    {
        Debug.Log(gameObject.Name + " ragdoll ON");
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
        CanBeTackled = false;
        Ragdoll.EnableRagdoll();

    }

    private void DisableRagdoll()
    {
        Debug.Log(gameObject.Name + " ragdoll OFF");
        gameObject.transform.position = Ragdoll.GetHips().transform.position;
        //gameObject.transform.eulerAngles = new Vector3(0, Ragdoll.GetHips().transform.localEulerAngles.y, 0);
        Ragdoll.DisableRagdoll();
        gameObject.GetComponent<Rigidbody>().enabled = true;

        // call coroutine function that sets canragdoll true
        RagdollRecoverer = RagdollRecovery();
        StartCoroutine(RagdollRecoverer);
    }

    private void LocalActivateRagdoll(float duration, Ragdoll.ImpactParams param)
    {
        SendRPC("RPCStartRagdoll", duration, param);
        RPCStartRagdoll(duration, param);
    }

    public void RPCLocalActivateRagdoll(float duration, Ragdoll.ImpactParams param)
    {
        if(MatchSystem.instance.LocalChad)
        {
            MatchSystem.instance.LocalChad.LocalActivateRagdoll(duration, param);
        }
    }
    /* Call for activating ragdoll
    */
    public void ActivateRagdoll(float duration, Ragdoll.ImpactParams param)
    {
        if(!isOwner)
        {
            NetPeer peerThatOwnsThisPlayer = MatchSystem.instance.Scene.Players.FirstOrDefault(player => player.Value == Identity).Key;
            if(peerThatOwnsThisPlayer != null)
            {
                MatchSystem.instance.SendRPC(peerThatOwnsThisPlayer, -1, "RPCLocalActivateRagdoll", duration, param);
            }
            RPCStartRagdoll(duration, param);
        }
        else
        {
            LocalActivateRagdoll(duration, param);
        }
        Ragdoll.Smack();
    }

    public void RPCStartRagdoll(float duration, Ragdoll.ImpactParams param)
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
            Ragdolling = StartRagdoll(duration, param);
            StartCoroutine(Ragdolling);
            //if(isOwner)
            //    Camera.InitFreeLookCamera();
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
        //if (Input.GetKeyUp(Input.Keys.Escape))
        //{
        //    Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
        //}

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
                {
                    if (Direction.x != 0)
                        diagonalModifier = 0.5f;
                    CurrentVelocity.y = -modifiedBaseSpeed * 0.75f * diagonalModifier;
                }
                    
                if(OnGround() && Direction.z >= 0) // as to not allow acceleration mid air
                    CurrentVelocity.y += Direction.z * Acceleration * diagonalModifier * Time.DeltaTime;

                if (Direction.z == 0)
                    CurrentVelocity.y = 0;

                CurrentVelocity.x = Direction.x * modifiedBaseSpeed * 0.75f * diagonalModifier;
                CurrentVelocity.y = MathHelper.Clamp(CurrentVelocity.y, -modifiedBaseSpeed, modifiedMaxSpeed);
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
        //Animations.RPCResetTimer(0);
        float timer = 1.5f;
        while (timer > 0.0f)
        {
            timer -= Time.DeltaTime;
            yield return null;
        }
        CurrentVelocity.y = BaseSpeed;
        State = STATE.CHADING;
    }

    IEnumerator StartRagdoll(float duration, Ragdoll.ImpactParams param)
    {
        if (isOwner)
        {
            State = STATE.RAGDOLL;
            Ragdoll.AddForce(param);

            yield return new WaitForSeconds(duration);
            float timer = 0;
            while (Ragdoll.DistanceToWorld() >= 0.5f && timer < 5)
            {
                timer += Time.DeltaTime;
                yield return null;
            }
            yield return new WaitForSeconds(1);
            State = STATE.CHADING;
            CurrentVelocity.y = BaseSpeed;
        }
    }

    IEnumerator RagdollRecovery()
    {
        yield return new WaitForSeconds(2);
        CanBeTackled = true;

        // Recovery particles
    }

    //IEnumerator FadePickupText()
    //{
    //    Color pickupColor = PowerupPickupText.color;
    //    Color descriptionColor = PowerupPickupDescText.color;

    //    while (pickupColor.a > 0 && descriptionColor.a > 0)
    //    {
    //        pickupColor.a -= 25;
    //        descriptionColor.a -= 25;

    //        if(pickupColor.a > 0 || descriptionColor.a > 0)
    //        {
    //            PowerupPickupText.color = pickupColor;
    //            PowerupPickupDescText.color = descriptionColor;
    //        }
    //        else
    //        {
    //            pickupColor.a = 0;
    //            descriptionColor.a = 0;
    //            PowerupPickupText.color = pickupColor;
    //            PowerupPickupDescText.color = descriptionColor;
    //        }
            
    //        yield return new WaitForSeconds(0.2f);
    //    }
    //}

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
        

        PickedUpObject.ThrowForce = MathHelper.Lerp(PickedUpObject.BaseThrowForce, PickedUpObject.MaxThrowForce, ChargeTime / PickedUpObject.chargeTimeMax);
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

    #region PickupPowerup
    private void DisplayPowerupText(ref Text powerupText, ref Text powerupDesc, String powerup, String description)
    {
        powerupText.text = powerup;
        powerupDesc.text = description;
    }
    #endregion

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
        if (initialState)
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
#if (PRINT_CONSOLE_DEBUG)
            if (pickupablea)
                Debug.Log("Entered Pickup: " + pickupablea.Name);
#endif

            PickupableObject pickupable = collider.transform.parent?.gameObject.GetComponent<PickupableObject>();
            if (pickupable && PickedUpObject == null)
            {
                if (pickupable.transform.parent == null)
                {
                    // Change pick-up text ON screen
                    //if (pickupablea.gameObject.Name == "Vindaloo")
                    //{
                    //    DisplayPowerupText(ref PowerupPickupText, ref PowerupPickupDescText, "Vindaloo", "Throw to Explode");
                    //    FadeText = FadePickupText();
                    //    StartCoroutine(FadeText); 
                    //}
                    //else if (pickupablea.gameObject.Name == "ThomasTrain")
                    //{
                    //    DisplayPowerupText(ref PowerupPickupText, ref PowerupPickupDescText, "Thomas Train", "Release the Train");
                    //}

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
                    // Activate ragdoll
                    Vector3 force = (transform.forward + Vector3.Up * 0.5f) * ImpactFactor * CurrentVelocity.Length();
                    Ragdoll.ImpactParams param = new Ragdoll.ImpactParams(otherChad.gameObject.transform.position, force, 0.5f);
                    param.bodyPartFactor[(int)Ragdoll.BODYPART.RIGHT_LOWER_LEG] = 1.3f;
                    param.bodyPartFactor[(int)Ragdoll.BODYPART.LEFT_LOWER_LEG] = 1.3f;
                    otherChad.ActivateRagdoll(MinimumRagdollTimer, param);
                }

            } 
        }
    }
}
