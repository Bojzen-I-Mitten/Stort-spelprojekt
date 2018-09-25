#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasLights.hlsl>

Texture2D diffuseTex;
Texture2D normalTex;
Texture2D specularTex;

cbuffer MATERIAL_PROPERTIES
{
    float4 color : COLOR;
    float smoothness;
};


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

struct v2f
{
    float4 vertex : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float3x3 TBN : TBN;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

v2f vert(appdata_thomas v)
{
    v2f o;

    float3 posL = v.vertex;

    o.vertex = ThomasObjectToClipPos(posL);
    o.worldPos = ThomasObjectToWorldPos(posL);
    
    
    float3 tangent = ThomasObjectToWorldDir(v.tangent);
    float3 bitangent = ThomasObjectToWorldDir(v.bitangent);
    float3 normal = ThomasObjectToWorldDir(v.normal);

    o.TBN = float3x3(tangent, bitangent, normal);
    o.normal = normal;
    
    o.texcoord = v.texcoord;
    return o;
}

float4 frag(v2f input) : SV_TARGET
{
    float4 diffuse = diffuseTex.Sample(StandardWrapSampler, input.texcoord);
    float3 normal = normalTex.Sample(StandardWrapSampler, input.texcoord);
    float specularMapFactor = specularTex.Sample(StandardWrapSampler, input.texcoord);

    normal = normalize(normal * 2.0f - 1.0f);
    normal = normalize(mul(normal, input.TBN));

    diffuse.xyz = pow(diffuse.xyz * color.xyz / 255.0f + AddLights(input.worldPos.xyz, normal, specularMapFactor, smoothness + 1), 0.4545454545f); //gamma correction

    return saturate(diffuse);
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