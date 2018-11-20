using System;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;
using System.Collections;

public class NetworkPlayer : NetworkComponent
{
    public String PlayerName = "Chad";
    [Newtonsoft.Json.JsonIgnore]
    public Team Team { get; set; }
    public float BottomOfTheWorld { get; set; } = -5;
    public float ragdollOffset = 0.8f;
    public float textOffset = 1.3f;
    public Font NameFont { get; set; }
    public float textScale { get; set; } = 0.008f;
    public int HasTackled = 0;
    public int BeenTackled = 0;
    public int GoalsScored = 0;

    Material mat;
    Rigidbody rb;
    Ragdoll rag;
    Canvas nameCanvas;
    Text text;
    public override void Awake()
    {
        rag = gameObject.GetComponent<Ragdoll>();
        rb = gameObject.GetComponent<Rigidbody>();
        if (Team == null)
            Team = MatchSystem.instance.FindTeam(TEAM_TYPE.UNASSIGNED);
    }

    public override void Start()
    {
        if (Team == null || Team.TeamType == TEAM_TYPE.TEAM_SPECTATOR || Team.TeamType == TEAM_TYPE.UNASSIGNED)
            gameObject.SetActive(false);
        mat = (gameObject.GetComponent<RenderSkinnedComponent>().material = new Material(gameObject.GetComponent<RenderSkinnedComponent>().material));

        nameCanvas = CameraMaster.instance.Camera.AddCanvas();
        text = nameCanvas.Add("");
        text.font = NameFont;
        text.scale = new Vector2(0.05f);
        text.origin = new Vector2(0.5f, 0.5f);
        nameCanvas.is3D = true;
    }

    public override void Update()
    {
        if (!isOwner)
        {
            text.text = PlayerName;
            text.scale = new Vector2(textScale);
            text.color = Team.Color;
            Vector3 position = Vector3.Zero;
            if (rag.RagdollEnabled)
                position = rag.GetHips().transform.position + new Vector3(0, ragdollOffset, 0);
            else
                position = rb.Position + new Vector3(0, textOffset, 0);

            nameCanvas.worldMatrix = Matrix.CreateConstrainedBillboard(position, CameraMaster.instance.Camera.transform.position, Vector3.Down, null, null);
        }


        if (transform.position.y < BottomOfTheWorld)
            Reset();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        if(initialState)
        {
            writer.Put(PlayerName);
        }

        if (Team != null)
            writer.Put((int)Team.TeamType);
        else
            writer.Put((int)TEAM_TYPE.UNASSIGNED);
        return true;
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {

        if(initialState)
        {
            PlayerName = reader.GetString();
        }

        TEAM_TYPE teamType = (TEAM_TYPE)reader.GetInt();
        Team newTeam = MatchSystem.instance.FindTeam(teamType);
        if (Team != newTeam)
            JoinTeam(newTeam);
        if (initialState)
        {
            if (teamType == TEAM_TYPE.TEAM_1 || teamType == TEAM_TYPE.TEAM_2)
                gameObject.SetActive(true);
            if(Team != null)
                mat?.SetColor("color", Team.Color);
        }

    }
    
    public void JoinTeam(TEAM_TYPE teamType)
    {
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

        if(Team.TeamType == TEAM_TYPE.TEAM_SPECTATOR)
        {
            CameraMaster.instance.gameObject.GetComponent<ChadCam>().enabled = false;
            CameraMaster.instance.gameObject.GetComponent<SpectatorCam>().enabled = true;
        }else
        {
            CameraMaster.instance.gameObject.GetComponent<ChadCam>().enabled = true;
            CameraMaster.instance.gameObject.GetComponent<SpectatorCam>().enabled = false;
        }
        
    }


    public void JoinTeam(Team team)
    {
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
}
