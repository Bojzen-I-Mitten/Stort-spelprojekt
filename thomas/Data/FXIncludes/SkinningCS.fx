#pragma warning(disable: 4717) // removes effect deprecation warning.


#include <ThomasCG.hlsl>

cbuffer constants
{
    uint maxNrOfVerts;
};

RWByteAddressBuffer vertexData; //<float3>
RWByteAddressBuffer normalData; //<float3>
RWByteAddressBuffer boneWieghtData; //<float4>
RWByteAddressBuffer boneIndexData; //<float>

[numthreads(256, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    uint Tid = (Gid.x * 256) + GTid.x;

    if (Tid < maxNrOfVerts)
    {
        appdata_thomas_skin data; // = skinData[Tid];
        appdata_thomas output;

        float4 posL = float4(data.vertex, 1.f);
        float3 normalL = data.normal;
        float3 tangentL = data.tangent;
        
        ThomasSkinVertex(posL, normalL, data.boneWeight, data.boneIndex);


        float bisign = BiTangentSign(data.normal, data.tangent, data.bitangent);
        float3 bitangL = cross(normalL, tangentL) * bisign;
	
        output.vertex = posL;
        //output.worldPos = ThomasObjectToWorldPos(posL);

        output.tangent = tangentL;
        output.bitangent = bitangL;
        output.normal = normalL;
        
        output.texcoord = data.texcoord;
       
      //  vertexData[Tid] = output;
    }
}


technique11 TheSkin
{
    pass P0
    {

        SetComputeShader(CompileShader(cs_5_0, CSMain()));
    }
}