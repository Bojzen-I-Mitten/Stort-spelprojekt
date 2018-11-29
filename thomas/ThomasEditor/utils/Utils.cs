using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

namespace ThomasEditor.utils
{
    class Utils
    {
        /* Iterate the data values and ensure they are not null.
        */
        public static List<GameObject> VerifyValidObjects(IEnumerable<GameObject> values)
        {
            List<GameObject> newList = new List<GameObject>();
            foreach (GameObject e in values)
            {
                if (ThomasWrapper.CurrentScene.InScene(e))
                {
                    newList.Add(e);
                }
            }
            return newList;
        }
        /* Iterate the data values and ensure they are not null.
        */
        public static List<GameObject> DetachParents(IEnumerable<GameObject> values)
        {
            List<GameObject> newList = new List<GameObject>();
            foreach (GameObject e in values)
            {
                if(e.transform.parent == null || !values.Contains(e.transform.parent.gameObject))
                {
                    newList.Add(e);
                }
            }
            return newList;
        }
    }
}
