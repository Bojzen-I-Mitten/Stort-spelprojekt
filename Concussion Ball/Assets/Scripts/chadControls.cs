/* TODO: 
 * Turn standing still
 * Can't press S while going forward in air, can't press W while going backward in air
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

        public override void Start()
        {
            rBody = gameObject.GetComponent<Rigidbody>();

            //Changes nothing
            rBody.Mass = 1.0f;
        }

        IEnumerator JumpingCoroutine()
        {
            jumpDelay = false;
            rBody.ApplyCentralImpulseForce(new Vector3(0.0f, force, 0.0f));
            Debug.Log("Started jumping.");
            yield return new WaitForSeconds(1.0f);
            jumpDelay = true;
            movingForward = false;
            movingBackward = false;
            Debug.Log("Coroutine has waited for 1 second.. Ready to jump again.");
        }

        public override void Update()
        {  
            //Jumping, change to time based.
            if (Input.GetKeyDown(Input.Keys.Space) && jumpDelay)
            {
                if (Input.GetKey(Input.Keys.W))
                    movingForward = true;
                if (Input.GetKey(Input.Keys.S))
                    movingBackward = true;
                StartCoroutine(JumpingCoroutine());
            }   
            //WASD movement
            //if (movingForward)
            if (Input.GetKey(Input.Keys.S) && !movingForward)
            {
                transform.position -= transform.forward * speed * Time.DeltaTime;
                if (Input.GetKey(Input.Keys.A))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
                if (Input.GetKey(Input.Keys.D))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
            }
            if (Input.GetKey(Input.Keys.W) && !movingBackward)
            {
                transform.position += transform.forward * speed * Time.DeltaTime;
                if (Input.GetKey(Input.Keys.A))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
                if (Input.GetKey(Input.Keys.D))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
            }
            if(!Input.GetKey(Input.Keys.W) && !Input.GetKey(Input.Keys.S))
            {
                if (Input.GetKey(Input.Keys.A))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
                if (Input.GetKey(Input.Keys.D))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
            }

            //Avoid tumbling TEST??
            //Quaternion test = Quaternion.CreateFromYawPitchRoll(transform.rotation.y, 0.0f, 0.0f);
            //transform.rotation = test;
        }
    }
}
