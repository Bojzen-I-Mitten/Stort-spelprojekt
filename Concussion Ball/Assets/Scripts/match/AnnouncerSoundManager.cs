//#define T_FOR_RESET

using System;
using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using System.ComponentModel;
using System.Collections;

public enum ANNOUNCEMENT_TYPE
{
    EXPLODED,
    TACKLED,
    GOAL,
    GOLDENGOAL,
    HAT,
    WELLPLAYED,
    WELCOME,
    PICKUPBALL,
    RANDOM
}



[DisallowMultipleComponent]
public class AnnouncerSoundManager : ScriptComponent
{

    private List<AudioClip> ExplodedSounds;
    private List<AudioClip> TackledSounds;
    private List<AudioClip> ChadRandomSounds;
    private List<AudioClip> GoalSounds;
    private List<AudioClip> GoldenGoalSounds;
    private List<AudioClip> HatSounds;
    private List<AudioClip> WellPlayedSounds;
    private List<AudioClip> WelcomeSounds;
    private List<AudioClip> PickUpBallSounds;

    private SoundComponent MasterSound;
    private System.Random rng;
    bool playing = false;

    private float randomClipCooldown;

    public static AnnouncerSoundManager Instance;

  

    public override void OnEnable()
    {
        if (Instance == null)
            Instance = this;
        else
            Debug.LogError("Multiple announcer components exist!");
    }

    public override void OnDisable()
    {
        Instance = null;
    }

    public override void Start()
    {
        rng = new System.Random();
        MasterSound = gameObject.AddComponent<SoundComponent>();
        MasterSound.Clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/Sounds/Carl/Random1.wav");
        MasterSound.Looping = false;
        MasterSound.Type = SoundComponent.SoundType.Voice;
        MasterSound.Is3D = false;
        randomClipCooldown = 15.0f;

        ExplodedSounds = new List<AudioClip>();
        TackledSounds = new List<AudioClip>();
        GoalSounds = new List<AudioClip>();
        GoldenGoalSounds = new List<AudioClip>();
        HatSounds = new List<AudioClip>();
        ChadRandomSounds = new List<AudioClip>();
        WellPlayedSounds = new List<AudioClip>();
        WelcomeSounds = new List<AudioClip>();
        PickUpBallSounds = new List<AudioClip>();

        //  EXPLODED    EXPLODED    EXPLODED    EXPLODED    EXPLODED
        for (int i = 1; i < 16; ++i)
        {
            String path = "Sounds/Carl/Exploded" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                ExplodedSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }
        ExplodedSounds.Shuffle();


        //TACKLED   TACKLED     TACKLED     TACKLED     TACKLED     TACKLED
        for (int i = 1; i < 15; ++i)
        {
            String path = "Sounds/Carl/Tackled" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                TackledSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }
        TackledSounds.Shuffle();


        //GOAL      GOAL        GOAL        GOAL        GOAL        GOAL
        for (int i = 1; i < 4; ++i)
        {
            String path = "Sounds/Carl/Goal" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                GoalSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }


        //GOLDENGOAL        GOLDENGOAL      GOLDENGOAL      GOLDENGOAL      GOLDENGOAL
        for (int i = 1; i < 5; ++i)
        {
            String path = "Sounds/Carl/GoldenGoal" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                GoldenGoalSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }
        GoldenGoalSounds.Shuffle();


        //HAT       HAT       HAT       HAT       HAT         HAT       HAT       HAT
        for (int i = 1; i < 9; ++i)
        {
            String path = "Sounds/Carl/Hat" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                HatSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }
        HatSounds.Shuffle();


        //WELLPLAYED        WELLPLAYED      WELLPLAYED      WELLPLAYED      WELLPLAYED
        for (int i = 1; i < 4; ++i)
        {
            String path = "Sounds/Carl/WellPlayed" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                WellPlayedSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }
        WellPlayedSounds.Shuffle();


        //WELCOME       WELCOME     WELCOME     WELCOME     WELCOME     WELCOME
        for (int i = 1; i < 7; ++i)
        {
            String path = "Sounds/Carl/Welcome" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                WelcomeSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }
        WelcomeSounds.Shuffle();


        //PICKUPBALL        PICKUPBALL      PICKUPBALL      PICKUPBALL      PICKUPBALL
        for (int i = 1; i < 4; ++i)
        {
            String path = "Sounds/Carl/PickUpBall" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                PickUpBallSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }
        PickUpBallSounds.Shuffle();


        //RANDOM        RANDOM      RANDOM      RANDOM      RANDOM      RANDOM      RANDOM
        for (int i = 1; i < 6; ++i)
        {
            String path = "Sounds/Carl/Random" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                ChadRandomSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }

        for (int i = 1; i < 6; ++i)
        {
            String path = "Sounds/Carl/Chads" + i.ToString() + ".wav";
            AudioClip clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/" + path);
            if (clip != null)
                ChadRandomSounds.Add(clip);
            else
                Debug.Log("Faild loading clip at: " + path);
        }
        ChadRandomSounds.Shuffle();
    }

