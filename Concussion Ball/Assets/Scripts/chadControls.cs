/* TODO: 
 * Freeze rotation x and z axes 
 */

using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;

namespace ThomasEditor
{
    public class ChadControls : ScriptComponent
    //public class ChadControls : NetworkComponent
    {

        public float baseThrowForce { get; set; } = 5.0f;
        public float maxThrowForce { get; set; } = 20.0f;
       
        public int speed { get; set; } = 5;
        public float force { get; set; } = 5;
        public float cameraSensitivity { get; set; } = 1;
        public float cameraDistance { get; set; } = 2;

        private float throwForce;

        public Transform hand { get; set; }

        public Camera camera { get; set; }
        private Vector3 initalCameraPos;
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
            if (/*!isOwner && */camera)
            {
                camera.enabled = false;
                initalCameraPos = camera.transform.localPosition;
            }
                

            if (!camera)
                Debug.LogWarning("Camera not set for player");

            //rBody.IsKinematic = !isOwner;
            throwForce = baseThrowForce;
            ball = Object.GetObjectsOfType<Ball>().FirstOrDefault();
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

            HandleMovement();

            if(hasBall) FondleBall();
        }

        public void HandleMovement()
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            float xStep = Input.GetMouseX() * Time.ActualDeltaTime * cameraSensitivity;
            float yStep = Input.GetMouseY() * Time.ActualDeltaTime * cameraSensitivity * 20.0f;

            Quaternion rot = Quaternion.CreateFromAxisAngle(transform.right, -yStep);
            rot *= Quaternion.CreateFromAxisAngle(transform.up, -xStep);

            Vector3 up = Vector3.Transform(transform.up, rot);
            Vector3 forward = Vector3.Transform(transform.forward, rot);
            if (camera)
            {
                camera.transform.localEulerAngles += new Vector3(-yStep, 0, 0);

                camera.transform.position = Vector3.Transform(new Vector3(0.0f, 1.7f, cameraDistance), camera.transform.localRotation)
                    + transform.position;
            }

            transform.RotateByAxis(transform.up, -xStep);

            if (Input.GetKey(Input.Keys.W))
                transform.position += transform.forward * speed * Time.DeltaTime;
            else if (Input.GetKey(Input.Keys.S))
                transform.position -= transform.forward * speed * Time.DeltaTime;

            else if (Input.GetKey(Input.Keys.D))
                transform.position += transform.right * speed * Time.DeltaTime;
            else if (Input.GetKey(Input.Keys.A))
                transform.position -= transform.right * speed * Time.DeltaTime;
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
