#pragma warning(disable: 4717) // removes effect deprecation warning.
//File included in particle .fx files
#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER

#define UINT_MAX_AS_FLOAT 4294967296.0f
#define INVERSE_UINT_MAX_AS_FLOAT 2.328306436538696289e-10
#define UPDATE_THREAD_DIM_X 256
#define EMIT_THREAD_DIM_X 128




uint RandMarsaglia(uint rng_state)
{
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

float RandClamp(uint s)//returns a value between 0 and 1
{
	return s * INVERSE_UINT_MAX_AS_FLOAT; // / UINT_MAX_AS_FLOAT;
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

//particle counter byteaddressbuffer
static const uint DEAD_COUNT_OFFSET = 0;
static const uint ALIVE_COUNT_OFFSET = DEAD_COUNT_OFFSET + 4;
static const uint MAX_NR_OF_PARTICLES = ALIVE_COUNT_OFFSET + 4;

//indirect buffers offset
static const uint UPDATE_DISPATCH_INDIRECT_OFFSET = 0;
static const uint DRAW_INDIRECT_OFFSET = UPDATE_DISPATCH_INDIRECT_OFFSET + 12;

/*
struct ParticleCounters
{
    uint deadCount;
    uint aliveCount;
    uint maxNrOfParticles;
    uint aliveCountAfterSimulation;
};
*/
#endif // PARTICLE_HEADER
