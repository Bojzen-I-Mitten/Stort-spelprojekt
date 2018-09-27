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

	//float4 startColor;

	//float4 endColor;

};

struct BillboardStruct
{
	float3 quad[2][3];
	float2 pad2;
	float2 uvs[2][3];
//	float4 colorFactor;
};

StructuredBuffer<ParticleStruct> particlesRead;
RWStructuredBuffer<ParticleStruct> particlesWrite;
RWStructuredBuffer<BillboardStruct> billboards;

AppendStructuredBuffer<uint> deadList;
ConsumeStructuredBuffer<uint> aliveList;


[numthreads(256, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    uint Tid = (Gid.x * 256) + GTid.x;
    uint index = aliveList.Consume(); 
    float dt = thomas_DeltaTime;
    
    ParticleStruct readParticle = particlesRead[index];
    ParticleStruct writeParticle;
    
    
	writeParticle.size = readParticle.size;
        
        
        
    float lerpValue = 1 - (readParticle.lifeTimeLeft / readParticle.lifeTime);

	//lerp between start and end speed
    float speed = lerp(readParticle.speed, readParticle.endSpeed, lerpValue);

	//update position
    float3 particlePosWS = readParticle.position + readParticle.direction * speed * dt + float3(0, 1, 0) * readParticle.gravity * dt;
	writeParticle.position = particlePosWS;


	float scale = 0.0f;

    if (readParticle.lifeTimeLeft < 0.0f)
	{
		scale = 0.0f;
		//billboards[index].colorFactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
        writeParticle.lifeTimeLeft = readParticle.lifeTimeLeft;
        deadList.Append(index);
    }
	else
	{
		scale = lerp(readParticle.size, readParticle.endSize, lerpValue);
		//billboards[index].colorFactor = lerp(readParticle.startColor, readParticle.endColor, lerpValue);
        writeParticle.lifeTimeLeft = readParticle.lifeTimeLeft - dt;
            
	}
    //BILLBOARD
        
    float3x3 viewMatrix = thomas_MatrixV;
        
    float3 right = float3(viewMatrix[0][0], viewMatrix[0][1], viewMatrix[0][2]) * scale; //cameraRight * scale;
    float3 up = float3(viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2]) * scale; //-cameraUp * scale;
		
    writeParticle.rotation = readParticle.rotation + readParticle.rotationSpeed * dt;

    float sinangle = sin(readParticle.rotation);
    float cosangle = cos(readParticle.rotation);

	//rotate billboard
	float3 temp = cosangle * right - sinangle * up;
	right = sinangle * right + cosangle * up;
	up = temp;



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
