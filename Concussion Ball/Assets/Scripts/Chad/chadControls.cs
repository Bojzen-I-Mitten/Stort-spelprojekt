﻿#define PRINT_CONSOLE_DEBUG
//#define L_FOR_RAGDOLL

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
        DANCING,    // user dances to powerup music

        NUM_STATES
    };
    public STATE State { get; set; }
    public bool Locked = false;
    public bool CanBeTackled = true;

    // Counter tracking frames before enabling ragdoll
    private readonly static int FRAME_TICK_WAIT_RAGDOLL = 2;
    private static int frameRagdollDisableTick = FRAME_TICK_WAIT_RAGDOLL;

    #region GUI
    private Canvas Canvas;
    public Font PickupFont { get; set; }
    private Text PowerupPickupText;
    #endregion

    #region Throwing stuff
    [Category("Throwing")]
    public Transform hand { get; set; }
    [Category("Throwing")]
    public float ChargeTime { get; set; }

    

    private uint ChargeAnimIndex = 0;
    private uint ThrowAnimIndex = 1;
    private bool HasThrown = false;
    #endregion

    #region Movement stuff
    public Vector3 Direction; //Right, roll, forward

    public Vector2 CurrentVelocity = new Vector2(0, 0); //Right and forward
    public float Acceleration { get; set; } = 2.0f; //2 m/s^2
    public float BaseSpeed { get; set; }  = 5.0f;
    public float MaxSpeed { get; set; } = 10.0f;
    public float FirstJumpForce;
    public float SecondJumpForce;

    public Quaternion DivingRotation = Quaternion.Identity;
    public float MinimumRagdollTimer = 2.0f;

    public float ImpactFactor = 80.0f;//{ get; set; } = 100;
    public float TackleThreshold { get; set; } = 8.5f;
    private float DivingTimer = 0.0f;
    private float JumpingTimer = 0.0f;
    private bool Jumping = false;
    private bool Landed = false;

    private ParticleEmitter runningFastParticles;
    #endregion

    [Browsable(false)]
    public Rigidbody rBody { get; set; }
    private NetworkTransform ragdollSync;
    Chadimations Animations;
    public Ragdoll Ragdoll;

    [Browsable(false)]
    public NetworkPlayer NetPlayer { get; private set; }

    IEnumerator RagdollRecoverer = null;
    IEnumerator Ragdolling = null;
    IEnumerator Throwing = null;
    IEnumerator Diving = null;
    IEnumerator FadeText = null;

    public PickupableObject PickedUpObject;
    private float xStep { get { return Input.GetMouseX() * Time.ActualDeltaTime; } }

    // TOY SOLDIER
    // Coroutines
    IEnumerator ToySoldierModifier = null;

    public bool ToySoldierAffected = false;
    private float ScaleCountdown;
    private Vector3 OriginalScale;
    private float OriginalAcceleration;
    private float OriginalBaseSpeed;
    private float OriginalMaxSpeed;
    private Vector3 OrginalCapsuleCenter;
    private float OriginalCapsuleHeight;
    private float OriginalCapsuleRadius;
    private float OriginalMass;
    private float OriginalFirstJumpForce;
    private float OriginalSecondJumpForce;

    private Material[] OriginalMaterials;

    public Material ToySoldierMaterial { get; set; }


    // Tweaking constants
    private float ScaleDuration = 10.0f;

    public override void OnAwake()
    {
        ragdollSync = gameObject.AddComponent<NetworkTransform>();
        NetworkIdentity c = gameObject.GetComponent<NetworkIdentity>();
        gameObject.SetComponentIndex(c, 0xfffffff);  // Ensure network writer is last

        OriginalMaterials = gameObject.GetComponent<RenderSkinnedComponent>().materials;
    }

    public override void Start()
    {
        Canvas = ChadHud.Instance.Canvas;

        // Init pick-up text and description
        PowerupPickupText = Canvas.Add("");
        PowerupPickupText.position = new Vector2(0.4975f, 0.83f);
        PowerupPickupText.color = Color.White; // Need black outline for better visual effect
        PowerupPickupText.origin = new Vector2(0.5f, 0.0f);
        PowerupPickupText.outline = true;
        PowerupPickupText.font = PickupFont;

        State = STATE.CHADING;

        // Access rigidbody and apply
        rBody = gameObject.GetComponent<Rigidbody>();
        rBody.IsKinematic = false;

        if (rBody != null)
            rBody.IsKinematic = !isOwner;

        NetPlayer = gameObject.GetComponent<NetworkPlayer>();
        rBody.Friction = 0.99f;
        Animations = gameObject.GetComponent<Chadimations>();
        Ragdoll = gameObject.GetComponent<Ragdoll>();
        ragdollSync.target = Ragdoll.GetHips().transform;
        ragdollSync.SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;
        if (rBody != null)
            rBody.IsKinematic = !isOwner;
        Identity.RefreshCache();

        MinimumRagdollTimer = 2.0f;
        
        FirstJumpForce = 450.0f;
        SecondJumpForce = 350.0f;
        OriginalFirstJumpForce = FirstJumpForce;
        OriginalSecondJumpForce = SecondJumpForce;

        // Store original values
        OriginalScale = gameObject.transform.scale;
        OriginalAcceleration = Acceleration;
        OriginalBaseSpeed = BaseSpeed;
        OriginalMaxSpeed = MaxSpeed;

        CapsuleCollider capsule = gameObject.GetComponent<CapsuleCollider>();
        OrginalCapsuleCenter = capsule.center;
        OriginalCapsuleHeight = capsule.height;
        OriginalCapsuleRadius = capsule.radius;
        OriginalMass = rBody.Mass;

        //OriginalMaterials = gameObject.GetComponent<RenderSkinnedComponent>().materials;

        TackleThreshold = 8.5f;

        runningFastParticles = gameObject.AddComponent<ParticleEmitter>();
        Texture2D smokeTex = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/smoke_particle.png");
        if (smokeTex.height > 0)
            runningFastParticles.Texture = smokeTex;
        runningFastParticles.MinSize = 0.2f;
        runningFastParticles.MaxSize = 0.4f;
        runningFastParticles.EndSize = 0.0f;
        runningFastParticles.MinLifeTime = 0.2f;
        runningFastParticles.MaxLifeTime = 0.9f;
        runningFastParticles.EmissionRate = 40;
        runningFastParticles.MinRotationSpeed = -5.0f;
        runningFastParticles.MaxRotationSpeed = 5.0f;
        runningFastParticles.MinSpeed = 0.0f;
        runningFastParticles.MaxSpeed = 0.0f;
        runningFastParticles.EndSpeed = 0.0f;
        runningFastParticles.DistanceFromSphereCenter = 0;
        runningFastParticles.Gravity = 0.0f;
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
            ChadCam.instance.gameObject.SetActive(false);
            Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
        }

    }

    public void ActivateCamera()
    {
        if (isOwner)
        {
            Debug.Log("ActivateCamera chadControls");
            ChadCam.instance.gameObject.SetActive(true);
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
        }

    }
    #endregion

    public override void Update()
    {
        if (isOwner)
        {
            // Toy solider powerup
            if (ToySoldierAffected)
            {
                if(ToySoldierModifier == null)
                {
                    ToySoldierModifier = ToySoldierRoutine();
                    StartCoroutine(ToySoldierModifier);
                }
            }

            DivingTimer += Time.DeltaTime;
            JumpingTimer += Time.DeltaTime;

            Direction = new Vector3(0, 0, 0);
            if (State != STATE.RAGDOLL && State != STATE.DANCING)
            {
            if ((CameraMaster.instance.GetState() != CAM_STATE.EXIT_MENU) && (CameraMaster.instance.GetState() != CAM_STATE.OPTIONS_MENU))
            {
                HandleKeyboardInput();
                HandleMouseInput();
            }

                AirHandling();
            }
            //Accelerate fake gravity as it felt too low, playtest
            if (!OnGround() && rBody.LinearVelocity.y < 0 && rBody.LinearVelocity.y > -5.9f && JumpingTimer > 1)
                rBody.LinearVelocity = rBody.LinearVelocity = Vector3.Transform(new Vector3(rBody.LinearVelocity.x, rBody.LinearVelocity.y - 2, rBody.LinearVelocity.z), rBody.Rotation);
        }
        StateMachine();
        
        if (rBody != null)
            rBody.IsKinematic = !isOwner;

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
            Ragdoll.ImpactParams param = new Ragdoll.ImpactParams(gameObject.transform.position, (/*-transform.forward +*/ transform.up * 0.5f) * 200000, 1);
            ActivateRagdoll(MinimumRagdollTimer, param);
        }
        if (Input.GetKeyDown(Input.Keys.J))
            Debug.Log(rBody.Position);
        if (Input.GetKeyDown(Input.Keys.K))
            NetPlayer.Reset();
