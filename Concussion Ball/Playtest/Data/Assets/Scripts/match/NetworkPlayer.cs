using System;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;
using System.Collections;

public class NetworkPlayer : NetworkComponent
{
    public String PlayerName = "";
    [Newtonsoft.Json.JsonIgnore]
    public Team Team { get; set; }
    public float BottomOfTheWorld { get; set; } = -5;
    public float ragdollOffset = 1.0f;
    public float textOffset = 1.3f;
    public Font NameFont { get; set; }
    public float textScale { get; set; } = 0.008f;
    public int HasTackled = 0;
    public int Owngoal = 0;
    public int GoalsScored = 0;
    public int Score = 0;
    public bool ReadyToStart = false;

    Material mat;
    Rigidbody rb;
    Ragdoll rag;
    Canvas nameCanvas;
    Text text;
    public override void OnAwake()
    {
        rag = gameObject.GetComponent<Ragdoll>();
        rb = gameObject.GetComponent<Rigidbody>();
        if (Team == null)
            Team = MatchSystem.instance.FindTeam(TEAM_TYPE.UNASSIGNED);
    }

    public override void Start()
    {
        HasTackled = 0;
        Owngoal = 0;
        GoalsScored = 0;
        if (Team == null || Team.TeamType == TEAM_TYPE.TEAM_SPECTATOR || Team.TeamType == TEAM_TYPE.UNASSIGNED)
            gameObject.SetActive(false);
        RenderSkinnedComponent model = gameObject.GetComponent<RenderSkinnedComponent>();
        if(!model)
        {
            throw new InvalidOperationException("Player requires a RenderSkinnedComponent.");
        }
        mat = model.CreateMaterialInstance("Chad66");
        if(mat == null)
        {
            throw new InvalidOperationException("Player not assigned Chad66 material.");
        }

        nameCanvas = CameraMaster.instance.Camera.AddCanvas();
        text = nameCanvas.Add("");
        text.font = NameFont;
        text.scale = new Vector2(0.05f);
        text.origin = new Vector2(0.5f, 0.5f);
        nameCanvas.is3D = true;
        PlayerName = GUIMainMenu.PlayerString;
    }

    public int GetPing()
    {
        return Identity.Ping;
    }


    public override void OnDisable()
    {
        if (nameCanvas != null)
            nameCanvas.isRendering = false;
    }

    public override void Update()
    {
        if (!isOwner)
        {
            Vector3 betweenChads = Vector3.Zero;
            if (!rag.RagdollEnabled)
                betweenChads = transform.position - ChadCam.instance.transform.position;
            else
                betweenChads = rag.GetHips().transform.position - ChadCam.instance.transform.position;
            betweenChads.Normalize();
            Ray ray = new Ray(ChadCam.instance.transform.position, betweenChads);
            RaycastHit info;
            Physics.Raycast(ray, out info);
            if (info.rigidbody == rb)
            {
                text.text = PlayerName;
                text.scale = new Vector2(textScale);
                text.color = Team.Color;
                Vector3 position = Vector3.Zero;
                if (rag.RagdollEnabled)
                    position = rag.GetHips().transform.position + new Vector3(0, ragdollOffset, 0);
                else
                    position = rb.Position + new Vector3(0, textOffset, 0);

                nameCanvas.isRendering = true;
                nameCanvas.worldMatrix = Matrix.CreateConstrainedBillboard(position, CameraMaster.instance.Camera.transform.position, Vector3.Down, null, null);
            }
            else if (nameCanvas != null)
                nameCanvas.isRendering = false;
        }
        else if (nameCanvas != null)
            nameCanvas.isRendering = false;
        if (Team != null && mat != null)
            mat.SetColor("color", Team.Color);
        if (transform.position.y < BottomOfTheWorld)
            Reset();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {

        writer.Put(PlayerName);
        writer.Put(ReadyToStart);
        writer.Put(HasTackled);
        writer.Put(Owngoal);
        writer.Put(GoalsScored);
        if (Team != null)
            writer.Put((int)Team.TeamType);
        else
            writer.Put((int)TEAM_TYPE.UNASSIGNED);

        return true;
    }

    public override void OnDisconnect()
    {
        JoinTeam(TEAM_TYPE.UNASSIGNED);
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {


        PlayerName = reader.GetString();
        ReadyToStart = reader.GetBool();
        HasTackled = reader.GetInt();
        Owngoal = reader.GetInt();
        GoalsScored = reader.GetInt();

        TEAM_TYPE teamType = (TEAM_TYPE)reader.GetInt();
        Team newTeam = MatchSystem.instance.FindTeam(teamType);
        if (Team != newTeam)
            JoinTeam(newTeam);
        if (initialState)
        {
            if (teamType == TEAM_TYPE.TEAM_1 || teamType == TEAM_TYPE.TEAM_2)
                gameObject.SetActive(true);
        }
    }

    public void JoinTeam(TEAM_TYPE teamType)
    {
        HasTackled = 0;
        Owngoal = 0;
        GoalsScored = 0;
        Score = 0;
        RPCJoinTeam((int)teamType);
        SendRPC("RPCJoinTeam", (int)teamType);
    }

    public void RPCJoinTeam(int teamType)
    {
        Team newTeam = MatchSystem.instance.FindTeam((TEAM_TYPE)teamType);
        JoinTeam(newTeam);
    }

    public void Reset()
    {
        mat?.SetColor("color", Team.Color);
        if (isOwner && (int)Team.TeamType > (int)TEAM_TYPE.TEAM_SPECTATOR)
        {
            gameObject.GetComponent<ChadControls>().Reset();

            transform.position = Team.GetSpawnPosition();
            transform.LookAt(new Vector3(0, transform.position.y, 0));

            rb.Position = transform.position;
            rb.Rotation = transform.rotation;
            rb.IgnoreNextTransformUpdate();
        }

        if (Team.TeamType == TEAM_TYPE.TEAM_SPECTATOR)
        {
            CameraMaster.instance.gameObject.GetComponent<ChadCam>().enabled = false;
            CameraMaster.instance.gameObject.GetComponent<SpectatorCam>().enabled = true;
        }
        else
        {
            CameraMaster.instance.gameObject.GetComponent<ChadCam>().enabled = true;
            CameraMaster.instance.gameObject.GetComponent<SpectatorCam>().enabled = false;
        }

    }
    public void JoinTeam(Team team)
    {
        if (this.Team == team)
        {
            Debug.LogWarning("Player is already in team.");
            return;
        }

        if (this.Team != null)
        {
            this.Team.RemovePlayer(this);
        }
        if (team != null)
        {
            team.AddPlayer(this);
            mat?.SetColor("color", Team.Color);
        }

    }

    public void Ready(bool ready)
    {
        ReadyToStart = ready;
        if (isOwner)
            SendRPC("Ready", ReadyToStart);
    }
}