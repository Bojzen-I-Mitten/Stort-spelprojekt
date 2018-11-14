#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasLights.hlsl>
#include <ThomasCG.hlsl>

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

RasterizerState RasterizerSolid
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
    
    o.vertex = WorldToLightClipPos(posL);

    return o;
}

float4 frag(v2f input) : SV_TARGET
{
    
    return float4(1, 1, 1, 1);
}

technique11 Standard
{
    pass P0
    {
		VERT(vert());
        SetGeometryShader(NULL);
		FRAG(frag());
        SetDepthStencilState(EnableDepth, 0);
        SetRasterizerState(RasterizerSolid);
    }

}