    public override void Update()
    {
        randomClipCooldown -= Time.DeltaTime;
        if (randomClipCooldown < 0.0f)
        {
            Announce(ANNOUNCEMENT_TYPE.RANDOM);
        }
    }

    IEnumerator PlaySoundCoroutine(AudioClip clip, float startWait, float endWait)
    {
        playing = true;
        yield return new WaitForSeconds(startWait);
        MasterSound.Clip = clip;
        MasterSound.Play();
        while (MasterSound.IsPlaying())
            yield return new WaitForSeconds(1.0f);

        yield return new WaitForSeconds(endWait);
        playing = false;
    }

    bool PlaySound(AudioClip clip, float startWait, float endWait)
    {
        if (!playing)
        {
            StartCoroutine(PlaySoundCoroutine(clip, startWait, endWait));
            return true;
        }
        return false;
    }

    public void Announce(ANNOUNCEMENT_TYPE TYPE)
    {
        switch (TYPE)
        {
            case ANNOUNCEMENT_TYPE.EXPLODED:
                if (PlaySound(ExplodedSounds[rng.Next(0, ExplodedSounds.Count)], 1.2f, 3.0f))
                {
                    ExplodedSounds.Shuffle();
                }
                break;
            case ANNOUNCEMENT_TYPE.TACKLED:
                if (PlaySound(TackledSounds[rng.Next(0, TackledSounds.Count)], 0.8f, 1.0f))
                {
                    TackledSounds.Shuffle();
                }
                break;
            case ANNOUNCEMENT_TYPE.GOAL:
                if (PlaySound(GoalSounds[rng.Next(0, GoalSounds.Count)], 2.0f, 2.0f))
                {
                    GoalSounds.Shuffle();
                }
                break;
            case ANNOUNCEMENT_TYPE.GOLDENGOAL:
                if (PlaySound(GoldenGoalSounds[rng.Next(0, GoldenGoalSounds.Count)], 2.0f, 1.0f))
                {
                    GoldenGoalSounds.Shuffle();
                }
                break;
            case ANNOUNCEMENT_TYPE.HAT:
                if (PlaySound(HatSounds[rng.Next(0, HatSounds.Count)], 0.0f, 0.0f))
                {
                    HatSounds.Shuffle();
                }
                break;
            case ANNOUNCEMENT_TYPE.WELLPLAYED:
                if (PlaySound(WellPlayedSounds[rng.Next(0, WellPlayedSounds.Count)], 1.0f, 1.0f))
                {
                    WellPlayedSounds.Shuffle();
                }
                break;
            case ANNOUNCEMENT_TYPE.WELCOME:
                if (PlaySound(WelcomeSounds[rng.Next(0, WelcomeSounds.Count)], 4.0f, 1.0f))
                {
                    WelcomeSounds.Shuffle();
                }
                break;
            case ANNOUNCEMENT_TYPE.PICKUPBALL:
                if (PlaySound(PickUpBallSounds[rng.Next(0, PickUpBallSounds.Count)], 1.3f, 1.0f))
                {
                    PickUpBallSounds.Shuffle();
                }
                break;
            case ANNOUNCEMENT_TYPE.RANDOM:
                if (PlaySound(ChadRandomSounds[rng.Next(0, ChadRandomSounds.Count)], 1.0f, 5.0f))
                {
                    ChadRandomSounds.Shuffle();
                }
                break;
        }
        randomClipCooldown = 10.0f;
    }

    
}

static class MyExtensions
{
    private static System.Random rng = new System.Random();

    public static void Shuffle<T>(this IList<T> list)
    {
        int n = list.Count;
        while (n > 1)
        {
            n--;
            int k = rng.Next(n + 1);
            T value = list[k];
            list[k] = list[n];
            list[n] = value;
        }
    }
}