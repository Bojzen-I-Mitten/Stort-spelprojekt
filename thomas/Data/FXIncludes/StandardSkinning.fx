#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasLights.hlsl>
Texture2D ambientTex;


SamplerState StandardWrapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer MATERIAL_PROPERTIES
{
    
    float4 wow : COLOR;
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

struct v2f
{
    float4 vertex : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

v2f vert(appdata_thomas_skin v)
{
    v2f o;
    
    float4 posL = float4(v.vertex, 1.f);
    float3 normalL = v.normal;
    ThomasSkinVertex(posL, normalL, v.boneWeight, v.boneIndex);
	

    o.vertex = ThomasObjectToClipPos(posL);
    o.worldPos = ThomasObjectToWorldPos(posL);
    o.normal = normalize(ThomasWorldToObjectDir(normalL));
    o.texcoord = v.texcoord;
    return o;
}


float4 frag(v2f input) : SV_TARGET
{
    return saturate(wow / 255.0f + float4(AddLights(input.worldPos.xyz, input.normal), 1.0f));
}


technique11 Standard
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