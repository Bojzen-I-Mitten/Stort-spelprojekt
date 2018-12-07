#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasLights.hlsl>

Texture2D diffuseTex;
Texture2D normalTex : NORMALTEXTURE;
Texture2D specularTex;
Texture2D shirtPattern;

cbuffer MATERIAL_PROPERTIES
{
    float4 color : COLOR;
    float smoothness : MATERIALSMOOTHNESSFACTOR;
    float4 uvTiling : UVTILING; // (uv tiling x, uv tiling y, uv offset x, uv offset y)
};


SamplerState StandardWrapSampler
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
    MaxAnisotropy = 16;
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
struct appdata_chad_skin
{
    float3 vertex : POSITION;
    float2 texcoord : TEXCOORD0;
    float2 shirtCoord : TEXCOORD1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 boneWeight : BONEWEIGHTS;
    int4 boneIndex : BONEINDICES;
};

struct v2f
{
    float4 vertex : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float3x3 TBN : TBN;
    float2 texcoord : TEXCOORD0;
    float2 shirtCoord : TEXCOORD1;
};


float biTangentSign(float3 norm, float3 tang, float3 bitang)
{
    return dot(cross(norm, tang), bitang) > 0.f ? 1.f : -1.f;
}

v2f vert(appdata_chad_skin v)
{
    v2f o;
    
    float4 posL = float4(v.vertex, 1.f);
    float3 normalL = v.normal;
    float3 tangentL = v.tangent;
    float bisign = biTangentSign(v.normal, v.tangent, v.bitangent);
    ThomasSkinVertex(posL, normalL, v.boneWeight, v.boneIndex);
    float3 bitangL = cross(normalL, tangentL) * bisign;
    o.vertex = ThomasObjectToClipPos(posL);
    o.worldPos = ThomasObjectToWorldPos(posL);

    float3 tangent = ThomasObjectToWorldDir(tangentL);
    float3 bitangent = ThomasObjectToWorldDir(bitangL);
    float3 normal = ThomasObjectToWorldDir(normalL);

    o.TBN = float3x3(tangent, bitangent, normal);
    
	o.texcoord = v.texcoord * uvTiling.xy + uvTiling.zw;
    o.shirtCoord = v.shirtCoord;

    return o;
}

float4 frag(v2f input) : SV_TARGET
{
    // Base texture
    float3 diffuse = diffuseTex.Sample(StandardWrapSampler, input.texcoord);
    // TeamColor
    diffuse *= color;
    // Mix in shirt pattern
    float3 shirtDiffuse = shirtPattern.Sample(StandardWrapSampler, input.shirtCoord);
    diffuse = lerp(shirtDiffuse, diffuse, shirtDiffuse.g);
    //if(shirtDiffuse.g < 0.75f)
    //    diffuse = shirtDiffuse;

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