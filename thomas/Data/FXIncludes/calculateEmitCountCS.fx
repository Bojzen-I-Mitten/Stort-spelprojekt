#pragma warning(disable: 4717) // removes effect deprecation warning.
#include <ParticleHeader.h>

RWByteAddressBuffer indirectargs;
ByteAddressBuffer counterbuffer;

[numthreads(1, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    uint aliveCount = counterbuffer.Load(ALIVE_COUNT_OFFSET);
    indirectargs.Store(UPDATE_DISPATCH_INDIRECT_OFFSET, ((aliveCount + UPDATE_THREAD_DIM_X - 1) / UPDATE_THREAD_DIM_X));
    indirectargs.Store(DRAW_INDIRECT_OFFSET, aliveCount * 6);
}


technique11 CalculateEmitCS
{
    pass P0
    {
        SetComputeShader(CompileShader(cs_5_0, CSMain()));
    }
}