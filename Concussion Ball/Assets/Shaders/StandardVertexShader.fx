#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasLights.hlsl>

cbuffer MATERIAL_PROPERTIES
{
    float4 color : COLOR;
    float specular : COLOR;
    float smoothness : MATERIALSMOOTHNESSFACTOR;
};


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

BlendState AlphaBlendingOff
{
    BlendEnable[0] = FALSE;
    SrcBlend = ONE;
    DestBlend = ZERO;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0f;

};

struct v2f
{
    float4 vertex : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float4 color : COLOR0;
    float3 normal : NORMAL0;
};

v2f vert(appdata_thomas_vertex v)
{
    v2f o;

    float3 posL = v.vertex;

    o.vertex = ThomasObjectToClipPos(posL);
    o.worldPos = ThomasObjectToWorldPos(posL);
    o.color = v.color;
    o.normal = v.normal;
    return o;
}

float4 frag(v2f input) : SV_TARGET
{
    float3 diffuse = input.color.xyz * color.xyz;  

    // Calculate light
    diffuse = AddLights(input.worldPos.xyz, input.normal, diffuse, specular, smoothness + 1);
    
    // Gamma correction
    diffuse.xyz = pow(diffuse, 0.4545454545f);
    
    return saturate(float4(diffuse, 1.0f));
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
        SetBlendState(AlphaBlendingOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

}