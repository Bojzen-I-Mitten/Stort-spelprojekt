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


[numthreads(256, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
	float index = (Gid.x * 256) + GTid.x;
    float dt = thomas_DeltaTime;
    

    //load into registers
    float particleSpeed = particlesRead[index].speed;
	float particleTimeLeft = particlesRead[index].lifeTimeLeft;
    float particleRotation = particlesRead[index].rotation;

        
    particlesWrite[index].speed = particleSpeed;
        
	particlesWrite[index].size = particlesRead[index].size;
        
        
        
    float lerpValue = 1 - (particleTimeLeft / particlesRead[index].lifeTime);

	//lerp between start and end speed
    float speed = lerp(particleSpeed, particlesRead[index].endSpeed, lerpValue);

	//update position
    float3 particlePosWS = particlesRead[index].position + particlesRead[index].direction * speed * dt + float3(0, 1, 0) * particlesRead[index].gravity * dt;
	particlesWrite[index].position = particlePosWS;


	float scale = 0.0f;

    if (particleTimeLeft < 0.0f)
	{
		scale = 0.0f;
		//billboards[index].colorFactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
        particlesWrite[index].lifeTimeLeft = particleTimeLeft;
    }
	else
	{
		scale = lerp(particlesRead[index].size, particlesRead[index].endSize, lerpValue);
		//billboards[index].colorFactor = lerp(particlesRead[index].startColor, particlesRead[index].endColor, lerpValue);
        particlesWrite[index].lifeTimeLeft = particleTimeLeft - dt;
            
	}
    //BILLBOARD
        
    float4x4 viewMatrix = thomas_MatrixV;
        
    float3 right = float3(viewMatrix[0][0], viewMatrix[0][1], viewMatrix[0][2]) * scale; //cameraRight * scale;
    float3 up = float3(viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2]) * scale; //-cameraUp * scale;
		
    particlesWrite[index].rotation = particleRotation + particlesRead[index].rotationSpeed * dt;

	float sinangle = sin(particleRotation);
	float cosangle = cos(particleRotation);

	//rotate billboard
	float3 temp = cosangle * right - sinangle * up;
	right = sinangle * right + cosangle * up;
	up = temp;

    //tri 1
	billboards[index].quad[0][0] = particlePosWS + up + right;
	billboards[index].quad[0][1] = particlePosWS + up - right;
	billboards[index].quad[0][2] = particlePosWS - up + right;
    billboards[index].uvs[0][0] = float2(1, 1);
    billboards[index].uvs[0][1] = float2(1, 0);
    billboards[index].uvs[0][2] = float2(0, 1);
    //tri 2
	billboards[index].quad[1][0] = particlePosWS - up + right;
	billboards[index].quad[1][1] = particlePosWS + up - right;
	billboards[index].quad[1][2] = particlePosWS - up - right;
    billboards[index].uvs[1][0] = float2(0, 1);
    billboards[index].uvs[1][1] = float2(1, 0);
    billboards[index].uvs[1][2] = float2(0, 0);

}
