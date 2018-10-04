#pragma warning(disable: 4717) // removes effect deprecation warning.
#include <ParticleDataHeader.hlsl>

RWByteAddressBuffer indirectargsforupdate;
ByteAddressBuffer counterbuffer;

[numthreads(1, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    indirectargsforupdate.Store3(0, uint3((counterbuffer.Load(4) - 1) / UPDATE_THREAD_DIM_X + 1, 1, 1));
}


technique11 CalculateEmitCS
{
    pass P0
    {
        SetComputeShader(CompileShader(cs_5_0, CSMain()));
    }
}