#endif

        if (Input.GetKeyDown(Input.Keys.F12))
        {
            if (PickedUpObject)
                Debug.Log("Your current held item: " + PickedUpObject);
            else
                Debug.Log("No held item.");
        }

        rBody.Friction = 0.5f;
        if (!OnGround())
            rBody.Friction = 0.0f;


        if (State != STATE.DIVING)
            Animations.ResetTimer(STATE.DIVING, 0);
    }

    #region Ragdoll handling
    private void EnableRagdoll()
    {
        //Debug.Log(gameObject.Name + " ragdoll ON");
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
        //Debug.Log(gameObject.Name + " ragdoll OFF");
        gameObject.transform.position = Ragdoll.GetHips().transform.position;
        //gameObject.transform.eulerAngles = new Vector3(0, Ragdoll.GetHips().transform.localEulerAngles.y, 0);
        Ragdoll.DisableRagdoll();
        rBody.enabled = true;

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




    #region Input handling
    private void HandleKeyboardInput()
    {
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
            JumpFondling();
        }
    }

    private void HandleMouseInput()
    {
        if (Input.GetMouseMode() == Input.MouseMode.POSITION_RELATIVE)
        {
            //Throw stuff
            if (HasThrowableObject())
            {
                if (State != STATE.DIVING)
                {
                    if ((Input.GetMouseButtonDown(Input.MouseButtons.RIGHT) && !Input.GetMouseButton(Input.MouseButtons.LEFT)) || (Input.GetMouseButtonDown(Input.MouseButtons.LEFT) && !Input.GetMouseButton(Input.MouseButtons.RIGHT)) && !HasThrown)
                    {
                        State = STATE.THROWING;
                        ChadHud.Instance.ActivateAimHUD();

                        Animations.SetAnimationWeight(ChargeAnimIndex, 1);
                    }
                    else if((Input.GetMouseButtonUp(Input.MouseButtons.LEFT) || Input.GetMouseButtonUp(Input.MouseButtons.RIGHT)) && !HasThrown && State == STATE.THROWING)
                    {
                        HasThrown = true;

                        Throwing = PlayThrowAnim();
                        StartCoroutine(Throwing);
                    }
                    else if ((Input.GetMouseButton(Input.MouseButtons.RIGHT) || Input.GetMouseButton(Input.MouseButtons.LEFT)) && !HasThrown && State == STATE.THROWING)
                    {
                        ChargeObject();
                        
                    }
                    //else if (Input.GetMouseButtonUp(Input.MouseButtons.RIGHT) && State == STATE.THROWING && Throwing == null)
                    //{
                    //    State = STATE.CHADING;
                    //    ResetThrow();
                    //}
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
        Animations.SetAnimationWeight(ChargeAnimIndex, 0);
        Animations.SetAnimationWeight(ThrowAnimIndex, 0);
        
        if (PickedUpObject)
        {
            PickedUpObject.StopEmitting();
            PickedUpObject.Cleanup();
        }

    }

    private void ResetThrow()
    {
        if (isOwner)
        {
            ChadHud.Instance.DeactivateAimHUD();
            ChargeTime = 0;
        }
        SendRPC("RPCResetThrow");
        RPCResetThrow();
    }
    #endregion

    private void AirHandling()
    {
        //No random in-air y-directional velocity
        if (!OnGround() && rBody.LinearVelocity.y > 0 && Landed)
            rBody.LinearVelocity = new Vector3(rBody.LinearVelocity.x, 0.0f, rBody.LinearVelocity.z);
    }

    private void JumpFondling()
    {
        float modifiedBaseSpeed = PickedUpObject ? PickedUpObject.MovementSpeedModifier * BaseSpeed : BaseSpeed;
        float modifiedMaxSpeed = PickedUpObject ? PickedUpObject.MovementSpeedModifier * MaxSpeed : MaxSpeed;

        if (OnGround())
        {
            JumpingTimer = 0.0f;
            Jumping = true;
            Landed = false;
            DivingRotation = this.gameObject.transform.rotation;

            float velocityBetweenBaseAndMax = modifiedMaxSpeed - ((modifiedMaxSpeed - modifiedBaseSpeed) / 2);
            if (CurrentVelocity.y > velocityBetweenBaseAndMax)
                CurrentVelocity.y = velocityBetweenBaseAndMax;
            rBody.LinearVelocity = Vector3.Transform(new Vector3(rBody.LinearVelocity.x, 0, rBody.LinearVelocity.z), rBody.Rotation);
            rBody.AddForce(new Vector3(0, FirstJumpForce, 0), Rigidbody.ForceMode.Impulse);
        }
        else if ((Jumping || (!OnGround() && JumpingTimer > 3.0f)) && !PickedUpObject)
        {
            JumpingTimer = 0.0f;
            Jumping = false;
            Landed = false;
            DivingRotation = this.gameObject.transform.rotation;
            CurrentVelocity.y = BaseSpeed;
            rBody.LinearVelocity = Vector3.Transform(new Vector3(rBody.LinearVelocity.x, 0, rBody.LinearVelocity.z), rBody.Rotation);
            rBody.AddForce(new Vector3(0, SecondJumpForce, 0), Rigidbody.ForceMode.Impulse);
        }
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
                if (Direction.z != 0 && Direction.x != 0)
                {
                    diagonalModifier = 0.5f;
                }
                CurrentVelocity.y = Slope(Direction.z, 1) * modifiedBaseSpeed * diagonalModifier;
                CurrentVelocity.x = Slope(Direction.x, 1) * modifiedBaseSpeed * diagonalModifier;
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
            case STATE.DANCING:
                break;
        }
        //if (Input.GetKeyDown(Input.Keys.M))
        //    Debug.Log(CurrentVelocity.Length());

        if (State != STATE.DIVING && Landed)
            rBody.LinearVelocity = Vector3.Transform(new Vector3(-CurrentVelocity.x, rBody.LinearVelocity.y, -CurrentVelocity.y), rBody.Rotation);
        else
            rBody.LinearVelocity = Vector3.Transform(new Vector3(-CurrentVelocity.x, rBody.LinearVelocity.y, -CurrentVelocity.y), DivingRotation);

        //Temporary animation fix, without this no animation will play when moving diagonally
        if(Direction.z != 0)
            Direction.x = 0;

        if (CurrentVelocity.y > TackleThreshold)
            runningFastParticles.Emit = true;
        else
            runningFastParticles.Emit = false;
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
        DisableRagdoll();
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
        if (ToySoldierAffected)
        {
            ToySoldierAffected = false;
            RPCResetMaterial();
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
        AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.TACKLED);
        State = STATE.RAGDOLL;
        EnableRagdoll();
        Ragdoll.AddForce(param);
        if (isOwner)
        {
            yield return new WaitForSeconds(duration);
            Debug.Log("Deactivating them ragdoll bois was good");
            while (Ragdoll.DistanceToWorld() >= 0.75f) // can trigger mid air atm, check if ray hits ground and not chad
            {
                yield return null;
            }
            //Debug.Log("The ground has been reached.");
            yield return new WaitForSeconds(1);
            State = STATE.CHADING;
            CurrentVelocity.y = BaseSpeed;
        }
        else
            yield return null;
    }

    IEnumerator RagdollRecovery()
    {
        yield return new WaitForSeconds(2);
        CanBeTackled = true;

        // Recovery particles
    }

    IEnumerator FadePickupText()
    {
        // Wait a second before start fading
        yield return new WaitForSeconds(1.0f);

        Color pickupColor = PowerupPickupText.color;
        Color outlineColor = PowerupPickupText.outlineColor;

        while (pickupColor.a > 0)
        {
            pickupColor.a -= 5;
            outlineColor.a -= 5;

            if (pickupColor.a > 0)
            {
                PowerupPickupText.color = pickupColor;
                PowerupPickupText.outlineColor = outlineColor;
            }

            yield return new WaitForSeconds(0.01f);
        }

        PowerupPickupText.rendering = false;
    }

    IEnumerator ToySoldierRoutine()
    {
        yield return new WaitForSeconds(ScaleDuration);

        // Set back original values and properties when timer has expired
        gameObject.transform.localScale = OriginalScale;
        Acceleration = OriginalAcceleration;
        BaseSpeed = OriginalBaseSpeed;
        MaxSpeed = OriginalMaxSpeed;
        FirstJumpForce = OriginalFirstJumpForce;
        SecondJumpForce = OriginalSecondJumpForce;

        CapsuleCollider capsule = gameObject.GetComponent<CapsuleCollider>();
        capsule.center = OrginalCapsuleCenter;
        capsule.height = OriginalCapsuleHeight;
        capsule.radius = OriginalCapsuleRadius;
        rBody.Mass = OriginalMass;
        rBody.AddForce(new Vector3(0, 200.0f, 0), Rigidbody.ForceMode.Impulse);

        // Network

        if(MatchSystem.instance.LocalChad.ToySoldierAffected)
        {
            SendRPC("RPCResetMaterial");
            RPCResetMaterial();
        }        
    }

    public void RPCResetMaterial()
    {
        ToySoldierAffected = false;
        ToySoldierModifier = null;
        ScaleCountdown = ScaleDuration;
        RenderSkinnedComponent render = gameObject.GetComponent<RenderSkinnedComponent>();
        render.materials = OriginalMaterials;
        gameObject.GetComponent<ShirtRenderer>().Reset();
        Debug.Log("reset");
    }

    public void SetTiny()
    {
        SendRPC("RPCSetTiny");
        RPCSetTiny();
    }

    public void RPCSetTiny()
    {
        if (PickedUpObject)
            PickedUpObject.Drop();

        RenderSkinnedComponent render = gameObject.GetComponent<RenderSkinnedComponent>();
        if (ToySoldierMaterial != null)
        {
            render.SetMaterial(0, ToySoldierMaterial);
            render.SetMaterial(1, ToySoldierMaterial);
            render.SetMaterial(2, ToySoldierMaterial);
            Debug.Log("tinyMat");
        }
        rBody.enabled = false;
        Debug.Log("tiny");
        rBody.Mass *= 0.5f;
        transform.scale *= 0.5f;

        CapsuleCollider capsule = gameObject.GetComponent<CapsuleCollider>();
        capsule.center = new Vector3(0, 0.32f, 0);
        capsule.height *= 0.5f;
        capsule.radius *= 0.5f;
        rBody.enabled = true;
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
        if (isOwner)
            ChadHud.Instance.DeactivateAimHUD();
        RPCStartThrow();
        SendRPC("RPCStartThrow");
        Vector3 chosenDirection = ChadCam.instance.transform.forward;
        Vector3 ballCamPos = ChadCam.instance.transform.position;
        
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
            if (PickedUpObject)
            {
                if (PickedUpObject.ID == id)
                    return;
                else
                    PickedUpObject.RPCDrop();
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

    #region PickupPowerup
    private void DisplayPowerupText(ref Text powerupText, String description)
    {
        powerupText.text = description;
    }

    private void ResetAlpha(ref Text powerupText)
    {
        Color pickupColor = powerupText.color;
        Color outlineColor = powerupText.outlineColor;
        pickupColor.a = 255;
        outlineColor.a = 255;
        powerupText.color = pickupColor;
        powerupText.outlineColor = outlineColor;
        powerupText.rendering = true;
    }
    #endregion

    public override void OnRead(NetDataReader reader, bool initialState)
    {
        if (isOwner)
        {
            reader.GetInt();
            reader.GetInt();
            reader.GetVector3();
            reader.GetVector2();
            reader.GetBool();
            reader.GetBool();
            reader.GetBool();
            return;
        }

        int pickedUpObject = reader.GetInt();
        if (pickedUpObject >= 0)
        {
            RPCPickup(pickedUpObject);
        }
        else if (PickedUpObject)
        {
            PickedUpObject.RPCDrop();
            PickedUpObject = null;
        }


        State = (STATE)reader.GetInt();
        Direction = reader.GetVector3();
        CurrentVelocity = reader.GetVector2();
        HasThrown = reader.GetBool();
        CanBeTackled = reader.GetBool();
        if(!Animations)
            Animations = gameObject.GetComponent<Chadimations>();
        Animations.Throwing = reader.GetBool();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        writer.Put(PickedUpObject ? PickedUpObject.ID : -1);
        writer.Put((int)State);
        writer.Put(Direction);
        writer.Put(CurrentVelocity);
        writer.Put(HasThrown);
        writer.Put(CanBeTackled);
        if (!Animations)
            Animations = gameObject.GetComponent<Chadimations>();
        writer.Put(Animations.Throwing);
        return true;
    }

    private void Pickup(PickupableObject pickupable)
    {
        if(!ToySoldierAffected)
        {
            if (pickupable.transform.parent == null)
            {
                ChargeTime = 0;
                if (FadeText != null)
                {
                    StopCoroutine(FadeText);
                    FadeText = null;
                } 

                ResetAlpha(ref PowerupPickupText);

            if (pickupable.gameObject.Name == "ball")
            {
                DisplayPowerupText(ref PowerupPickupText, "Picked up Ball");
                AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.PICKUPBALL);
            }
            else if (pickupable.gameObject.Name == "Vindaloo")
            {
                DisplayPowerupText(ref PowerupPickupText, "Picked up Vindaloo");
            }
            else if (pickupable.gameObject.Name == "ThomasTrain")
            {
                DisplayPowerupText(ref PowerupPickupText, "Picked up Thomas Train");
            }
            else if (pickupable.gameObject.Name == "Banana")
            {
                DisplayPowerupText(ref PowerupPickupText, "Picked up Banana");
            }
            else if (pickupable.gameObject.Name == "Gramophone")
            {
                DisplayPowerupText(ref PowerupPickupText, "Picked up Grandmaphone");
            }
            else if (pickupable.gameObject.Name == "ToySoldier")
            {
                DisplayPowerupText(ref PowerupPickupText, "Picked up Toy Soldier");
            }

                FadeText = FadePickupText();
                StartCoroutine(FadeText);

                TakeOwnership(pickupable.gameObject);
                RPCPickup(pickupable.ID);
                //SendRPC("RPCPickup", pickupable.ID);
            }
        }
    }

    public override void OnTriggerEnter(Collider collider)
    {
        if (isOwner && State != STATE.RAGDOLL && !Locked)
        {
            PickupableObject pickupable = collider.transform.parent?.gameObject.GetComponent<PickupableObject>();
            if (pickupable && pickupable.gameObject.GetActive() && PickedUpObject == null)
            {
                Pickup(pickupable);
            }
        }
    }

    public override void OnCollisionStay(Collider collider)
    {
        if (MatchSystem.instance && isOwner && State != STATE.RAGDOLL && !Locked)
        {
            ChadControls otherChad = collider.gameObject.GetComponent<ChadControls>();

            if (otherChad)
            {
                float modifiedBaseSpeed = PickedUpObject ? PickedUpObject.MovementSpeedModifier * BaseSpeed : BaseSpeed;
                float TheirVelocity = otherChad.CurrentVelocity.y;//Length();
                if (MatchSystem.instance.GetPlayerTeam(collider.gameObject) == MatchSystem.instance.GetPlayerTeam(this.gameObject))
                {
                    //Debug.Log("Trying to tackle player on same team, you baka.");
                }
                else if (otherChad.CanBeTackled && ((CurrentVelocity.y/*Length()*/ > TackleThreshold || (PickedUpObject && CurrentVelocity.y > BaseSpeed)) && CurrentVelocity.y/*Length()*/ >= TheirVelocity))
                {
                    Debug.Log("They tackled with: " + TheirVelocity + "(Forward: " + otherChad.CurrentVelocity.y + ", Strafe: " + otherChad.CurrentVelocity.x + ")");
                    Debug.Log("You tackled with: " + CurrentVelocity.y/*Length()*/ + "(Forward: " + CurrentVelocity.y + ", Strafe: " + CurrentVelocity.x + ")");

                    // Activate ragdoll
                    Vector3 force = (transform.forward + Vector3.Up * 0.5f) * ImpactFactor * CurrentVelocity.Length();
                    Ragdoll.ImpactParams param = new Ragdoll.ImpactParams(otherChad.gameObject.transform.position, force, 0.5f);
                    param.bodyPartFactor[(int)Ragdoll.BODYPART.RIGHT_LOWER_LEG] = 1.3f;
                    param.bodyPartFactor[(int)Ragdoll.BODYPART.LEFT_LOWER_LEG] = 1.3f;
                    otherChad.ActivateRagdoll(MinimumRagdollTimer, param);
                    AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.TACKLED);

                    NetPlayer.HasTackled += 1;
                    CurrentVelocity.y = modifiedBaseSpeed;
                }

            } 
        }
    }
}
