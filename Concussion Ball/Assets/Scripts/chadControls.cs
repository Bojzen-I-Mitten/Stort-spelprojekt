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

namespace ThomasEditor
{
    public class ChadControls : NetworkComponent
    {
        public float baseThrowForce { get; set; } = 5.0f;
        public float maxThrowForce { get; set; } = 20.0f;
       
        public int speed { get; set; } = 5;
        public float force { get; set; } = 5;
        public float cameraSensitivity_x { get; set; } = 10;
        public float cameraSensitivity_y { get; set; } = 20;
        public float cameraHeight { get; set; } = 1;
        public float cameraVertAngle { get; set; } = 60;
        private float camVertRadians { get { return ThomasEngine.MathHelper.ToRadians(cameraVertAngle); } }

        public float cameraDistance { get; set; } = 2;

        private float throwForce;

        public Transform hand { get; set; }

        public Camera camera { get; set; }
        private Quaternion FreeLookDirection;
        Rigidbody rBody;
 
        //Camera test;

        private Ball ball = null;
        private bool hasBall = false;
        private bool canPickupBall = true;

        bool jumpDelay = true;
        bool movingForward = false;
        bool movingBackward = false;
        bool tackling = false;
        bool jumping = false;

        public override void Start()
        {
            rBody = gameObject.GetComponent<Rigidbody>();
            //test = camPrefab.GetComponent<Camera>();
            //if (!isOwner && camera)
            //{
            //    camera.enabled = false;
            //    initalCameraPos = camera.transform.localPosition;
            //}
                

            if (!camera)
                Debug.LogWarning("Camera not set for player");

            //rBody.IsKinematic = !isOwner;
            throwForce = baseThrowForce;
            ball = GetObjectsOfType<Ball>().FirstOrDefault();
        }

        //Coroutine for jumping delay, also used for tackling delay
        IEnumerator JumpingCoroutine()
        {
            jumpDelay = false;
            Debug.Log("Started jumping.");
            rBody.AddForce(new Vector3(0.0f, force, 0.0f), Rigidbody.ForceMode.Impulse);
            yield return new WaitForSeconds(1.0f);

            if (tackling)
            {
                transform.Rotate(0.0f, 0.5f, 0.0f);
                tackling = false;
                //test.fieldOfView = 70;
            }
            yield return new WaitForSeconds(0.2f);
            jumpDelay = true;
            jumping = false;
            movingForward = false;
            movingBackward = false;
        }

        public override void Update()
        {
            //if (!isOwner)
            //    return;
            ////Jumping
            //if (Input.GetKey(Input.Keys.Space) && jumpDelay)
            //{
            //    //Set bools to avoid direction changes mid air or tackling mid air
            //    jumping = true;
            //    if (Input.GetKey(Input.Keys.W))
            //        movingForward = true;
            //    if (Input.GetKey(Input.Keys.S))
            //        movingBackward = true;
            //    StartCoroutine(JumpingCoroutine());
            //}

            //HandleMovement();

            if(hasBall) FondleBall();
        }

        public void HandleMovement()
        {
            if (Input.GetKey(Input.Keys.W))
                transform.position += transform.forward * speed * Time.DeltaTime;
            else if (Input.GetKey(Input.Keys.S))
                transform.position -= transform.forward * speed * Time.DeltaTime;

            else if (Input.GetKey(Input.Keys.D))
                transform.position += transform.right * speed * Time.DeltaTime;
            else if (Input.GetKey(Input.Keys.A))
                transform.position -= transform.right * speed * Time.DeltaTime;
        }

