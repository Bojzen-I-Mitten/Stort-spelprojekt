#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ParticleHeader.h>

Texture2DArray textures;

SamplerState StandardWrapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = LESS;
};

RasterizerState Rasterizer
{
    FillMode = SOLID;
    CullMode = BACK;
    FrontCounterClockWise = TRUE;
    DepthClipEnable = FALSE;
};

BlendState SrcAlphaBlend
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState AdditiveBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;

};

StructuredBuffer<BillboardStruct> billboards;

struct v2f
{
    float4 vertex : SV_POSITION;
    float3 texcoord : TEXCOORD0;
    float fade : FADEVAL;
};

v2f vert(uint id : SV_VertexID)
{
    v2f output = (v2f) 0;

    uint particleIndex = (uint) (id / 6);
    uint triangleIndex = (uint) ((id % 6) / 3);
    uint vertexIndex = id % 3;

    output.vertex = mul(thomas_MatrixVP, float4(billboards[particleIndex].quad[triangleIndex][vertexIndex], 1.0f));
    output.vertex /= output.vertex.w; 
   
    output.texcoord = float3(billboards[particleIndex].uvs[triangleIndex][vertexIndex], billboards[particleIndex].textureIndex);
    output.fade = billboards[particleIndex].fade;

    return output;
}


float4 frag(v2f input) : SV_Target
{
    float4 outputColor = textures.Sample(StandardWrapSampler, input.texcoord);
    
    return float4(outputColor.xyz, outputColor.w - input.fade); //float4(0.0f,1.0f,1.0,1.0f);

}

technique11 Particles
{
    pass P0
    {
        VERT(vert());
        SetGeometryShader(NULL);
		FRAG(frag());
        SetDepthStencilState(EnableDepth, 1);
        SetRasterizerState(Rasterizer);
        SetBlendState(SrcAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
  //  pass AdditiveBlendPass
  //  {
  //      VERT(vert());
  //      SetGeometryShader(NULL);
		//FRAG(frag());
  //      SetDepthStencilState(EnableDepth, 1);
  //      SetRasterizerState(Rasterizer);
  //      SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
  //  }
}