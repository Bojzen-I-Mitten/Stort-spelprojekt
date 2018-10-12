﻿/* TODO: 
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

        public int Speed { get; set; } = 5;
        public float Force { get; set; } = 5;
        public float CameraSensitivity_x { get; set; } = 10;
        public float CameraSensitivity_y { get; set; } = 20;
        public float CameraHeight { get; set; } = 1;
        public float CameraMaxVertDegrees { get; set; } = 60;
        private float CameraMaxVertRadians { get { return ThomasEngine.MathHelper.ToRadians(CameraMaxVertDegrees); } }
        public float CameraDistance { get; set; } = 2;
        private float TotalYStep = 0;
        private float TotalXStep = 0;
        private Vector3 CameraStartPos;

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
            rBody.AddForce(new Vector3(0.0f, Force, 0.0f), Rigidbody.ForceMode.Impulse);
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

            if (hasBall) FondleBall();
        }

        public void HandleMovement()
        {
            if (Input.GetKey(Input.Keys.W))
                transform.position += transform.forward * Speed * Time.DeltaTime;
            else if (Input.GetKey(Input.Keys.S))
                transform.position -= transform.forward * Speed * Time.DeltaTime;

            else if (Input.GetKey(Input.Keys.D))
                transform.position += transform.right * Speed * Time.DeltaTime;
            else if (Input.GetKey(Input.Keys.A))
                transform.position -= transform.right * Speed * Time.DeltaTime;
        }

        public void FondleCamera(float velocity, float xStep, float yStep)
        {
            if (camera)
            {
                float yaw = ThomasEngine.MathHelper.ToRadians(-xStep * CameraSensitivity_x);
                if(velocity != 0)
                    yaw = ClampCameraRadians(yaw, -1 / velocity, 1 / velocity);
                transform.RotateByAxis(transform.up, yaw);

                TotalXStep -= yaw; //for of freelook
                TotalYStep -= ThomasEngine.MathHelper.ToRadians(yStep * CameraSensitivity_y);
                TotalYStep = ClampCameraRadians(TotalYStep, -CameraMaxVertRadians, CameraMaxVertRadians);
                camera.transform.localRotation = Quaternion.CreateFromAxisAngle(Vector3.Right, TotalYStep);
                camera.transform.localPosition = Vector3.Transform(new Vector3(0, CameraHeight, CameraDistance), camera.transform.localRotation);

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
        public void FondleBall()
        {
            if (Input.GetMouseButton(Input.MouseButtons.LEFT) && throwForce < maxThrowForce)
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
