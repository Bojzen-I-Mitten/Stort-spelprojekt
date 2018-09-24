struct InitParticlesBuffer
{
	float3 initPosition;
	float initSpread;
	
	uint particleBlockIndex;
	float initMaxSpeed;
	float radius;
	bool spawnAtSphereEdge;

	float initMinSpeed;
	float initEndSpeed;

	float initMaxSize;
	float initMinSize;
	float initEndSize;
	float initMaxLifeTime;

	float initMinLifeTime;
	float rand;
	float initRotationSpeed;
	float initRotation;

	matrix directionMatrix;

	float initGravity;
	float3 padding;

};

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
    
};

StructuredBuffer<InitParticlesBuffer> initParticles;
RWStructuredBuffer<ParticleStruct> particlesWrite;
RWStructuredBuffer<ParticleStruct> particlesWrite2;


uint rand_xorshift(uint rng_state)
{
	rng_state ^= (rng_state << 13);
	rng_state ^= (rng_state >> 17);
	rng_state ^= (rng_state << 5);
	return rng_state;
}


[numthreads(1, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
	float index = particleBlockIndex + Gid.x;
	uint rng_state = index * rand;

	uint w1 = rand_xorshift(rng_state);
	uint w2 = rand_xorshift(w1);
	uint w3 = rand_xorshift(w2);
	uint w4 = rand_xorshift(w3);
	uint w5 = rand_xorshift(w4);
	
	float randClamp = (1.0f / 4294967296.0f);
    
	float speed = max((w2 * randClamp * (initMaxSpeed - initMinSpeed)), 0.0f) + initMinSpeed;

	float size = (w4 * randClamp * (initMaxSize - initMinSize)) + initMinSize;
	float lifeTime = (w5 * randClamp * (initMaxLifeTime - initMinLifeTime)) + initMinLifeTime;

	float x = (w3 * randClamp);
	float y = (w4 * randClamp);
	float z = (w5 * randClamp);

	float3 rng = float3(x, y, z);
	normalize(rng);

    //spread
	float theta = x * 3.14159265359 * 2.0f;
	float phi = y * ((initSpread-1) % 3.14159265359f);
	float xAngle = sin(phi) * cos(theta);
	float yAngle = sin(phi) * sin(theta);
	float zAngle = cos(phi);
	float3 randDir = float3(xAngle, yAngle, zAngle);
	normalize(randDir);

	float3 dir = mul(randDir, (float3x3) directionMatrix);
	normalize(dir);

	float3 position = initPosition + dir * radius;
	if (spawnAtSphereEdge)
	{
		dir *= -1; //make the particles go inward;
	}

	particlesWrite[index].position = position;
	particlesWrite[index].gravity = initGravity;
	particlesWrite[index].direction = dir;
	particlesWrite[index].speed = speed;
	particlesWrite[index].endSpeed = initEndSpeed;
	particlesWrite[index].size = size;
	particlesWrite[index].endSize = initEndSize;
	particlesWrite[index].lifeTimeLeft = lifeTime;
	particlesWrite[index].lifeTime = lifeTime;
	particlesWrite[index].rotationSpeed = initRotationSpeed;
	particlesWrite[index].rotation = initRotation;


	particlesWrite2[index].position = position;
	particlesWrite2[index].gravity = initGravity;
	particlesWrite2[index].direction = dir;
	particlesWrite2[index].speed = speed;
	particlesWrite2[index].endSpeed = initEndSpeed;
	particlesWrite2[index].size = size;
	particlesWrite2[index].endSize = initEndSize;
	particlesWrite2[index].lifeTimeLeft = lifeTime;
	particlesWrite2[index].lifeTime = lifeTime;
	particlesWrite2[index].rotationSpeed = initRotationSpeed;
	particlesWrite2[index].rotation = initRotation;
}
