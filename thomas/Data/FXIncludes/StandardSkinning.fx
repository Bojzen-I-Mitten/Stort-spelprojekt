#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasLights.hlsl>

Texture2D diffuseTex;
Texture2D normalTex : NORMALTEXTURE;
Texture2D specularTex;

cbuffer MATERIAL_PROPERTIES
{
    float4 color : COLOR;
    float smoothness : MATERIALSMOOTHNESSFACTOR;
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

v2f vert(appdata_thomas_skin v, uint instanceID : SV_INSTANCEID)
{
    v2f o;
    
    float4 posL = float4(v.vertex, 1.f);
    float3 normalL = v.normal;
    ThomasSkinVertex(posL, normalL, v.boneWeight, v.boneIndex);
	
    o.vertex = ThomasObjectToClipPos(posL, instanceID);
    o.worldPos = ThomasObjectToWorldPos(posL, instanceID);

    float3 tangent = ThomasObjectToWorldDir(v.tangent, instanceID);
    float3 bitangent = ThomasObjectToWorldDir(v.bitangent, instanceID);
    float3 normal = ThomasObjectToWorldDir(v.normal, instanceID);

    o.TBN = float3x3(tangent, bitangent, normal);
    
    o.texcoord = v.texcoord;
    return o;
}


float4 frag(v2f input) : SV_TARGET
{
    float3 diffuse = diffuseTex.Sample(StandardWrapSampler, input.texcoord);
    diffuse *= color.xyz;
    float3 normal = normalTex.Sample(StandardWrapSampler, input.texcoord);
    float specularMapFactor = specularTex.Sample(StandardWrapSampler, input.texcoord);

    normal.xy = normal.xy * 2.0f - 1.0f;
    normal = normalize(normal);
    normal = normalize(mul(normal, input.TBN));

    diffuse = AddLights(input.worldPos.xyz, normal, diffuse, specularMapFactor, smoothness + 1); // Calculate light
    diffuse.xyz = pow(diffuse, 0.4545454545f); // Gamma correction

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