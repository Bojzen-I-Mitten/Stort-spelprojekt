#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>

cbuffer MATERIAL_PROPERTIES
{
    float4 color : COLOR;
};

SamplerState StandardWrapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

DepthStencilState DepthTestGreater
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = GREATER;
};

RasterizerState Rasterizer
{
    FillMode = SOLID;
    CullMode = BACK;
    FrontCounterClockWise = TRUE;
    DepthClipEnable = FALSE;
};


struct v2f
{
    float4 vertex : SV_POSITION;
};

v2f vert(appdata_thomas v)
{
    v2f o;

    float3 posL = v.vertex;

    o.vertex = ThomasObjectToClipPos(posL);

    return o;
}

float4 frag(v2f input) : SV_TARGET
{
    return saturate(color);
}

technique11 Standard
{
    pass P0
    {
		VERT(vert());
        SetGeometryShader(NULL);
		FRAG(frag());
        SetDepthStencilState(DepthTestGreater, 0);
        SetRasterizerState(Rasterizer);
    }

}