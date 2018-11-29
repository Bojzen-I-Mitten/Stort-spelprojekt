using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using System.Collections.Generic;
using LiteNetLib.Utils;
using System.Collections;
public class ReplaySystem : ScriptComponent
{

    struct ReplayState
    {
        public bool initialState;
        public float timestamp;
        public NetDataReader reader;
    }
    public float durationInSeconds { get; set; } = 10.0f;
    public float saveInterval = 0.1f;
    public float initialStateInterval = 1.0f;
    float timeSinceLastSave = 0.1f;
    float timeSinceLastIntialStateSave = 1.0f;
    List<ReplayState> States = new List<ReplayState>();
    public bool Replaying = false;
    public bool recordGame = true;
    SpectatorCam specCam;

    public override void Start()
    {
        specCam = CameraMaster.instance.gameObject.GetComponent<SpectatorCam>();
    }

    public override void Update()
    {
        if (!MatchSystem.instance || !MatchSystem.instance.MatchStarted || Replaying || !recordGame)
            return;

        saveInterval = MatchSystem.instance.InternalManager.UpdateTime / 1000.0f;

        timeSinceLastSave += Time.ActualDeltaTime;
        timeSinceLastIntialStateSave += Time.ActualDeltaTime;
        if(timeSinceLastSave > saveInterval)
        {
            timeSinceLastSave = 0.0f;
            if (timeSinceLastIntialStateSave > initialStateInterval)
            {
                SaveObjectState(true);
                timeSinceLastIntialStateSave = 0.0f;
            }
                SaveObjectState(false);
        }

        if (States.Count > 0 && (Time.ElapsedTime - States[0].timestamp) > durationInSeconds + 1.0f)
        {
            States.RemoveAt(0);
            RemoveUntilInitialState();
        }
    }

    public void StartReplay(Team teamThatScored)
    {
        if(!Replaying)
        {
            Vector3 goalPos = MatchSystem.instance.Teams[MatchSystem.instance.GetOpposingTeam(teamThatScored.TeamType)].GoalPosition;
            StartCoroutine(RelayCoroutine(goalPos));
        }
    }

    private IEnumerator RelayCoroutine(Vector3 goalPos)
    {
        MatchSystem.instance.blockIncomingData = true;
        CameraMaster.instance.StartReplay();
        specCam.transform.position = goalPos + new Vector3(goalPos.z / 1.2f, 5, 0);
        MatchSystem.instance.ReadOwnerAsNormal = true;
        Replaying = true;
        ReplayState initialState = States[0];
        RemoveAllInitialStates();
        float currentTime = initialState.timestamp;
        LoadObjectState(initialState);

        while(States.Count > 0)
        {
            while(currentTime < States[0].timestamp)
            {
                currentTime += Time.ActualDeltaTime;
                MatchSystem.instance.MatchStartTime += Time.ActualDeltaTime;
                yield return null;
            }
            LoadObjectState(States[0]);
            States.RemoveAt(0);
        }
        Replaying = false;
        MatchSystem.instance.ReadOwnerAsNormal = false;
        MatchSystem.instance.blockIncomingData = false;
        CameraMaster.instance.StopReplay();

    }


    private void RemoveUntilInitialState()
    {
        while(States.Count > 0 && !States[0].initialState)
        {
            States.RemoveAt(0);
        }
    }

    private void RemoveAllInitialStates()
    {
        for(int i=0; i < States.Count; i++)
        {
            if(States[i].initialState)
            {
                States.RemoveAt(i);
                i--;
            }
        }
    }

    private void SaveObjectState(bool initialState)
    {
        NetDataWriter writer = new NetDataWriter(true);

        foreach(var player in MatchSystem.instance.Scene.AllPlayers)
        {
            player.WriteData(writer, initialState);
        }

        foreach (var netObj in MatchSystem.instance.Scene.NetworkObjects)
        {
            writer.Put(netObj.Key); //Network ID
            netObj.Value.WriteData(writer, initialState);
        }
        NetDataReader reader = new NetDataReader();
        reader.SetSource(writer);

        ReplayState state = new ReplayState
        {
            initialState = initialState,
            timestamp = Time.ElapsedTime,
            reader = reader
        };
        States.Add(state);
    }

    private void LoadObjectState(ReplayState state)
    {

        foreach (var player in MatchSystem.instance.Scene.AllPlayers)
        {
            player.ReadData(state.reader, state.initialState);
        }

        while (!state.reader.EndOfData)
        {
            int id = state.reader.GetInt();
            NetworkIdentity identity = MatchSystem.instance.Scene.NetworkObjects[id];
            identity.ReadData(state.reader, state.initialState);
        }
        
    }

}
