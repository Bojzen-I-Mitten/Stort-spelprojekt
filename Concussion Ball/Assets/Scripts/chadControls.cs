/* TODO: 
 * Freeze rotation x and z axes 
 */

using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

namespace ThomasEditor
{
    public class ChadControls : ScriptComponent
    {
        public int speed { get; set; } = 5;
        public float force { get; set; } = 5;

        Rigidbody rBody;

        bool jumpDelay = true;
        bool movingForward = false;
        bool movingBackward = false;
        bool tackling = false;
        bool jumping = false;

        public override void Start()
        {
            rBody = gameObject.GetComponent<Rigidbody>();
        }

        //Coroutine for jumping delay, also used for tackling delay
        IEnumerator JumpingCoroutine()
        {
            jumpDelay = false;
            rBody.ApplyCentralImpulseForce(new Vector3(0.0f, force, 0.0f));
            yield return new WaitForSeconds(1.0f);
            jumpDelay = true;
            movingForward = false;
            movingBackward = false;
            tackling = false;
            jumping = false;
        }

        public override void Update()
        {  
            //Jumping
            if (Input.GetKeyDown(Input.Keys.Space) && jumpDelay)
            {
                //Set bools to avoid direction changes mid air or tackling mid air
                jumping = true;
                if (Input.GetKey(Input.Keys.W))
                    movingForward = true;
                if (Input.GetKey(Input.Keys.S))
                    movingBackward = true;
                StartCoroutine(JumpingCoroutine());
            }

            //Tackling
            if (Input.GetKeyDown(Input.Keys.LeftShift) && !tackling && !jumping)
            {
                tackling = true;
                StartCoroutine(JumpingCoroutine());
            }

            //Set double speed for tackling
            if (tackling)
                transform.position += transform.forward * speed * 2 * Time.DeltaTime;

            //If character is in air and player not pressing W, should still move forward, can't stop mid air
            if (movingForward && !Input.GetKey(Input.Keys.W) && !tackling)
                transform.position += transform.forward * speed * Time.DeltaTime;
            
            //Same, but for backward
            if (movingBackward && !Input.GetKey(Input.Keys.S))
                transform.position -= transform.forward * speed * Time.DeltaTime;

            //If player is pressing S and is not moving forward in air.
            if (Input.GetKey(Input.Keys.S) && !movingForward && !tackling)
            {
                transform.position -= transform.forward * speed * Time.DeltaTime;
                if (Input.GetKey(Input.Keys.A))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
                if (Input.GetKey(Input.Keys.D))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
            }
            //Same, but for forward
            if (Input.GetKey(Input.Keys.W) && !movingBackward && !tackling)
            {
                transform.position += transform.forward * speed * Time.DeltaTime;
                if (Input.GetKey(Input.Keys.A))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
                if (Input.GetKey(Input.Keys.D))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
            }
            //If character is standing still
            if(!Input.GetKey(Input.Keys.W) && !Input.GetKey(Input.Keys.S))
            {
                if (Input.GetKey(Input.Keys.A))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
                if (Input.GetKey(Input.Keys.D))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
            }
        }
    }
}
