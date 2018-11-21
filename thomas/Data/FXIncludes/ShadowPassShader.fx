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

cbuffer LightMatrices
{
    float4x4 lightMatrixVP;
};

inline float4 ObjectToLightClipPos(in float3 pos)//, uint lightIndex)//temp dirlight id
{
    return mul(lightMatrixVP, mul(thomas_ObjectToWorld, float4(pos, 1.0)));
}

struct v2f
{
    float4 vertex : SV_POSITION;
};

v2f vert(appdata_thomas v)
{
    v2f o;

    float3 posL = v.vertex;
    
    o.vertex = ObjectToLightClipPos(posL);

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