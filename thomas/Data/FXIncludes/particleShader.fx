#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ParticleHeader.h>

Texture2DArray textures;
//Texture2D diffuseTexture;
//SamplerState diffuseSampler : register(s0);

SamplerState StandardWrapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

RasterizerState TestRasterizer
{
    FillMode = SOLID;
    CullMode = BACK;
    FrontCounterClockWise = TRUE;
    DepthClipEnable = FALSE;
};


BlendState AlphaBlendingOn
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;

};

StructuredBuffer<BillboardStruct> billboards;

struct v2f
{
    float4 vertex : SV_POSITION;
    float3 texcoord : TEXCOORD0;
};

v2f vert(uint id : SV_VertexID)
{
    v2f output = (v2f) 0;

    uint particleIndex = (uint) (id / 6);
    uint triangleIndex = (uint) ((id % 6) / 3);
    uint vertexIndex = id % 3;

    output.vertex = mul(thomas_MatrixVP, float4(billboards[particleIndex].quad[triangleIndex][vertexIndex], 1.0f));
    //output.vertex /= output.vertex.w; 
   
    output.texcoord = float3(billboards[particleIndex].uvs[triangleIndex][vertexIndex], billboards[particleIndex].texIndex);

    return output;
}


float4 frag(v2f input) : SV_Target
{
    float4 outputColor = textures.Sample(StandardWrapSampler, input.texcoord);
    
    return outputColor; //float4(0.0f,1.0f,1.0,1.0f);

}

technique11 Particles
{
    pass P0
    {
        VERT(vert());
        SetGeometryShader(NULL);
		FRAG(frag());
        SetDepthStencilState(EnableDepth, 0);
        SetRasterizerState(TestRasterizer);
        SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}