        public void FondleCamera(float velocity, float xStep, float yStep)
        {
            float _xStep = xStep;
            //Limit turn speed------------------------------
            float xStepSign = Math.Sign(xStep);
            float TurnRatio = 1;
            if (velocity != 0)
                TurnRatio = 1 / velocity;
            _xStep = Math.Min(Math.Abs(xStep), TurnRatio);
            _xStep *= xStepSign;
            //----------------------------------------------

            transform.RotateByAxis(transform.up, ThomasEngine.MathHelper.ToRadians(-_xStep * cameraSensitivity_x));

            if (camera)
            {
                //camera.transform.localEulerAngles += new Vector3(-yStep * cameraSensitivity_y, 0, 0);
                camera.transform.RotateByAxis(new Vector3(1, 0, 0), ThomasEngine.MathHelper.ToRadians(-yStep * cameraSensitivity_y));
                Vector3 camForwardXZ = new Vector3(camera.transform.forward.x, 0, camera.transform.forward.z);
                camForwardXZ.Normalize();
                Debug.Log(camForwardXZ);
                float forwardAngle = (float)Math.Acos(Vector3.Dot(camForwardXZ, camera.transform.forward));
                //float forwardSign = Math.Sign(Vector3.Cross(tempForward, camera.transform.forward).y);
                if (camVertRadians < forwardAngle || forwardAngle < 0)
                {
                    Debug.Log("Angle: " + forwardAngle);
                    camera.transform.rotation = Quaternion.CreateFromYawPitchRoll(camera.transform.eulerAngles.x, camVertRadians, camera.transform.eulerAngles.z);
                }

                camera.transform.position = Vector3.Transform(new Vector3(0, cameraHeight, cameraDistance), camera.transform.localRotation) + transform.position;
            }
        }

        public void FreeLookCamera(float velocity, float xStep, float yStep)
        {
            if (camera)
            {
                camera.transform.localEulerAngles += new Vector3(-yStep * cameraSensitivity_y, -xStep * cameraSensitivity_x, 0);

                Vector3 camForwardXZ = new Vector3(camera.transform.forward.x, 0, camera.transform.forward.z);
                camForwardXZ.Normalize();
                Debug.Log(camForwardXZ);
                float forwardAngle = (float)Math.Acos(Vector3.Dot(camForwardXZ, camera.transform.forward));
                if (camVertRadians < forwardAngle || forwardAngle < 0)
                {
                    Debug.Log("Angle: " + forwardAngle);
                    camera.transform.rotation = Quaternion.CreateFromYawPitchRoll(camera.transform.eulerAngles.x, camVertRadians, camera.transform.eulerAngles.z);
                }

                camera.transform.position = Vector3.Transform(new Vector3(0, cameraHeight, cameraDistance), camera.transform.localRotation) + transform.position;
            }
        }

        public void ResetCamera()
        {
            if (camera)
            {
                //Find the angle to reset the camera behind the player
                Vector3 camForwardXZ = new Vector3(camera.transform.forward.x, 0, camera.transform.forward.z);
                camForwardXZ.Normalize();
                float angle = (float)Math.Acos(Vector3.Dot(camForwardXZ, transform.forward));
                float sign = Math.Sign(Vector3.Cross(camForwardXZ, transform.forward).y);
                angle *= sign;
                //--------------------------------------------------

                //Rotate by found angle, and shift position behind player, and up a little
                camera.transform.RotateByAxis(transform.up, angle);
                camera.transform.position = transform.position + new Vector3(0, cameraHeight, cameraDistance);
                camera.transform.localEulerAngles = new Vector3(0, 0, 0);
            }
        }

        public void FondleBall()
        {
            if(Input.GetMouseButton(Input.MouseButtons.LEFT) && throwForce < maxThrowForce)
            {
                throwForce += 5.0f * Time.DeltaTime;
                if (throwForce > 19)
                    Debug.Log("Current throw force: " + (int)throwForce);
                ball.ChargeColor(throwForce);
            }
            if (hand && camera && Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            {
                hasBall = false;

                ball.Throw(camera.transform.forward * throwForce);
                throwForce = baseThrowForce;
            }
        }

        public override void OnCollisionEnter(Collider collider)
        {
            if (ball /*&& isOwner*/)
            {
                if (collider.gameObject == ball.gameObject)
                {
                    //TakeOwnership(collider.gameObject);
                    ball.Pickup(gameObject, hand ? hand : transform);
                    hasBall = true;
                    canPickupBall = false;
                }
            }
        }
    }
}
