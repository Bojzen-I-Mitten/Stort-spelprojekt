#ifndef PARTICLE_DATA_HEADER
#define PARTICLE_DATA_HEADER

#define UINT_MAX_AS_FLOAT 4294967296.0f
//#define INVERS_UINT_MAX_AS_FLOAT 2.32830644e-10
#define UPDATE_THREAD_DIM_X 256

uint RandMarsaglia(uint rng_state)
{
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

float RandClamp(uint s)//returns a value between 0 and 1
{
    return s / UINT_MAX_AS_FLOAT;
}


struct ParticleStruct
{
    float3 position;
    float gravity;

    float3 direction;
    float speed;

    float endSpeed;
    float size;
    float endSize;
    float lifeTime;

    float lifeTimeLeft;
    float rotationSpeed;
    float rotation;
    float pad;
};

struct BillboardStruct
{
    float3 quad[2][3];
    float2 pad2;
    float2 uvs[2][3];
};

struct ParticleCounters
{
    uint deadCount;
    uint aliveCount;
    uint realEmitCount;
    uint aliveCountAfterSimulation;
};

#endif // PARTICLE_DATA_HEADER
