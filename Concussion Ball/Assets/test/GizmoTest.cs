using ThomasEngine;
using ThomasEngine.Script;
using System;

public class GizmoTest : ScriptComponent
{


    public override void Start()
    {
    }
    public override void Update()
    {
        Vector3 x = Vector3.Zero, y = Vector3.Zero, z = Vector3.Zero;
        Gizmos.SetMatrix(Matrix.Identity);
        Gizmos.SetColor(new Color(1f, 0f, 0f));
        Gizmos.DrawLine(Vector3.Zero, Vector3.Right);
        Gizmos.SetColor(new Color(0f, 0f, 1f));
        Gizmos.DrawLine(Vector3.Zero, new Vector3(0, 0, 1f));
        Gizmos.SetColor(new Color(0f, 1f, 0f));
        Gizmos.DrawLine(Vector3.Zero, Vector3.Up);

    }
}
