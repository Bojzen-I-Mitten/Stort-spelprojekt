#pragma warning(disable: 4717) // removes effect deprecation warning.
#include <ParticleHeader.h>


struct InitParticlesBuffer
{
    float3 position;
    float spread;

    float radius;
    float maxSpeed;
    float minSpeed;
    float endSpeed;

    float maxSize;
    float minSize;
    float endSize;
    float maxLifeTime;

    float minLifeTime;
    float gravity;
    float minRotationSpeed;
    float maxRotationSpeed;

    float3x3 directionMatrix;
    float endRotationSpeed;
    float pad2;
    float pad3;

    uint nrOfParticlesToEmit;
	uint spawnAtSphereEdge;
	uint rand;
	uint pad4;
};


StructuredBuffer<InitParticlesBuffer> initparticles;

RWStructuredBuffer<ParticleStruct> particles;

ConsumeStructuredBuffer<uint> deadlist;
AppendStructuredBuffer<uint> alivelist;

RWByteAddressBuffer counterbuffer;

[numthreads(EMIT_THREAD_DIM_X, 1, 1)]
void CSmain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    int ix = (Gid.x * EMIT_THREAD_DIM_X) + GTid.x;
    int iy = Gid.y;

    InitParticlesBuffer newParticle = initparticles[iy];

    uint2 aliveAndMaxCount = counterbuffer.Load2(ALIVE_COUNT_OFFSET);
    uint aliveCount = aliveAndMaxCount.x;
    uint maxCount = aliveAndMaxCount.y;
    uint capacityLeft = maxCount - aliveCount;

    int threshold = min(newParticle.nrOfParticlesToEmit, max(capacityLeft - EMIT_THREAD_DIM_X * iy, 0)); //Presume that emitter with lower iy want to emit an ammount of EMIT_THREAD_DIM_X particles

    if (ix < threshold)
    {
        uint rng_state = (ix + 1) * newParticle.rand; //seed

        uint w1 = RandMarsaglia(rng_state);
        uint w2 = RandMarsaglia(w1);
        uint w3 = RandMarsaglia(w2);
        uint w4 = RandMarsaglia(w3);
        uint w5 = RandMarsaglia(w4);
        uint w6 = RandMarsaglia(w5);
	
    
        float speed =           (RandClamp(w1) * (newParticle.maxSpeed - newParticle.minSpeed)) + newParticle.minSpeed;
        float size =            (RandClamp(w2) * (newParticle.maxSize - newParticle.minSize)) + newParticle.minSize;
        float lifeTime =        (RandClamp(w3) * (newParticle.maxLifeTime - newParticle.minLifeTime)) + newParticle.minLifeTime;
        float rotationSpeed =   (RandClamp(w4) * (newParticle.maxRotationSpeed - newParticle.minRotationSpeed)) + newParticle.minRotationSpeed;
        
       /* float3 defaultUP = float3(0.0f, 1.0f, 0.0f);
        float3 defaultForward = float3(0.0f, 0.0f, 1.0f);
        float3 defaultRight = float3(1.0f, 0.0f, 0.0f);

        float3 test = normalize(float3(1.0f, 1.0f, 1.0f));

        
        
        float phi = RandClamp(w5) * 3.14159265359f * 2.0f;
        float theta = RandClamp(w6) * 3.14159265359f;
        float xAngle = sin(theta) * cos(phi);
        float yAngle = sin(theta) * sin(phi);
        float zAngle = cos(theta);
*/


        //spread
        float phi = RandClamp(w5) * 3.14159265359f * 2.0f;
        float theta = RandClamp(w6) * 3.14159265359f;
        float xAngle = sin(theta) * cos(phi);
        float yAngle = sin(theta) * sin(phi);
        float zAngle = cos(theta);

        /*theta = atan(test.y / test.x);
        phi = atan(sqrt(test.x * test.x + test.y * test.y * test.z * test.z) / test.z);

        xAngle = sin(theta) * cos(phi);
        yAngle = sin(theta) * sin(phi);
        zAngle = cos(theta);
        */

        float3 randDir = float3(xAngle, yAngle, zAngle);
        normalize(randDir);

        float3 dir = randDir; //mul(randDir, (float3x3) newParticle.directionMatrix);
        normalize(dir);

        float3 position = newParticle.position; // + dir * newParticle.radius;

        if ((bool) newParticle.spawnAtSphereEdge)
        {
            dir *= -1; //make the particles go inward;
        }

        float gravity = newParticle.gravity;
        float endSpeed = newParticle.endSpeed;
        float endSize = newParticle.endSize;
        float rotation = 0;
        
        
        ParticleStruct fillBuffer = (ParticleStruct) 0;

        fillBuffer.position = position;
        fillBuffer.rotation = rotation;
        fillBuffer.gravity = gravity;
        fillBuffer.direction = dir;
        fillBuffer.speed = speed;
        fillBuffer.endSpeed = endSpeed;
        fillBuffer.size = size;
        fillBuffer.endSize = endSize;
        fillBuffer.lifeTimeLeft = lifeTime;
        fillBuffer.lifeTime = lifeTime;
        fillBuffer.rotationSpeed = rotationSpeed;
        
        uint writeindex = deadlist.Consume();
        alivelist.Append(writeindex);
        particles[writeindex] = fillBuffer;
    }
    
}

technique11 EmitCS
{
    pass P0
    {

        SetComputeShader(CompileShader(cs_5_0, CSmain()));
    }
}