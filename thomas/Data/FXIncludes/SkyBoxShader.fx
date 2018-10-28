#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>

SamplerState StandardWrapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

DepthStencilState DepthWriteAll
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

RasterizerState Rasterizer
{
    FillMode = SOLID;
    CullMode = NONE;
    FrontCounterClockWise = TRUE;//FALSE
  //  DepthClipEnable = FALSE;
};

BlendState AlphaBlendstate
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

struct v2f
{
    float4 vertex : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

v2f vert(appdata_thomas v)
{
    v2f output = (v2f) 0;

    output.vertex = ThomasObjectToClipPos(v.vertex).xyww;
    
    output.texcoord = v.texcoord;
    return output;
}


float4 frag(v2f input) : SV_Target
{
    //return SkyMap.Sample(ObjSamplerState, input.texCoord);
    return float4(1.0f,0.0f,0.0,1.0f);

}

technique11 Skybox
{
    pass SkyPass
    {
        VERT(vert());
        SetGeometryShader(NULL);
		FRAG(frag());
        SetDepthStencilState(DepthWriteAll, 0);
        SetRasterizerState(Rasterizer);
        //SetBlendState(AlphaBlendstate, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
    
}