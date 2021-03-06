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
    public float textOffset = 1.5f;
    public Font NameFont { get; set; }
    public float textScale { get; set; } = 0.008f;
    public int HasTackled = 0;
    public int Owngoal = 0;
    public int GoalsScored = 0;
    public int Score = 0;
    public string shirtNumber = "00";
    public bool ReadyToStart = false;

    Rigidbody rb;
    Ragdoll rag;
    Canvas nameCanvas;
    Text text;
    ShirtRenderer shirt;
    Hatter hatter;
    private int _HatIndex = 0;
    public int HatIndex
    {
        get
        {
            return _HatIndex;
        }
        set
        {
            if (hatter)
            {
                if(_HatIndex != value)
                    UpdateHat(value);
            }
        }
    }


    public override void OnAwake()
    {
        rag = gameObject.GetComponent<Ragdoll>();
        rb = gameObject.GetComponent<Rigidbody>();
        shirt = gameObject.GetComponent<ShirtRenderer>();
        hatter = gameObject.AddComponent<Hatter>();
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

        nameCanvas = CameraMaster.instance.Camera.AddCanvas();
        text = nameCanvas.Add("");
        text.font = NameFont;
        text.scale = new Vector2(0.05f);
        text.origin = new Vector2(0.5f, 0.5f);
        nameCanvas.is3D = true;
        if (isOwner)
            PlayerName = CameraMaster.instance.GetPlayerName();

        UpdateHat(_HatIndex);
    }

    public int GetPing()
    {
        return Identity.Ping;
    }

    private void UpdateHat(int newHatIndex)
    {
        if (hatter.SetHat(newHatIndex))
        {
            _HatIndex = newHatIndex;
        }
    }

    public override void OnDisable()
    {
        if (nameCanvas != null)
            nameCanvas.isRendering = false;
    }

    public override void OnEnable()
    {
        if (isOwner)
        {
            string ip = NetUtils.GetLocalIp(LocalAddrType.IPv4);
            shirtNumber = ip.Substring(ip.Length - 2);
        }
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
        if (transform.position.y < BottomOfTheWorld)
            Reset();

        if (shirt)
        {
            shirt.PlayerName = PlayerName;
            shirt.number = shirtNumber;
            if (Team != null)
            {
                shirt.TeamName = Team.Name;
                shirt.Team = Team.TeamType;
                shirt.Color = Team.Color;
            }
        }
            
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {

        writer.Put(HatIndex);
        writer.Put(PlayerName);
        writer.Put(ReadyToStart);
        writer.Put(HasTackled);
        writer.Put(Owngoal);
        writer.Put(GoalsScored);
        writer.Put(shirtNumber);
        if (Team != null)
            writer.Put((int)Team.TeamType);
        else
            writer.Put((int)TEAM_TYPE.UNASSIGNED);

        return true;
    }

    public override void OnDisconnect()
    {
        HasTackled = 0;
        Owngoal = 0;
        GoalsScored = 0;
        Score = 0;
        JoinTeam(MatchSystem.instance.FindTeam(TEAM_TYPE.UNASSIGNED));
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {

        HatIndex = reader.GetInt();
        PlayerName = reader.GetString();
        ReadyToStart = reader.GetBool();
        HasTackled = reader.GetInt();
        Owngoal = reader.GetInt();
        GoalsScored = reader.GetInt();
        shirtNumber = reader.GetString();
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
        if (isOwner && (int)Team.TeamType > (int)TEAM_TYPE.TEAM_SPECTATOR)
        {
            gameObject.GetComponent<ChadControls>().Reset();

            transform.position = Team.GetSpawnPosition();
            transform.LookAt(new Vector3(0, 0, 0));

            rb.Position = transform.position;
            rb.Rotation = transform.rotation;
            rb.IgnoreNextTransformUpdate();
        }

        if (isOwner)
        {
            if (Team.TeamType == TEAM_TYPE.TEAM_SPECTATOR)
            {
                CameraMaster.instance.gameObject.GetComponent<ChadCam>().enabled = false;
                CameraMaster.instance.gameObject.GetComponent<SpectatorCam>().enabled = true;
            }
            else
            {
                Debug.Log("Reset NetworkPlayer");
                CameraMaster.instance.gameObject.GetComponent<ChadCam>().enabled = true;
                CameraMaster.instance.gameObject.GetComponent<SpectatorCam>().enabled = false;
            }
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
        }

    }

    public void Ready(bool ready)
    {
        ReadyToStart = ready;
        if (isOwner)
            SendRPC("Ready", ReadyToStart);
    }

    public bool GetReady()
    {
        return ReadyToStart;
    }
}
