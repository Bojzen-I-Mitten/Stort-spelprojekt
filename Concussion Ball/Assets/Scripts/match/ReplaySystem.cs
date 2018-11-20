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
    public float durationInSeconds = 10.0f;
    public float saveInterval = 0.1f;
    public float initialStateInterval = 1.0f;
    float timeSinceLastSave = 0.1f;
    float timeSinceLastIntialStateSave = 1.0f;
    List<ReplayState> States = new List<ReplayState>();
    bool replaying = false;
    public override void Start()
    {
    }

    public override void Update()
    {
        if (!MatchSystem.instance || !MatchSystem.instance.MatchStarted || replaying)
            return;

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

        if (States.Count > 0 && (MatchSystem.instance.ElapsedMatchTime - States[0].timestamp) > durationInSeconds + 1.0f)
        {
            States.RemoveAt(0);
            RemoveUntilInitialState();
        }
        if (Input.GetKeyDown(Input.Keys.P))
            StartCoroutine(StartReplay());
    }

    private IEnumerator StartReplay()
    {
        MatchSystem.instance.ReadOwnerAsNormal = true;
        replaying = true;
        ReplayState initialState = States[0];
        RemoveAllInitialStates();
        float currentTime = initialState.timestamp;
        LoadObjectState(initialState);

        while(States.Count > 0)
        {
            while(currentTime < States[0].timestamp)
            {
                currentTime += Time.ActualDeltaTime;
                yield return null;
            }
            LoadObjectState(States[0]);
            States.RemoveAt(0);
        }
        replaying = false;
        MatchSystem.instance.ReadOwnerAsNormal = false;
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
            timestamp = MatchSystem.instance.ElapsedMatchTime,
            reader = reader
        };
        States.Add(state);
    }

    private void LoadObjectState(ReplayState state)
    {
        while (!state.reader.EndOfData)
        {
            int id = state.reader.GetInt();
            Debug.Log("ID: " + id);
            NetworkIdentity identity = MatchSystem.instance.Scene.NetworkObjects[id];
            identity.ReadData(state.reader, state.initialState);
        }
        
    }

}
