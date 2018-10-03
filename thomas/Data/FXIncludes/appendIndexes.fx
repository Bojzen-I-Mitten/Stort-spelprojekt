#pragma warning(disable: 4717) // removes effect deprecation warning.

AppendStructuredBuffer<uint> deadlist;

cbuffer maxParticlesCbuffer
{
    int maxNrOfParticles;
};

[numthreads(512, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    int index = (Gid.x * 512) + GTid.x;
    if (index < maxNrOfParticles)
    deadlist.Append(index);
}


technique11 shittyCS
{
    pass P0
    {
        SetComputeShader(CompileShader(cs_5_0, CSMain()));
    }
}