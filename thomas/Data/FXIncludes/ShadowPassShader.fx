#pragma warning(disable: 4717) // removes effect deprecation warning.

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
    MultisampleEnable = FALSE;
};

cbuffer LightMatrix
{
    float4x4 lightMatrixVP;
    int animate;
};

inline float4 ObjectToLightClipPos(in float3 pos)//, uint lightIndex)//temp dirlight id
{
    return mul(lightMatrixVP, mul(thomas_ObjectToWorld, float4(pos, 1.0)));
}

float biTangentSign(float3 norm, float3 tang, float3 bitang)
{
    return dot(cross(norm, tang), bitang) > 0.f ? 1.f : -1.f;
}

struct v2f
{
    float4 vertex : SV_POSITION;
};

v2f vert(appdata_thomas_skin v)
{
    v2f o;

    float4 posL = float4(v.vertex, 1.0);
    if (animate >= 1)
    {
        ThomasSkinVertex(posL, v.normal, v.boneWeight, v.boneIndex);
    }
    

    o.vertex = ObjectToLightClipPos(posL.xyz);

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