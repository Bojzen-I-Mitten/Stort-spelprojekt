#pragma warning(disable: 4717) // removes effect deprecation warning.
#include <ParticleHeader.h>
#include <ThomasShaderVariables.hlsl>


RWStructuredBuffer<ParticleStruct> particles;
RWStructuredBuffer<BillboardStruct> billboards;

AppendStructuredBuffer<uint> deadlist;
AppendStructuredBuffer<uint> appendalivelist;
ConsumeStructuredBuffer<uint> consumealivelist;

RWByteAddressBuffer counterbuffer;

[numthreads(UPDATE_THREAD_DIM_X, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    uint Tid = (Gid.x * UPDATE_THREAD_DIM_X) + GTid.x;
    uint nrOfAliveParticles = counterbuffer.Load(ALIVE_COUNT_OFFSET);

    if (Tid < nrOfAliveParticles)
    {
    
        uint index = consumealivelist.Consume();
        float3x3 viewMatrix = thomas_MatrixV;
        float dt = thomas_DeltaTime;

    
        ParticleStruct particle = particles.Load(index);
    
       
        float lerpValue = 1 - (particle.lifeTimeLeft / particle.lifeTime);
        particle.lifeTimeLeft = particle.lifeTimeLeft - dt;

	    //lerp between start and end speed
        float speed = lerp(particle.speed, particle.endSpeed, lerpValue);

	    //update position
        float3 particlePosWS = particle.position + particle.direction * speed * dt - float3(0, 1, 0) * particle.gravity * dt;
        particle.position = particlePosWS;

    
        float size = lerp(particle.size, particle.endSize, lerpValue);

        
        
    
        if (particle.lifeTimeLeft < 0.0f)
        {
            size = 0.0f;

            deadlist.Append(index);
        }
        else
        {
            appendalivelist.Append(index);
        }
        
        //BILLBOARD

        float3 right = float3(viewMatrix[0].xyz) * size;
        float3 up = float3(viewMatrix[1].xyz) * size;
    
        particle.rotation = particle.rotation + particle.rotationSpeed * dt;
    
        float sinangle = sin(particle.rotation);
        float cosangle = cos(particle.rotation);

	    //rotate billboard
        float3 temp = cosangle * right + sinangle * up;
        right = sinangle * right - cosangle * up;
        up = temp;

    
        particles[index] = particle;

        BillboardStruct billboard = (BillboardStruct) 0;

    //tri 1
        billboard.quad[0][0] = particlePosWS + up + right;
        billboard.quad[0][1] = particlePosWS + up - right;
        billboard.quad[0][2] = particlePosWS - up + right;
        billboard.uvs[0][0] = float2(1, 1);
        billboard.uvs[0][1] = float2(1, 0);
        billboard.uvs[0][2] = float2(0, 1);
    //tri 2
        billboard.quad[1][0] = particlePosWS - up + right;
        billboard.quad[1][1] = particlePosWS + up - right;
        billboard.quad[1][2] = particlePosWS - up - right;
        billboard.uvs[1][0] = float2(0, 1);
        billboard.uvs[1][1] = float2(1, 0);
        billboard.uvs[1][2] = float2(0, 0);

        billboard.textureIndex = particle.textureIndex;
    
        billboards[Tid] = billboard;
    }
}


technique11 UpdateCS
{
    pass P0
    {

        SetComputeShader(CompileShader(cs_5_0, CSMain()));
    }
}