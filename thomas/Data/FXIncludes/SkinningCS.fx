#pragma warning(disable: 4717) // removes effect deprecation warning.


#include <ThomasCG.hlsl>

cbuffer constants
{
    uint maxNrOfVerts;
};

ByteAddressBuffer vertexOrigData; //<float4>
ByteAddressBuffer normalOrigData; //<float3>
ByteAddressBuffer boneWeightOrigData; //<float4>
ByteAddressBuffer boneIndexOrigData; //<float>

RWByteAddressBuffer vertexData; //<float4>
RWByteAddressBuffer normalData; //<float3>
RWByteAddressBuffer boneWeightData; //<float4>
RWByteAddressBuffer boneIndexData; //<float>

[numthreads(256, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    uint Tid = (Gid.x * 256) + GTid.x;
    

    if (Tid < maxNrOfVerts)
    {
        uint vertexTid = Tid * 16;
        uint normalTid = Tid * 12;
        uint boneWeightTid = Tid * 16;
        uint boneIndexTid = Tid * 4;

        float4 posL = vertexOrigData.Load4(vertexTid);
        float3 normalL = normalOrigData.Load3(normalTid);
        float4 boneWeight = boneWeightOrigData.Load4(boneWeightTid);
        float boneIndex = boneIndexOrigData.Load(boneIndexTid);
        
        //ThomasSkinVertex(posL, normalL, boneWeight, boneIndex);

        vertexData.Store4(vertexTid, posL); // + float4(Tid * 100.0, 0.0, 0.0, 0.0));
        normalData.Store3(normalTid, normalL);
        boneWeightData.Store4(boneWeightTid, boneWeight);
        boneIndexData.Store(boneIndexTid, boneIndex);

    }
}


technique11 TheSkin
{
    pass P0
    {

        SetComputeShader(CompileShader(cs_5_0, CSMain()));
    }
}