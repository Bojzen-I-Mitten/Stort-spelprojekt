using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using System;
using ThomasEngine.Network;
using LiteNetLib.Utils;
using LiteNetLib;

public class ChadControls : NetworkComponent
{
    public float baseThrowForce { get; set; } = 5.0f;
    public float maxThrowForce { get; set; } = 20.0f;

    public float Force { get; set; } = 5;
    public float CameraSensitivity_x { get; set; } = 10;
    public float CameraSensitivity_y { get; set; } = 20;
    public float CameraHeight { get; set; } = 1;
    public float CameraMaxVertDegrees { get; set; } = 60;
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }
    public float CameraDistance { get; set; } = 2;
    public float CameraHeightThrowing { get; set; } = 0.8f;
    public float CameraDistanceThrowing { get; set; } = 0.5f;
    private float TotalYStep = 0;
    private float TotalXStep = 0;
    private Vector3 CameraStartPos;

    private float throwForce;

    public Transform hand { get; set; }

    public Camera camera { get; set; }
    private Quaternion FreeLookDirection;
    Rigidbody rBody;

    //Camera test;
    private Ball _Ball;
    private Ball Ball {
        get{
            if(!_Ball)
                _Ball = GetObjectsOfType<Ball>().FirstOrDefault();
            return _Ball;
        }
    }
    // private bool canPickupBall = true;

        bool jumpDelay = true;
        bool tackling = false;
        bool jumping = false;

    public override void Start()
    {
        rBody = gameObject.GetComponent<Rigidbody>();
        if (!isOwner && camera)
        {
            camera.enabled = false;
        }


        if (!camera)
            Debug.LogWarning("Camera not set for player");

        throwForce = baseThrowForce;
    }

    public override void Update()
    {
            
    }

    public void HandleMovement(float velocityForward, float velocityStrafe)
    {
        transform.position += (transform.forward * velocityForward + transform.right * velocityStrafe) * Time.DeltaTime;
    }

    #region Camera Controls
    public void FondleCamera(float velocity, float xStep, float yStep)
    {
        if (camera)
        {
            float yaw = ThomasEngine.MathHelper.ToRadians(-xStep * CameraSensitivity_x);
            if (velocity != 0)
                yaw = ClampCameraRadians(yaw, -1 / velocity, 1 / velocity);
            transform.RotateByAxis(transform.up, yaw);

            TotalXStep -= yaw; //for of freelook
            TotalYStep -= ThomasEngine.MathHelper.ToRadians(yStep * CameraSensitivity_y);
            TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);
            camera.transform.localRotation = Quaternion.CreateFromAxisAngle(Vector3.Right, TotalYStep);
            camera.transform.localPosition = Vector3.Transform(new Vector3(0, CameraHeight, CameraDistance), camera.transform.localRotation);

        }
    }

    public void FreeLookCamera(float velocity, float xStep, float yStep)
    {
        if (camera)
        {
            TotalXStep -= ThomasEngine.MathHelper.ToRadians(xStep * CameraSensitivity_x);
            TotalYStep -= ThomasEngine.MathHelper.ToRadians(yStep * CameraSensitivity_y);
            TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);

            Quaternion rot = Quaternion.CreateFromYawPitchRoll(TotalXStep, TotalYStep, 0);
            camera.transform.localRotation = rot;

            camera.transform.localPosition = Vector3.Transform(new Vector3(0, CameraHeight, CameraDistance), camera.transform.localRotation);
        }
    }

    public void ThrowingCamera(float velocity, float xStep, float yStep)
    {
        if (camera)
        {
            float yaw = ThomasEngine.MathHelper.ToRadians(-xStep * CameraSensitivity_x);
            if (velocity != 0)
                yaw = ClampCameraRadians(yaw, -1 / velocity, 1 / velocity);
            transform.RotateByAxis(transform.up, yaw);

            TotalXStep -= yaw; //for of freelook
            TotalYStep -= ThomasEngine.MathHelper.ToRadians(yStep * CameraSensitivity_y);
            TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);
            camera.transform.localRotation = Quaternion.CreateFromAxisAngle(Vector3.Right, TotalYStep);
            camera.transform.localPosition = new Vector3(CameraDistanceThrowing, CameraHeightThrowing, CameraDistanceThrowing);
        }
    }

    public void InitFreeLookCamera()
    {
        camera.transform.localPosition = new Vector3(0, CameraHeight, -CameraDistance);
        camera.transform.LookAt(transform.position + new Vector3(0, CameraHeight, 0));

        camera.transform.localEulerAngles = new Vector3(0, 180, 0);
        TotalXStep = ThomasEngine.MathHelper.Pi;
        TotalYStep = 0;
    }

    public void ResetCamera()
    {
        if (camera)
        {
            camera.transform.localPosition = new Vector3(0, CameraHeight, CameraDistance);
            camera.transform.LookAt(transform.position + new Vector3(0, CameraHeight, 0));

            camera.transform.localEulerAngles = new Vector3(0, 0, 0);
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


    public void ChargeBall()
    {
        Ball.ChargeColor();
    }

    public void ThrowBall(float chargeForce)
    {
        if(HasBall())
            Ball.Throw(camera.transform.forward * chargeForce);
    }

    public void RPCPickupBall()
    {
        if(Ball)
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
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        if (initialState)
        {
            writer.Put(HasBall());
        }
        return true;
    }
    
    public override void OnCollisionEnter(Collider collider)
    {
        if (isOwner && Ball)
        {
            if (collider.gameObject == Ball.gameObject)
            {
                if(Ball.transform.parent == null)
                {
                    TakeOwnership(Ball.gameObject);
                    SendRPC("RPCPickupBall");
                    RPCPickupBall();
                }

            }
        }
    }
}
