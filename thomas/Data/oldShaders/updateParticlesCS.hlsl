#include <..\FXIncludes\ThomasShaderVariables.hlsl>

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

RWStructuredBuffer<ParticleStruct> particles;
RWStructuredBuffer<BillboardStruct> billboards;

AppendStructuredBuffer<uint> deadList;
AppendStructuredBuffer<uint> appendAliveList;
ConsumeStructuredBuffer<uint> consumeAliveList;


[numthreads(256, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    uint Tid = (Gid.x * 256) + GTid.x;
    uint index = consumeAliveList.Consume();
    float3x3 viewMatrix = thomas_MatrixV;
    float dt = thomas_DeltaTime;

    
    ParticleStruct particle = particles[index];
    
        
    float lerpValue = 1 - (particle.lifeTimeLeft / particle.lifeTime);

	//lerp between start and end speed
    float speed = lerp(particle.speed, particle.endSpeed, lerpValue);

	//update position
    float3 particlePosWS = particle.position + particle.direction * speed * dt + float3(0, 1, 0) * particle.gravity * dt;
    particle.position = particlePosWS;


	float scale = 0.0f;

    if (particle.lifeTimeLeft < 0.0f)
	{
		scale = 0.0f;
        particle.lifeTimeLeft = particle.lifeTimeLeft;

        deadList.Append(index);
    }
	else
	{
        scale = lerp(particle.size, particle.endSize, lerpValue);
        particle.lifeTimeLeft = particle.lifeTimeLeft - dt;
        
        appendAliveList.Append(index);

    }
    //BILLBOARD
    float3 right = float3(viewMatrix[0][0], viewMatrix[0][1], viewMatrix[0][2]) * scale; //cameraRight * scale;
    float3 up = float3(viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2]) * scale; //-cameraUp * scale;
		
    particle.rotation = particle.rotation + particle.rotationSpeed * dt;

    float sinangle = sin(particle.rotation);
    float cosangle = cos(particle.rotation);

	//rotate billboard
	float3 temp = cosangle * right - sinangle * up;
	right = sinangle * right + cosangle * up;
	up = temp;

    particles[index] = particle;

    BillboardStruct billboard;

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

    
    billboards[Tid] = billboard;
}
