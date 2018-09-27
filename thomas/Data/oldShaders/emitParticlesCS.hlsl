struct InitParticlesBuffer
{
	float3 initPosition;
	float initSpread;
	
	uint nrOfParticlesToEmit;
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

struct CounterStruct
{
    uint deadCount;
    uint aliveCount;
};


StructuredBuffer<InitParticlesBuffer> initParticles;
RWStructuredBuffer<ParticleStruct> particlesWrite;
RWStructuredBuffer<ParticleStruct> particlesWrite2;
ConsumeStructuredBuffer<uint> deadList;
AppendStructuredBuffer<uint> aliveList;


uint rand_xorshift(uint rng_state)
{
	rng_state ^= (rng_state << 13);
	rng_state ^= (rng_state >> 17);
	rng_state ^= (rng_state << 5);
	return rng_state;
}


[numthreads(32, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    int x = (Gid.x * 32) + GTid.x;
    int y = Gid.y;

    InitParticlesBuffer newParticle = initParticles[y];

    if (x < newParticle.nrOfParticlesToEmit)
    {
        uint rng_state = x * newParticle.rand; //seed

        uint w1 = rand_xorshift(rng_state);
        uint w2 = rand_xorshift(w1);
        uint w3 = rand_xorshift(w2);
        uint w4 = rand_xorshift(w3);
        uint w5 = rand_xorshift(w4);
	
        float randClamp = (1.0f / 4294967296.0f);

    
        float speed = max((w2 * randClamp * (newParticle.initMaxSpeed - newParticle.initMinSize)), 0.0f) + newParticle.initMinSize;
        
        float size = (w4 * randClamp * (newParticle.initMaxSize - newParticle.initMinSize)) + newParticle.initMinSize;
        
        float lifeTime = (w5 * randClamp * (newParticle.initMaxLifeTime - newParticle.initMinLifeTime)) + newParticle.initMinLifeTime;

        float x = (w3 * randClamp);
        float y = (w4 * randClamp);
        float z = (w5 * randClamp);

        float3 rng = float3(x, y, z);
        normalize(rng);

        //spread
        float theta = x * 3.14159265359 * 2.0f;
        float phi = y * ((newParticle.initSpread - 1) % 3.14159265359f);
        float xAngle = sin(phi) * cos(theta);
        float yAngle = sin(phi) * sin(theta);
        float zAngle = cos(phi);
        float3 randDir = float3(xAngle, yAngle, zAngle);
        normalize(randDir);

        float3 dir = mul(randDir, (float3x3) newParticle.directionMatrix);
        normalize(dir);

        float3 position = newParticle.initPosition + dir * newParticle.radius;
        if (newParticle.spawnAtSphereEdge)
        {
            dir *= -1; //make the particles go inward;
        }

        float gravity = newParticle.initGravity;
        float endSpeed = newParticle.initEndSpeed;
        float endSize = newParticle.initEndSize;
        float rotationSpeed = newParticle.initRotationSpeed;
        float rotation = newParticle.initRotation;

        ParticleStruct fillBuffer;

        fillBuffer.position = position;
        fillBuffer.gravity = gravity;
        fillBuffer.direction = dir;
        fillBuffer.speed = speed;
        fillBuffer.endSpeed = endSpeed;
        fillBuffer.size = size;
        fillBuffer.endSize = endSize;
        fillBuffer.lifeTimeLeft = lifeTime;
        fillBuffer.lifeTime = lifeTime;
        fillBuffer.rotationSpeed = rotationSpeed;
        fillBuffer.rotation = rotation;

        
        uint writeindex = deadList.Consume();
        aliveList.Append(writeindex);
        

        particlesWrite[writeindex] = fillBuffer;
        particlesWrite2[writeindex] = fillBuffer;
        
        

        

        /*
        particlesWrite2[writeindex].position = position;
        particlesWrite2[writeindex].gravity = gravity;
        particlesWrite2[writeindex].direction = dir;
        particlesWrite2[writeindex].speed = speed;
        particlesWrite2[writeindex].endSpeed = endSpeed;
        particlesWrite2[writeindex].size = size;
        particlesWrite2[writeindex].endSize = endSize;
        particlesWrite2[writeindex].lifeTimeLeft = lifeTime;
        particlesWrite2[writeindex].lifeTime = lifeTime;
        particlesWrite2[writeindex].rotationSpeed = rotationSpeed;
        particlesWrite2[writeindex].rotation = rotation;*/
    }
}
