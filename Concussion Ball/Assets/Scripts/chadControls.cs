/* TODO: 
 */

using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using System;
using ThomasEngine.Network;


public class ChadControls : ScriptComponent
{
    #region Throwing stuff
    public float BaseThrowForce { get; set; } = 5.0f;
    public float MaxThrowForce { get; set; } = 20.0f;
    public float ChargeRate { get; set; } = 5.0f;
    private float ThrowForce;
    public Transform hand { get; set; }
    #endregion

    //public int Speed { get; set; } = 5;
    public float Force { get; set; } = 5;

    #region Camera Settings etc.
    public Camera camera { get; set; }
    public float CameraSensitivity_x { get; set; } = 10;
    public float CameraSensitivity_y { get; set; } = 20;
    public float CameraHeight { get; set; } = 1;
    public float CameraMaxVertDegrees { get; set; } = 60;
    private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }
    public float CameraDistance { get; set; } = 2;
    public float CameraHeightThrowing { get; set; } = 0.8f;
    public float CameraDistanceThrowing { get; set; } = 0.5f;
    private float xStep = 0;
    private float yStep = 0;
    private float TotalYStep = 0;
    private float TotalXStep = 0;
    private Vector3 CameraStartPos;

    bool Escape = false;
    #endregion
    
    private Quaternion FreeLookDirection;
    Rigidbody rBody;
    ChadStateMachine ChadSM;


    //Camera test;

    private Ball Ball = null;
    public bool HasBall = false;
    private bool IsThrowing = false;
    // private bool canPickupBall = true;

    public override void Start()
    {
        rBody = gameObject.GetComponent<Rigidbody>();
        ChadSM = gameObject.GetComponent<ChadStateMachine>();
        //if (!isOwner && camera)
        //{
        //    camera.enabled = false;
        //    initalCameraPos = camera.transform.localPosition;
        //}


        if (!camera)
            Debug.LogWarning("Camera not set for player");

        //rBody.IsKinematic = !isOwner;
        ThrowForce = BaseThrowForce;
        Ball = GetObjectsOfType<Ball>().FirstOrDefault();
    }

    //Coroutine for jumping delay, also used for tackling delay
    //IEnumerator JumpingCoroutine()
    //{
    //    jumpDelay = false;
    //    Debug.Log("Started jumping.");
    //    rBody.AddForce(new Vector3(0.0f, Force, 0.0f), Rigidbody.ForceMode.Impulse);
    //    yield return new WaitForSeconds(1.0f);

    //    if (tackling)
    //    {
    //        transform.Rotate(0.0f, 0.5f, 0.0f);
    //        tackling = false;
    //        //test.fieldOfView = 70;
    //    }
    //    yield return new WaitForSeconds(0.2f);
    //    jumpDelay = true;
    //    jumping = false;
    //    movingForward = false;
    //    movingBackward = false;
    //}

    //public override void Update()
    //{

    //}

    public override void Update()
    {
        if (Escape)
            Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
        else
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);

        ChadSM.Direction = new Vector3(0, 0, 0);

        HandleKeyboardInput();
        HandleMouseInput();
    }

    private void HandleKeyboardInput()
    {
        if (Input.GetKeyUp(Input.Keys.Escape))
            Escape = true;

        if (Input.GetKey(Input.Keys.W))
            ChadSM.Direction.z += 1;
        if (Input.GetKey(Input.Keys.S))
            ChadSM.Direction.z -= 1;

        if (Input.GetKey(Input.Keys.D))
            ChadSM.Direction.x += 1;
        if (Input.GetKey(Input.Keys.A))
            ChadSM.Direction.x -= -1;
    }

    private void HandleMouseInput()
    {
        //Focus stuff
        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT) && Escape)
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);

        //Throw stuff
        if (HasBall)
        {
            if (Input.GetMouseButtonDown(Input.MouseButtons.RIGHT))
            {
                IsThrowing = true;
                ChadSM.EnterThrow();
            }
            else if (Input.GetMouseButtonUp(Input.MouseButtons.RIGHT) && IsThrowing)
            {
                IsThrowing = false;
                ChadSM.ExitThrow();
            }
            else if (Input.GetMouseButton(Input.MouseButtons.LEFT) && IsThrowing)
                ChargeBall();
            else if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT) && IsThrowing)
            {
                IsThrowing = false;
                ThrowBall();
            }
        }
        
        float xStep = Input.GetMouseX() * Time.ActualDeltaTime;
        float yStep = Input.GetMouseY() * Time.ActualDeltaTime;

        ChadSM.Direction.y = xStep;
        //ChadSM.Direction.Normalize();

        if (!Input.GetKey(Input.Keys.LeftShift) && !Input.GetMouseButton(Input.MouseButtons.RIGHT))
        {
            //Regular cam
            FondleCamera(ChadSM.m_velocity.Length(), xStep, yStep);
        }
        else if (Input.GetMouseButton(Input.MouseButtons.RIGHT))
        {
            //Throwing cam
            ThrowingCamera(ChadSM.m_velocity.Length(), xStep, yStep);
        }
        else if (Input.GetKeyDown(Input.Keys.LeftShift))
        {
            //Free look
            InitFreeLookCamera();
        }
        else
        {
            //Free look
            FreeLookCamera(ChadSM.m_velocity.Length(), xStep, yStep);
        }

        if (Input.GetKeyUp(Input.Keys.LeftShift) && !Input.GetMouseButton(Input.MouseButtons.RIGHT)) //released shift while not throwing
        {
            ResetCamera();
        }
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
        ThrowForce += ChargeRate * Time.DeltaTime;
    }

    public void ThrowBall()
    {
        Ball.Throw(camera.transform.forward * ThrowForce);
        HasBall = false;
    }

    public override void OnCollisionEnter(Collider collider)
    {
        if (Ball /*&& isOwner*/)
        {
            if (collider.gameObject == Ball.gameObject)
            {
                Ball.Pickup(gameObject, hand ? hand : transform);
                HasBall = true;
            }
        }
    }
}
