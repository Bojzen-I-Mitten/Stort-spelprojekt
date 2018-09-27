#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <..\FXIncludes\ThomasCG.hlsl>

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

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uvs : TEXCOORD0;
    float2 paddddd : padding;
    float4 colorFactor : COLOR0;
};

struct BillboardStruct
{
    float3 quad[2][3];
    float2 pad2;
    float2 uvs[2][3];
    float4 colorFactor;
};

StructuredBuffer<BillboardStruct> billboards;


struct v2f
{
    float4 vertex : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

v2f vert(uint id : SV_VERTEXID)
{
    v2f o = (v2f) 0;

    uint particleIndex = (uint) (id / 6);
    uint triangleIndex = (uint) ((id % 6) / 3);
    uint vertexIndex = id % 3;

    o.vertex = mul(float4(billboards[particleIndex].quad[triangleIndex][vertexIndex], 1.0), thomas_MatrixVP);
	
    o.texcoord = billboards[particleIndex].uvs[triangleIndex][vertexIndex];
    //output.colorFactor = billboards[particleIndex].colorFactor;

    return o;
}


float4 frag(v2f input) : SV_Target
{
    //float4 outputColor = diffuseTexture.Sample(diffuseSampler, input.uvs);

    //outputColor *= input.colorFactor;
    
    return float4(0.5f, 0.0f, 0.5f, 1.0f);

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