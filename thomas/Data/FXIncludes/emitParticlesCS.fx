#pragma warning(disable: 4717) // removes effect deprecation warning.
#include <ParticleHeader.h>


struct InitParticleBufferStruct
{
    float3 position;
    float distanceFromSphereCenter;

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

    float3 direction;
    float pad;
    
    uint nrOfParticlesToEmit;
    uint spawnAtSphereEdge;
    uint rand;
    uint pad2;

};

cbuffer indexForParticleEmitter : register(b7)
{
    int initindex;
    int3 pad;
};

StructuredBuffer<InitParticleBufferStruct> initparticles;

RWStructuredBuffer<ParticleStruct> particles;

ConsumeStructuredBuffer<uint> deadlist;
AppendStructuredBuffer<uint> alivelist;

RWByteAddressBuffer counterbuffer;

[numthreads(EMIT_THREAD_DIM_X, 1, 1)]
void CSmain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    int ix = (Gid.x * EMIT_THREAD_DIM_X) + GTid.x;

    InitParticleBufferStruct newParticle = initparticles[initindex];

    uint2 aliveAndMaxCount = counterbuffer.Load2(ALIVE_COUNT_OFFSET);
    uint aliveCount = aliveAndMaxCount.x;
    uint maxCount = aliveAndMaxCount.y;
    uint capacityLeft = maxCount - aliveCount;

    int threshold = min(newParticle.nrOfParticlesToEmit, capacityLeft); 

    if (ix < threshold)
    {
        uint rng_state = (ix + 1) * newParticle.rand; //seed

        uint r1 = RandMarsaglia(rng_state);
        uint r2 = RandMarsaglia(r1);
        uint r3 = RandMarsaglia(r2);
        uint r4 = RandMarsaglia(r3);
        uint r5 = RandMarsaglia(r4);
        uint r6 = RandMarsaglia(r5);
	
        float speed =           (RandClamp(r1) * (newParticle.maxSpeed - newParticle.minSpeed)) + newParticle.minSpeed;
        float size =            (RandClamp(r2) * (newParticle.maxSize - newParticle.minSize)) + newParticle.minSize;
        float lifeTime =        (RandClamp(r3) * (newParticle.maxLifeTime - newParticle.minLifeTime)) + newParticle.minLifeTime;
        float rotationSpeed =   (RandClamp(r4) * (newParticle.maxRotationSpeed - newParticle.minRotationSpeed)) + newParticle.minRotationSpeed;
        
        //spread
        float phi = RandClamp(r5) * 3.14159265359f * 2.0f;
        float theta = RandClamp(r6) * 3.14159265359f;
        float xAngle = sin(theta) * cos(phi);
        float yAngle = sin(theta) * sin(phi);
        float zAngle = cos(theta);
        
        float3 randDir = float3(xAngle, yAngle, zAngle);//dir from center to sphere edge
        
        float3 directionToSphereEdge = randDir;
        //direction = normalize(direction);//is this already normalized?

        float3 positionInSphereEdge = newParticle.position + directionToSphereEdge * newParticle.radius;
        float3 position = newParticle.position;
        
        //Adding a distance from the sphere makes a cone shape
        float3 direction = positionInSphereEdge + newParticle.direction * newParticle.distanceFromSphereCenter - position;
        direction = normalize(direction);

        position += direction * newParticle.radius * newParticle.spawnAtSphereEdge;
        

        float gravity = newParticle.gravity;
        float endSpeed = newParticle.endSpeed;
        float endSize = newParticle.endSize;
        float rotation = phi * rotationSpeed; //0;
        
        
        ParticleStruct fillBuffer = (ParticleStruct) 0;

        fillBuffer.position = position;
        fillBuffer.rotation = rotation;
        fillBuffer.gravity = gravity;
        fillBuffer.direction = direction;
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