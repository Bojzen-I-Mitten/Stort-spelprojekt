#pragma warning(disable: 4717) // removes effect deprecation warning.
#include <ParticleDataHeader.hlsl>

RWByteAddressBuffer indirectargs;
ByteAddressBuffer counterbuffer;

[numthreads(1, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    indirectargs.Store(0, ((counterbuffer.Load(4) + UPDATE_THREAD_DIM_X - 1) / UPDATE_THREAD_DIM_X));
    indirectargs.Store(12, counterbuffer.Load(4) * 6);

}


technique11 CalculateEmitCS
{
    pass P0
    {
        SetComputeShader(CompileShader(cs_5_0, CSMain()));
    }
}