#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasLights.hlsl>

Texture2D diffuseTex;
Texture2D normalTex : NORMALTEXTURE;
Texture2D specularTex;
Texture2D maskTex;

cbuffer MATERIAL_PROPERTIES
{
    float4 color : COLOR;
    float smoothness : MATERIALSMOOTHNESSFACTOR;
    float4 uvTiling : UVTILING; // (uv tiling x, uv tiling y, uv offset x, uv offset y)
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
    float2 texcoord : TEXCOORD0;
};


float biTangentSign(float3 norm, float3 tang, float3 bitang)
{
    return dot(cross(norm, tang), bitang) > 0.f ? 1.f : -1.f;
}

v2f vert(appdata_thomas v)
{
    v2f o;

    o.vertex = ThomasObjectToClipPos(v.vertex);
    o.worldPos = ThomasObjectToWorldPos(v.vertex);
    
    float bisign = BiTangentSign(v.normal, v.tangent, v.bitangent);
    float3 tangentL = v.tangent;
    float3 bitangL = cross(v.normal, tangentL) * bisign;

    float3 tangent = ThomasObjectToWorldDir(tangentL);
    float3 bitangent = ThomasObjectToWorldDir(bitangL);
    float3 normal = ThomasObjectToWorldDir(v.normal);

    o.TBN = float3x3(tangent, bitangent, normal);
    
    o.texcoord = v.texcoord;

    return o;
}

float4 frag(v2f input) : SV_TARGET
{
    float3 diffuse = diffuseTex.Sample(StandardWrapSampler, input.texcoord);
    //diffuse *= color.xyz;
    diffuse += maskTex.Sample(StandardWrapSampler, input.texcoord) * color;

    float3 normal = normalTex.Sample(StandardWrapSampler, input.texcoord);
    float specularMapFactor = specularTex.Sample(StandardWrapSampler, input.texcoord);
    
    normal.xy = normal.xy * 2.0f - 1.0f;
    normal = normalize(normal);
    normal = normalize(mul(normal, input.TBN));

    
    diffuse = AddLights(input.worldPos.xyz, normal, diffuse, specularMapFactor, smoothness + 1);        // Calculate light
    
    
    diffuse.xyz = pow(diffuse, 0.4545454545f);                                                          // Gamma correction


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
        SetRasterizerState(TestRasterizer);
        SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

}