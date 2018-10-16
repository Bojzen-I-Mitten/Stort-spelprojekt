#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ParticleHeader.h>

Texture2DArray texArr;
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
    float2 texcoord : TEXCOORD0;
    float3 color : asdf;
};

v2f vert(uint id : SV_VertexID)
{
    v2f output = (v2f) 0;

    uint particleIndex = (uint) (id / 6);
    uint triangleIndex = (uint) ((id % 6) / 3);
    uint vertexIndex = id % 3;

    output.vertex = mul(thomas_MatrixVP, float4(billboards[particleIndex].quad[triangleIndex][vertexIndex], 1.0f));
    //output.vertex /= output.vertex.w; 
    //output.vertex = float4(billboards[particleIndex].quad[triangleIndex][vertexIndex], 1.0);
	
    output.texcoord = billboards[particleIndex].uvs[triangleIndex][vertexIndex];
    
    /*if (id == 0)
        output.vertex = mul(float4(0.0, 0.5, 0.5, 1.0), thomas_MatrixVP);
    else if (id == 2)
        output.vertex = mul(float4(0.5, -0.5, 0.5, 1.0), thomas_MatrixVP);
    else if (id == 1)
        output.vertex = mul(float4(-0.5, -0.5, 0.5, 1.0), thomas_MatrixVP);*/
    
    //if (particleIndex % 2 == 0)
        output.color = float3(1.0f, 0.0f, 0.0f);
    //else
      //  output.color = float3(0.0f, 1.0f, 0.0f);

    return output;
}


float4 frag(v2f input) : SV_Target
{
    //float4 outputColor = diffuseTexture.Sample(diffuseSampler, input.uvs);

    //outputColor *= input.colorFactor;
    
    return float4(input.color, 1.0f);

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